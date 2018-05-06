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

#include <ft2build.h>
#include FT_FREETYPE_H

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

#define TOTAL_BLOCKS 33

FT_Face face;

GLint attribute_coord;
GLint uniform_tex;
GLint uniform_color;

GLuint vbo;

struct point {
    GLfloat x;
    GLfloat y;
    GLfloat s;
    GLfloat t;
};

void render_text(const char *text, float x, float y, float sx, float sy)
{
    const char *p;
    FT_GlyphSlot g = face->glyph;

    /* Create a texture that will be used to hold one "glyph" */
    GLuint tex;

    glActiveTexture(GL_TEXTURE0);
    glGenTextures(1, &tex);
    glBindTexture(GL_TEXTURE_2D, tex);
    glUniform1i(uniform_tex, 0);

    /* We require 1 byte alignment when uploading texture data */
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    /* Clamping to edges is important to prevent artifacts when scaling */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    /* Linear filtering usually looks best for text */
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    /* Set up the VBO for our vertex data */
    glEnableVertexAttribArray(attribute_coord);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(attribute_coord, 4, GL_FLOAT, GL_FALSE, 0, 0);

    /* Loop through all characters */
    for (p = text; *p; p++) {
        /* Try to load and render the character */
        if (FT_Load_Char(face, *p, FT_LOAD_RENDER))
            continue;

        /* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
        glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);

        /* Calculate the vertex and texture coordinates */
        float x2 = x + g->bitmap_left * sx;
        float y2 = -y - g->bitmap_top * sy;
        float w = g->bitmap.width * sx;
        float h = g->bitmap.rows * sy;

        point box[4] = {
                {x2, -y2, 0, 0},
                {x2 + w, -y2, 1, 0},
                {x2, -y2 - h, 0, 1},
                {x2 + w, -y2 - h, 1, 1},
        };

        /* Draw the character on the screen */
        glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
        glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

        /* Advance the cursor to the start of the next character */
        x += (g->advance.x >> 6) * sx;
        y += (g->advance.y >> 6) * sy;
    }

    glDisableVertexAttribArray(attribute_coord);
    glDeleteTextures(1, &tex);
}

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

        auto shader_text = std::make_unique<Renderer::Shader>();
        shader_text->createGraphicShader(GL_VERTEX_SHADER, "text.vert");
        shader_text->createGraphicShader(GL_FRAGMENT_SHADER, "text.frag");
        shader_text->link();

        uniform_tex = glGetUniformLocation(shader_text->getShaderProgram(), "tex");
        attribute_coord = glGetAttribLocation(shader_text->getShaderProgram(), "coord");
        uniform_color = glGetUniformLocation(shader_text->getShaderProgram(), "color");

        glGenBuffers(1, &vbo);

        auto shader = std::make_unique<Renderer::Shader>();
        shader->createGraphicShader(GL_VERTEX_SHADER, "default.vert");
        shader->createGraphicShader(GL_FRAGMENT_SHADER, "default.frag");
        shader->link();

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


        int ball_speed = 0;
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
                    }
                }
            }
            if (pause) {
                return true;
            }

            ball_speed++;
            if(ball_speed % 250 == 0) {
                ball->increaseSpeed(0.001f);
                ball_speed = 0;
            }

            shader->use();

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

            shader_text->use();

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            GLfloat black[4] = { 1, 1, 1, 1 };

            /* Set font size to 48 pixels, color to black */
            FT_Set_Pixel_Sizes(face, 0, 48);
            glUniform4fv(uniform_color, 1, black);

            float sx = 2.0 / SCREEN_WIDTH;
            float sy = 2.0 / SCREEN_HEIGHT;

            render_text("The Quick Brown Fox Jumps Over The Lazy Dog",
                        -1 + 8 * sx,   1 - 50 * sy,    sx, sy);
            render_text("The Misaligned Fox Jumps Over The Lazy Dog",
                        -1 + 8.5 * sx, 1 - 100.5 * sy, sx, sy);


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