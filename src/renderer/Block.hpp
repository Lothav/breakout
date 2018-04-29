//
// Created by luiz0tavio on 4/29/18.
//

#ifndef BREAKOUT_BLOCK_HPP
#define BREAKOUT_BLOCK_HPP

#include <array>
#include <GLES3/gl3.h>
#include "../memory/Pool.hpp"
#include "../memory/Provider.hpp"
#include "../memory/Allocator.hpp"

#define COORDINATES_BY_VERTEX 5   // 5 coords each (x, y, z, u, v)
#define PLAYER_VERTICES 6         // 6 points (two triangles

#define SIZE_VERTICES PLAYER_VERTICES*COORDINATES_BY_VERTEX

namespace Renderer
{
    class Block
    {
    private:

        float x;
        float y;
        float height;
        float width;

        std::array<int, 2> texture_index_;
        std::array<GLfloat,SIZE_VERTICES> vertices_;

    public:

        Block(float x, float y, float height, float width, std::array<int, 2> texture_index)
                : x(x), y(y), height(height), width(width), texture_index_(texture_index)
        {
            std::map<
                char,
                std::array<GLfloat, 2>,
                std::less<char>,
                Memory::Allocator<std::pair<const char, std::array<GLfloat, 2>> >
            > uv;

            uv['a'] = {0.066f + texture_index_[0]*0.066f, texture_index_[0]*0.05f};
            uv['b'] = {texture_index_[0]*0.066f, 0.05f + texture_index_[0]*0.05f};
            uv['c'] = {texture_index_[0]*0.066f, texture_index_[0]*0.05f};

            uv['d'] = {0.066f + texture_index_[0]*0.066f, texture_index_[0]*0.05f};
            uv['e'] = {texture_index_[0]*0.066f, 0.05f + texture_index_[0]*0.05f};
            uv['f'] = {0.066f + texture_index_[0]*0.066f, 0.05f + texture_index_[0]*0.05f};

            vertices_ = {
                    // Triangles                                                         c ___
                    x + (width/2), y + (height/2), 0.0f, uv['a'][0], uv['a'][1], // a     |  /a
                    x - (width/2), y - (height/2), 0.0f, uv['b'][0], uv['b'][1], // b     | /
                    x - (width/2), y + (height/2), 0.0f, uv['c'][0], uv['c'][1], // c    b|/

                    x + (width/2), y + (height/2), 0.0f, uv['d'][0], uv['d'][1], // d       /|d
                    x - (width/2), y - (height/2), 0.0f, uv['e'][0], uv['e'][1], // e      / |
                    x + (width/2), y - (height/2), 0.0f, uv['f'][0], uv['f'][1], // f    e/__|f
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

        GLfloat* getVertices();
        unsigned int getTotalVertices();
    };
}
#endif //BREAKOUT_BLOCK_HPP
