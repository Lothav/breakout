//
// Created by luiz0tavio on 4/29/18.
//

#ifndef ONYX_DRAW_H
#define ONYX_DRAW_H

#include <vector>
#include <GLES3/gl3.h>

namespace Renderer
{
    class Meshes
    {
    private:

        std::vector<GLfloat> meshes;

    public:

        Meshes(): meshes({}) {}


        void * operator new (std::size_t size)
        {
            return Memory::Provider::getMemory(Memory::PoolType::POOL_TYPE_GENERIC, size);
        }

        void  operator delete (void* ptr, std::size_t)
        {
            std::cerr << "call delete for non-delete heap memory!" << std::endl;
        }

        void insert(GLfloat* mesh, unsigned int meshes_size)
        {
            for(unsigned int i = 0; i < meshes_size; i++) this->meshes.push_back(mesh[i]);
        }

        void clear()
        {
            this->meshes.clear();
        }

        GLfloat * get()
        {
            return this->meshes.data();
        }

        unsigned long getByteSize()
        {
            return this->meshes.size() * sizeof(GLfloat);
        }

        unsigned long getSize()
        {
            return this->meshes.size();
        }
    };

}

#endif //ONYX_DRAW_H