#include <functional>
#include <memory>
#include <iostream>

#include "renderer/Window.hpp"
#include "renderer/Shader.hpp"
#include "renderer/Vertex.hpp"
#include "renderer/Uniform.hpp"
#include "entity/Player.hpp"
#include "renderer/Meshes.hpp"
#include "entity/Block.hpp"
#include "entity/Ball.hpp"

#include <ft2build.h>
#include FT_FREETYPE_H
#include "renderer/Text.hpp"

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define TOTAL_BLOCKS 33

FT_Face face;

int main(int argc, char* argv[]) {

    Memory::Provider::initPools();

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Could not initialize sdl2: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    if (IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG) == 0) {
        std::cerr << "Could not initialize IMG's flags" << std::endl;
        return EXIT_FAILURE;
    }

    FT_Library ft;

    if(FT_Init_FreeType(&ft)) {
        fprintf(stderr, "Could not init freetype library\n");
        return 1;
    }

    if(FT_New_Face(ft, "fonts/VanillaGalaxies.ttf", 0, &face)) {
        fprintf(stderr, "Could not open font\n");
        return 1;
    }

    FT_Set_Pixel_Sizes(face, 0, 48);

    // Unique Ptr's scope
    {
        auto window = std::make_unique<Renderer::Window>(SCREEN_WIDTH, SCREEN_HEIGHT);

        auto mainContext = SDL_GL_CreateContext(window->getWindow());

        GLenum err = glewInit();
        if (err != GLEW_OK) {
            std::cerr << "Glew is not ok =(" << std::endl;
            exit(1);
        }

        if (!GLEW_VERSION_2_0) {
            std::cerr << "Error: your graphic card does not support OpenGL 2.0" << std::endl;
            return EXIT_FAILURE;
        }

        auto shader = std::make_unique<Renderer::Shader>();
        shader->createGraphicShader(GL_VERTEX_SHADER, "default.vert");
        shader->createGraphicShader(GL_FRAGMENT_SHADER, "default.frag");
        shader->link();

        auto meshes = std::make_unique<Renderer::Meshes>();

        auto *SDL_window = window->getWindow();

        // Init Textures

        float sx = 2.0f / SCREEN_WIDTH;
        float sy = 2.0f / SCREEN_HEIGHT;
        auto text_velocity     = std::make_unique<Renderer::Text>(-1 + 8 * sx, 1 - 50 * sy-1.8f, sx, sy, face);
        auto text_lives        = std::make_unique<Renderer::Text>(-1 + 8 * sx+1.75f, 1 - 50 * sy-1.8f, sx, sy, face);
        auto text_paddle_info  = std::make_unique<Renderer::Text>(sx - 0.98f, 1 - 50 * sy-1.0, sx, sy, face);
        auto text_paddle_vel   = std::make_unique<Renderer::Text>(sx - 0.98f, 1 - 50 * sy-1.05, sx, sy, face);
        auto text_ball_info    = std::make_unique<Renderer::Text>(sx - 0.98f, 1 - 50 * sy-1.1, sx, sy, face);
        auto text_ball_vel     = std::make_unique<Renderer::Text>(sx - 0.98f, 1 - 50 * sy-1.15, sx, sy, face);
        auto text_count_blocks = std::make_unique<Renderer::Text>(sx - 0.98f, 1 - 50 * sy-1.2, sx, sy, face);

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        int uniform_id = 0;
        auto blocks_texture = std::make_unique<Renderer::Uniform>(++uniform_id);
        blocks_texture->loadTexture("./data/breakout-blocks-texture.jpg", GL_RGB);

        auto paddle_texture = std::make_unique<Renderer::Uniform>(++uniform_id);
        paddle_texture->loadTexture("./data/brown_platform.png", GL_RGBA);

        auto ball_texture_0 = std::make_unique<Renderer::Uniform>(++uniform_id);
        ball_texture_0->loadTexture("./data/spike_ball_0.png", GL_RGBA);

        auto ball_texture_1 = std::make_unique<Renderer::Uniform>(++uniform_id);
        ball_texture_1->loadTexture("./data/spike_ball_1.png", GL_RGBA);

        auto background_texture_0 = std::make_unique<Renderer::Uniform>(++uniform_id);
        background_texture_0->loadTexture("./data/forest_background_0.png", GL_RGB);

        auto background_texture_1 = std::make_unique<Renderer::Uniform>(++uniform_id);
        background_texture_1->loadTexture("./data/forest_background_1.png", GL_RGB);

        auto background_texture_2 = std::make_unique<Renderer::Uniform>(++uniform_id);
        background_texture_2->loadTexture("./data/forest_background_2.png", GL_RGB);


        Entity::Paddle* player1 = nullptr;
        Entity::Ball* ball = nullptr;
        Entity::Block* blocks[TOTAL_BLOCKS];

        float velocity = 0;
        int lives = 3;
        bool pause = true;
        bool show_info = false;

        auto restart = [&]() {
            meshes->clear();
            // Init ball, paddle and blocks
            player1 = new Entity::Paddle(0.0f, -0.8f, 0.24f, 0.06f);
            ball = new Entity::Ball(0.0f, 0.0f, 0.005f);
            std::srand(static_cast<unsigned int>(std::time(nullptr)));
            for (int i = 0; i < TOTAL_BLOCKS; i++) {
                blocks[i] = new Entity::Block(-.8f + BLOCK_WIDTH * (i % 11), 0.8f - BLOCK_HEIGHT * static_cast<float>(floor(i / 11)));
            }
            lives = 3;
            pause = true;
            show_info = false;
        };
        restart();

        int ball_speed = 0;
        int text_count = 0;
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

                // Restart if hit 'r'
                if (e.type == SDL_KEYDOWN) {
                    if (e.key.keysym.sym == SDLK_r) {
                        restart();
                    }
                }

                // Restart if hit left mouse button
                if(e.type == SDL_MOUSEBUTTONDOWN) {
                    if (e.button.button == SDL_BUTTON_LEFT) {
                        pause = !pause;
                        show_info = false;
                    } else if (e.button.button == SDL_BUTTON_RIGHT) {
                        pause = !pause;
                        show_info = true;
                    }
                }
            }

            if (!pause) {
                ball_speed++;
                if(ball_speed % 250 == 0) {
                    ball->increaseSpeed(0.001f);
                    ball_speed = 0;
                }

                player1->move(velocity, .0f);

                auto player_vertices = player1->getArrayVertices();
                if(!ball->checkWallCollision()){
                    ball = new Entity::Ball(0.0f, 0.0f, ball->getSpeed());
                    lives--;
                    if (lives == 0) {
                        restart();
                    }
                }
                ball->checkObjectCollision(player_vertices, player1->isMoving() ? std::abs(velocity*100) : 1.0f);
                ball->moveBall();
            }

            // Set screen to black
            glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            auto vertex = std::make_unique<Renderer::Vertex>(shader->getShaderProgram());
            shader->use();

            // Set Background Textures active and Draw

            if (pause) {
                background_texture_2->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_TEXTURE);
                background_texture_2->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_MAT4);
            } else {
                if (text_count < 200) {
                    background_texture_0->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_TEXTURE);
                    background_texture_0->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_MAT4);
                } else {
                    background_texture_1->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_TEXTURE);
                    background_texture_1->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_MAT4);
                }

            }

            std::vector<GLfloat> background_vertices_ = {
                    // Triangles                    //     c ___
                     1.0f,  1.0f, 0.0f, 1.0f, 0.0f, // a    |  /a
                    -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // b    | /
                    -1.0f,  1.0f, 0.0f, 0.0f, 0.0f, // c   b|/
                     1.0f,  1.0f, 0.0f, 1.0f, 0.0f, // d       /|d
                    -1.0f, -1.0f, 0.0f, 0.0f, 1.0f, // e      / |
                     1.0f, -1.0f, 0.0f, 1.0f, 1.0f, // f    e/__|f
            };

            meshes->clear();
            meshes->insert(background_vertices_.data(), ball->getTotalVertices());

            vertex->setBufferData(meshes->getByteSize(), meshes->get());
            auto count_meshes = static_cast<GLsizei>(meshes->getSize());
            glDrawArrays(GL_TRIANGLES, 0, count_meshes);


            // Set Block Textures active and Draw

            blocks_texture->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_TEXTURE);
            blocks_texture->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_MAT4);

            bool block_hit = false;
            meshes->clear();
            for (int i = 0; i < TOTAL_BLOCKS; ++i) {
                if (blocks[i]->isAlive()) {
                    if (!block_hit && ball->checkObjectCollision(blocks[i]->getArrayVertices(), 1.0f)) {
                        blocks[i]->changeVisibility();
                        block_hit = true;
                        if (blocks[i]->isSpecial()) {
                            player1 = new Entity::Paddle(player1->getPos()[0], -0.8f, 0.44f, 0.06f);
                        }
                    }
                    meshes->insert(blocks[i]->getVertices(), blocks[i]->getTotalVertices());
                }
            }

            vertex->setBufferData(meshes->getByteSize(), meshes->get());
            count_meshes = static_cast<GLsizei>(meshes->getSize());
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

            if (pause) {
                ball_texture_0->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_TEXTURE);
                ball_texture_0->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_MAT4);
            } else {
                if (text_count <= 200) {
                    ball_texture_0->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_TEXTURE);
                    ball_texture_0->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_MAT4);
                } else {
                    ball_texture_1->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_TEXTURE);
                    ball_texture_1->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_MAT4);
                }
            }

            meshes->clear();
            meshes->insert(ball->getVertices(), ball->getTotalVertices());

            vertex->setBufferData(meshes->getByteSize(), meshes->get());
            count_meshes = static_cast<GLsizei>(meshes->getSize());
            glDrawArrays(GL_TRIANGLES, 0, count_meshes);


            // Set/Prepare Text and Draw

            std::string text_velocity_str = "Velocity: " + std::to_string(static_cast<int>((velocity*10000) / 4)) + "%";
            text_velocity->prepare(32);
            text_velocity->draw(text_velocity_str);

            std::string text_lives_str = "Lives: " + std::to_string(lives);
            text_lives->prepare(32);
            text_lives->draw(text_lives_str);

            if (show_info) {
                text_paddle_info->prepare(24);
                text_paddle_info->draw("Paddle Pos: (" +  std::to_string(player1->getPos()[0]) + ", "+std::to_string(player1->getPos()[1])+")");
                text_paddle_vel->prepare(24);
                text_paddle_vel->draw("Paddle Speed: (" +  std::to_string(velocity)+")");
                text_ball_info->prepare(24);
                text_ball_info->draw("Ball Pos: (" +  std::to_string(ball->getPos()[0]) + ", "+std::to_string(ball->getPos()[1])+")");
                text_ball_vel->prepare(24);
                text_ball_vel->draw("Ball Speed: (" +  std::to_string(ball->getSpeed()) + ")");

                int blocks_alive_count = 0;
                for (int i = 0; i < TOTAL_BLOCKS; ++i) {
                    if (blocks[i]->isAlive()) {
                        blocks_alive_count++;
                    }
                }
                text_count_blocks->prepare(24);
                text_count_blocks->draw("Blocks Remaining: (" +  std::to_string(blocks_alive_count) + ")");

                pause = true;
            }

            // Swap Window
            SDL_GL_SwapWindow(SDL_window);

            text_count++;
            if(text_count >= 400){
                text_count = 0;
            }

            // Adjust FPS
            if (1000/60 > (SDL_GetTicks() - start)) {
                SDL_Delay(1000/60 - (SDL_GetTicks()-start));
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
