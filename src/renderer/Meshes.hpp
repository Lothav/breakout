//
// Created by luiz0tavio on 4/29/18.
//

#ifndef ONYX_DRAW_H
#define ONYX_DRAW_H

#include <vector>
#include <GL/gl.h>
#include <iostream>
#include "../memory/Provider.hpp"
#include "../memory/Allocator.hpp"

namespace Renderer
{
    class Meshes
    {
    private:

        std::vector<GLfloat, Memory::Allocator<GLfloat>> meshes;

    public:

        Meshes(): meshes({}) {}

        void * operator new (std::size_t size)
        {
            return Memory::Provider::getMemory(Memory::PoolType::POOL_TYPE_GENERIC, size);
        }

        void operator delete (void* ptr, std::size_t size) {}

        void insert(GLfloat* mesh, unsigned int meshes_size);

        void clear();

        GLfloat * get();

        unsigned long getByteSize();

        unsigned long getSize();
    };

}

#endif //ONYX_DRAW_H