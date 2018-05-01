//
// Created by luiz0tavio on 4/29/18.
//

#ifndef BREAKOUT_BLOCK_HPP
#define BREAKOUT_BLOCK_HPP

#include <array>
#include <GLES3/gl3.h>
#include <ctime>
#include "../memory/Pool.hpp"
#include "../memory/Provider.hpp"
#include "../memory/Allocator.hpp"

#define BLOCK_COORDINATES_BY_VERTEX 5   // 5 coords each (x, y, z, u, v)
#define BLOCK_VERTICES 6                // 6 points (two triangles

#define BLOCK_SIZE_VERTICES BLOCK_VERTICES*BLOCK_COORDINATES_BY_VERTEX

#define BLOCK_TEXTURE_MAX_X 5
#define BLOCK_TEXTURE_MAX_Y 5

#define TEXTURE_UV_OFFSET_X 0.2f
#define TEXTURE_UV_OFFSET_Y 0.2f

#define BLOCK_WIDTH  0.16f
#define BLOCK_HEIGHT 0.1f

namespace Entity
{
    class Block
    {
    private:

        float x_;
        float y_;

        bool shown_;

        std::array<int, 2> texture_index_;
        std::array<GLfloat, BLOCK_SIZE_VERTICES> vertices_;

    public:

        Block(float x, float y) : x_(x), y_(y), shown_(true)
        {
            texture_index_[0] = std::rand() % BLOCK_TEXTURE_MAX_X;
            std::map<
                char,
                std::array<GLfloat, 2>,
                std::less<char>,
                Memory::Allocator<std::pair<const char, std::array<GLfloat, 2>> >
            > uv;

            texture_index_[0] = std::rand() % BLOCK_TEXTURE_MAX_X;
            texture_index_[1] = std::rand() % BLOCK_TEXTURE_MAX_Y;

            uv['a'] = {TEXTURE_UV_OFFSET_X + texture_index_[0]*TEXTURE_UV_OFFSET_X, texture_index_[1]*TEXTURE_UV_OFFSET_Y};
            uv['b'] = {texture_index_[0]*TEXTURE_UV_OFFSET_X, TEXTURE_UV_OFFSET_Y + texture_index_[1]*TEXTURE_UV_OFFSET_Y};
            uv['c'] = {texture_index_[0]*TEXTURE_UV_OFFSET_X, texture_index_[1]*TEXTURE_UV_OFFSET_Y};

            uv['d'] = {TEXTURE_UV_OFFSET_X + texture_index_[0]*TEXTURE_UV_OFFSET_X, texture_index_[1]*TEXTURE_UV_OFFSET_Y};
            uv['e'] = {texture_index_[0]*TEXTURE_UV_OFFSET_X, TEXTURE_UV_OFFSET_Y + texture_index_[1]*TEXTURE_UV_OFFSET_Y};
            uv['f'] = {TEXTURE_UV_OFFSET_X + texture_index_[0]*TEXTURE_UV_OFFSET_X, TEXTURE_UV_OFFSET_Y + texture_index_[1]*TEXTURE_UV_OFFSET_Y};

            vertices_ = {
                // Triangles                                                                      c ___
                x_ + (BLOCK_WIDTH/2), y_ + (BLOCK_HEIGHT/2), 0.0f, uv['a'][0], uv['a'][1], // a    |  /a
                x_ - (BLOCK_WIDTH/2), y_ - (BLOCK_HEIGHT/2), 0.0f, uv['b'][0], uv['b'][1], // b    | /
                x_ - (BLOCK_WIDTH/2), y_ + (BLOCK_HEIGHT/2), 0.0f, uv['c'][0], uv['c'][1], // c   b|/

                x_ + (BLOCK_WIDTH/2), y_ + (BLOCK_HEIGHT/2), 0.0f, uv['d'][0], uv['d'][1], // d      /|d
                x_ - (BLOCK_WIDTH/2), y_ - (BLOCK_HEIGHT/2), 0.0f, uv['e'][0], uv['e'][1], // e     / |
                x_ + (BLOCK_WIDTH/2), y_ - (BLOCK_HEIGHT/2), 0.0f, uv['f'][0], uv['f'][1], // f   e/__|f
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

        std::array<GLfloat, BLOCK_SIZE_VERTICES> getArrayVertices()
        {
            return vertices_;
        }

        void changeVisibility()
        {
            shown_ = false;
        }

        bool isAlive()
        {
            return shown_;
        }

        GLfloat* getVertices();
        unsigned int getTotalVertices();
    };
}
#endif //BREAKOUT_BLOCK_HPP
