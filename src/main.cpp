#include <functional>

#include "renderer/Window.hpp"
#include "renderer/Shader.hpp"
#include "renderer/Vertex.hpp"
#include "renderer/Uniform.hpp"
#include "renderer/Player.hpp"
#include "renderer/Meshes.hpp"
#include "renderer/Block.hpp"

/* This binary should check for updates and update the data files, the client and itself for non-console platforms (If internet connection)
 * Update server should be just an SFTP or something, can have the key inside the source code.
 * 1. Check for updates
 * 2. Check datafiles integrity against downloaded md5 references
 * References
 *   SelfUpdate (Windows): https://stackoverflow.com/questions/7483230/how-can-i-run-an-app-automatic-after-restart
 *	            (Linux  ): https://google.com
 */

#define SCREEN_WIDTH 800
#define SCREEN_HEIGHT 600

int main(int argc, char* args[]) {

    Memory::Provider::initPools();

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "Could not initialize sdl2: " << SDL_GetError() << std::endl;
        return EXIT_FAILURE;
    }

    if (IMG_Init(IMG_INIT_JPG) == 0) {
        std::cerr << "Could not initialize IMG's flags" << std::endl;
        return EXIT_FAILURE;
    }

    auto* window = new Renderer::Window(SCREEN_WIDTH, SCREEN_HEIGHT);

    auto* shader = new Renderer::Shader();
    shader->createGraphicShader(GL_VERTEX_SHADER, "default.vert");
    shader->createGraphicShader(GL_FRAGMENT_SHADER, "default.frag");
    shader->beginProgram();

    auto* texture = new Renderer::Uniform();
    texture->loadTexture("./data/breakout-blocks-texture.jpg");
    texture->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_TEXTURE);
    texture->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_MAT4);

    auto* vertex = new Renderer::Vertex(shader->getShaderProgram());
    auto* meshes = new Renderer::Meshes();

    auto* player1 = new Renderer::Player(0.0f, -0.8f, 0.24f, 0.06f);

    std::srand(std::time(nullptr));
    std::array<Renderer::Block *, 33> blocks;
    for (int i = 0; i < 33; ++i) {
        blocks[i] = new Renderer::Block(
            -.8f + BLOCK_WIDTH * (i % 11),
            0.8f - BLOCK_HEIGHT * floor(i / 11)
        );
    }

    auto* SDL_window = window->getWindow();

    float velocity = 0;
    auto loop =  [&] () -> bool
    {
        player1->move(velocity, .0f);

        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            auto mouseX = e.motion.x;

            if (mouseX > SCREEN_WIDTH/2) {
                velocity = (mouseX - SCREEN_WIDTH/2.0f) / 1000000.0f;
            } else if (mouseX > 0 && mouseX < SCREEN_WIDTH/2) {
                velocity = -(SCREEN_WIDTH/2.0f - mouseX) / 1000000.0f;
            }

            if(e.type == SDL_QUIT) return false;
        }
        meshes->clear();
        meshes->insert(player1->getVertices(), player1->getTotalVertices());

        for (int i = 0; i < 33; ++i) {
            meshes->insert(blocks[i]->getVertices(), blocks[i]->getTotalVertices());
            blocks[i];
        }
        vertex->setBufferData(meshes->getByteSize(), meshes->get());
        // Set screen to black
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        auto count_meshes = static_cast<GLsizei>(meshes->getSize());
        glDrawArrays(GL_TRIANGLES, 0, count_meshes);
        SDL_GL_SwapWindow(SDL_window);
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

    Memory::Provider::destroyPools();
    return EXIT_SUCCESS;
}