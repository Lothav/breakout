#ifndef ONYX_WINDOW_HPP
#define ONYX_WINDOW_HPP

#include <SDL2/SDL.h>
#include <iostream>
#include "../memory/Provider.hpp"

namespace Renderer {

    class Window
    {

    private:

        SDL_Window*     window;
        SDL_Renderer*   renderer;

    public:

        void* operator new (std::size_t size)
        {
            return Memory::Provider::getMemory(Memory::PoolType::POOL_TYPE_GENERIC, size);
        }

        void  operator delete (void* ptr, std::size_t)
        {
            std::cerr << "call delete for non-delete heap memory!" << std::endl;
        }

        Window(const int screen_width, const int screen_height) : window(nullptr), renderer(nullptr)
        {
            if (SDL_Init(SDL_INIT_VIDEO) < 0) {
                std::cout << "Could not initialize sdl2: " << SDL_GetError();
                return;
            }
            this->window = SDL_CreateWindow(
                "Onyx Launcher",
                SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
                screen_width, screen_height,
                SDL_WINDOW_SHOWN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_INPUT_FOCUS
            );
            if (this->window == nullptr) {
                fprintf(stderr, "Could not create window: %s\n", SDL_GetError());
                return;
            }

            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 2);
            SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
            SDL_GL_SetSwapInterval(0);
            SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
            SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

            this->renderer = SDL_CreateRenderer(this->window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_TARGETTEXTURE);
        }

        ~Window()
        {
            SDL_DestroyRenderer(this->renderer);
            SDL_DestroyWindow(this->window);
            SDL_Quit();
        }

        SDL_Window* getWindow() {
            return this->window;
        }

        SDL_Renderer* getRenderer() {
            return this->renderer;
        }
    };

}


#endif //ONYX_WINDOW_HPP
