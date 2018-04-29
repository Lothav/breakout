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
    //texture->loadTexture("./data/launcher.png");
    texture->loadTexture("./data/breakout-blocks-texture.jpg");
    texture->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_TEXTURE);
    texture->setUniform(shader->getShaderProgram(), UNIFORM_TYPE_MAT4);

    auto* vertex = new Renderer::Vertex(shader->getShaderProgram());
    auto* meshes = new Renderer::Meshes();

    auto* player1 = new Renderer::Player(0.0f, 0.0f, 0.08f, 0.16f);

    std::srand(std::time(nullptr));
    std::array<Renderer::Block *, 12> blocks;
    for (int i = 0; i < 12; ++i) {
        blocks[i] = new Renderer::Block(-.8f + BLOCK_WIDTH * (i % 6), - 0.4f  + BLOCK_HEIGHT * floor(i / 6));
    }

    auto* SDL_window = window->getWindow();

    auto loop =  [&] () -> bool
    {
        SDL_Event e;
        while(SDL_PollEvent(&e))
        {
            if(e.type == SDL_QUIT) return false;
            if(e.type == SDL_KEYDOWN)
            {
                switch( e.key.keysym.sym )
                {
                    case SDLK_UP:
                        player1->move(.0f, .10f);
                        std::cout << "Key Up pressed" << std::endl;
                        break;
                    case SDLK_DOWN:
                        player1->move(.0f, -.10f);
                        std::cout << "Key Down pressed" << std::endl;
                        break;
                    case SDLK_LEFT:
                        player1->move(-.10f, .0f);
                        std::cout << "Key Left pressed" << std::endl;
                        break;
                    case SDLK_RIGHT:
                        player1->move(.10f, .0f);
                        std::cout << "Key Right pressed" << std::endl;
                        break;

                    case SDLK_w:
                        texture->translateCamera(shader->getShaderProgram(), glm::vec3(0.0f, 0.1f, 0.0f));
                        break;
                    case SDLK_a:
                        texture->translateCamera(shader->getShaderProgram(), glm::vec3(-0.1f, 0.0f, 0.0f));
                        break;
                    case SDLK_s:
                        texture->translateCamera(shader->getShaderProgram(), glm::vec3(0.0f, -0.1f, 0.0f));
                        break;
                    case SDLK_d:
                        texture->translateCamera(shader->getShaderProgram(), glm::vec3(0.1f, 0.0f, 0.0f));
                        break;

                    default:
                        std::cout << "Non-arrow Key pressed" << std::endl;
                }
            }
        }
        meshes->clear();
        meshes->insert(player1->getVertices(), player1->getTotalVertices());

        for (int i = 0; i < 12; ++i) {
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