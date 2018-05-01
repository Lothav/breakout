//
// Created by luiz0tavio on 5/1/18.
//

#include "Window.hpp"

SDL_Window* Renderer::Window::getWindow()
{
    return this->window;
}

SDL_Renderer* Renderer::Window::getRenderer()
{
    return this->renderer;
}