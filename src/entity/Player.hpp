//
// Created by luiz0tavio on 4/29/18.
//

#ifndef ONYX_PLAYER_H
#define ONYX_PLAYER_H

#include <GLES3/gl3.h>
#include <algorithm>
#include <array>
#include "../memory/Provider.hpp"

#define COORDINATES_BY_VERTEX 5   // 5 coords each (x, y, z, u, v)
#define PLAYER_VERTICES 6         // 6 points (two triangles

#define SIZE_VERTICES PLAYER_VERTICES*COORDINATES_BY_VERTEX

namespace Entity
{
    class Paddle
    {
    private:

        float x;
        float y;
        float height;
        float width;

        std::array<GLfloat,SIZE_VERTICES>vertices;

    public:

        Paddle(float x, float y, float width, float height) : x(x), y(y), height(height), width(width)
        {
            vertices = {
                    // Triangles                                            c ___
                    x + (width/2), y + (height/2), 0.0f, 1.0f, 0.0f, // a    |  /a
                    x - (width/2), y - (height/2), 0.0f, 0.0f, 1.0f, // b    | /
                    x - (width/2), y + (height/2), 0.0f, 0.0f, 0.0f, // c   b|/

                    x + (width/2), y + (height/2), 0.0f, 1.0f, 0.0f, // d       /|d
                    x - (width/2), y - (height/2), 0.0f, 0.0f, 1.0f, // e      / |
                    x + (width/2), y - (height/2), 0.0f, 1.0f, 1.0f, // f    e/__|f
            };
        }

        void * operator new (std::size_t size)
        {
            return Memory::Provider::getMemory(Memory::PoolType::POOL_TYPE_GENERIC, size);
        }

        void  operator delete (void* ptr, std::size_t)
        {
            std::cerr << "call delete for non-delete heap memory!" << std::endl;
        }

        std::array<GLfloat, SIZE_VERTICES> getArrayVertices();

        GLfloat* getVertices();

        unsigned int getTotalVertices();

        void move(float x, float y);
    };
}

#endif //ONYX_PLAYER_H
