//
// Created by luiz0tavio on 4/29/18.
//

#ifndef ONYX_PLAYER_H
#define ONYX_PLAYER_H

#include <GLES3/gl3.h>
#include <algorithm>
#include <array>

#define COORDINATES_BY_VERTEX 5   // 5 coords each (x, y, z, u, v)
#define PLAYER_VERTICES 6         // 6 points (two triangles

#define SIZE_VERTICES PLAYER_VERTICES*COORDINATES_BY_VERTEX

namespace Renderer
{
    class Player
    {
    private:

        float x;
        float y;
        float height;
        float width;

        std::array<GLfloat,SIZE_VERTICES>vertices;

    public:

        Player(float x, float y, float width, float height) : x(x), y(y), height(height), width(width)
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

        GLfloat* getVertices()
        {
            return this->vertices.data();
        }

        unsigned int getTotalVertices()
        {
            return SIZE_VERTICES;
        }

        void move(float x, float y)
        {
            auto playerXWallRight = this->vertices[0]+x;
            auto playerXWallLeft  = this->vertices[COORDINATES_BY_VERTEX]+x;

            if (playerXWallRight <= 1.0f  && playerXWallLeft >= -1.f) {
                for (int i = 0; i < SIZE_VERTICES; i += COORDINATES_BY_VERTEX)
                {
                    this->vertices[i]+=x;
                }
            }
        }
    };
}

#endif //ONYX_PLAYER_H
