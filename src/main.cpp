#include <functional>
#include <memory>

#include "renderer/Window.hpp"
#include "renderer/Shader.hpp"
#include "renderer/Vertex.hpp"
#include "renderer/Uniform.hpp"
#include "entity/Player.hpp"
#include "renderer/Meshes.hpp"
#include "entity/Block.hpp"
#include "entity/Ball.hpp"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define TOTAL_BLOCKS 33

int main(int argc, char* argv[]) {

    Memory::Provider::initPools();

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Could not initialize sdl2: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    if (IMG_Init(IMG_INIT_JPG) == 0) {
        std::cerr << "Could not initialize IMG's flags" << std::endl;
        return EXIT_FAILURE;
    }

    // Unique Ptr's scope
    {
        auto window = std::make_unique<Renderer::Window>(SCREEN_WIDTH, SCREEN_HEIGHT);

        auto mainContext = SDL_GL_CreateContext(window->getWindow());

        GLenum err = glewInit();
        if (err != GLEW_OK) {
            std::cerr << "Glew is not ok =(" << std::endl;
            exit(1);
        }

        auto shader = std::make_unique<Renderer::Shader>();
        shader->createGraphicShader(GL_VERTEX_SHADER, "default.vert");
        shader->createGraphicShader(GL_FRAGMENT_SHADER, "default.frag");
        shader->beginProgram();

        auto vertex = std::make_unique<Renderer::Vertex>(shader->getShaderProgram());
        auto meshes = std::make_unique<Renderer::Meshes>();

        auto *SDL_window = window->getWindow();

        float velocity = 0;
        bool pause = false;
        bool first_frame = true;

        // Init Textures

        auto blocks_texture = std::make_unique<Renderer::Uniform>(0);
        blocks_texture->loadTexture("./data/breakout-blocks-texture.jpg", GL_RGB);

        auto paddle_texture = std::make_unique<Renderer::Uniform>(1);
        paddle_texture->loadTexture("./data/paddle.jpg", GL_RGB);

        auto ball_texture = std::make_unique<Renderer::Uniform>(2);
        ball_texture->loadTexture("./data/ball.jpg", GL_RGB);

        Entity::Paddle* player1 = nullptr;
        Entity::Ball* ball = nullptr;
        Entity::Block* blocks[TOTAL_BLOCKS];

        auto restart = [&]() {
            meshes->clear();
            // Init ball, paddle and blocks
            player1 = new Entity::Paddle(0.0f, -0.8f, 0.24f, 0.06f);
            ball = new Entity::Ball(0.0f, 0.0f, 0.24f);
            std::srand(static_cast<unsigned int>(std::time(nullptr)));
            for (int i = 0; i < TOTAL_BLOCKS; i++) {
                blocks[i] = new Entity::Block(-.8f + BLOCK_WIDTH * (i % 11), 0.8f - BLOCK_HEIGHT * static_cast<float>(floor(i / 11)));
            }
            first_frame = true;
        };
        restart();

        // Main Loop
        auto loop = [&]() -> bool {

            auto start = SDL_GetTicks();

            // Handle Input events
            SDL_Event e;
            while (SDL_PollEvent(&e)) {
                auto mouseX = e.motion.x;

                if (mouseX > SCREEN_WIDTH / 2) {
                    velocity = (mouseX - SCREEN_WIDTH / 2.0f) / 10000.0f;
                } else if (mouseX > 0 && mouseX < SCREEN_WIDTH / 2) {
                    velocity = -(SCREEN_WIDTH / 2.0f - mouseX) / 10000.0f;
                }

                if (e.type == SDL_QUIT || (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_q)) {
                    return false;
                }

                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_r) {
                        restart();
                    }
                }

                if(e.type == SDL_MOUSEBUTTONDOWN) {
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        pause = !pause;
                    }
                }
            }
            if (pause) {
                return true;
            }

            // Set screen to black
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            player1->move(velocity, .0f);

            auto player_vertices = player1->getArrayVertices();
            if(!ball->checkWallCollision()){
                restart();
            }
            ball->checkObjectCollision(player_vertices);
            ball->moveBall();


            // Set Block Textures active and Draw

            blocks_texture->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_TEXTURE);
            blocks_texture->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_MAT4);

            meshes->clear();
            for (int i = 0; i < TOTAL_BLOCKS; ++i) {
                if (blocks[i]->isAlive()) {
                    if (ball->checkObjectCollision(blocks[i]->getArrayVertices())) {
                        blocks[i]->changeVisibility();
                    }
                    meshes->insert(blocks[i]->getVertices(), blocks[i]->getTotalVertices());
                }
            }

            vertex->setBufferData(meshes->getByteSize(), meshes->get());
            auto count_meshes = static_cast<GLsizei>(meshes->getSize());
            glDrawArrays(GL_TRIANGLES, 0, count_meshes);


            // Set Paddle Textures active and Draw

            paddle_texture->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_TEXTURE);
            paddle_texture->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_MAT4);

            meshes->clear();
            meshes->insert(player1->getVertices(), player1->getTotalVertices());

            vertex->setBufferData(meshes->getByteSize(), meshes->get());
            count_meshes = static_cast<GLsizei>(meshes->getSize());
            glDrawArrays(GL_TRIANGLES, 0, count_meshes);


            // Set Ball Textures active and Draw

            ball_texture->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_TEXTURE);
            ball_texture->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_MAT4);

            meshes->clear();
            meshes->insert(ball->getVertices(), ball->getTotalVertices());

            vertex->setBufferData(meshes->getByteSize(), meshes->get());
            count_meshes = static_cast<GLsizei>(meshes->getSize());
            glDrawArrays(GL_TRIANGLES, 0, count_meshes);


            // Swap Window
            SDL_GL_SwapWindow(SDL_window);


            // Adjust FPS
            if (1000/60 > (SDL_GetTicks() - start)) {
                SDL_Delay(1000/60 - (SDL_GetTicks()-start));
            }

            if (first_frame) {
                pause = true;
                first_frame = false;
            }

#ifdef DEBUG
            return false;
#else
            return true;
#endif
        };

#ifdef __EMSCRIPTEN__
        emscripten_set_main_loop(loop, 0, true);
#else
        while(loop());
#endif
        SDL_GL_DeleteContext(mainContext);
    }

    Memory::Provider::destroyPools();
    SDL_Quit();
    glEnd();
    return EXIT_SUCCESS;
}