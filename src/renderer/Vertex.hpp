#ifndef ONYX_VERTEX_HPP
#define ONYX_VERTEX_HPP

#include <GLES3/gl3.h>

#define VERTEX_LINE_SIZE 5 // (x, y, z, u, v)

namespace Renderer {

    class Vertex
    {

    private:
        GLuint VBO;
        GLuint VAO;

        GLfloat* vertices;
        unsigned int vertices_size;

    public:

        Vertex(GLuint shader_program) : VBO(0), VAO(0), vertices(nullptr), vertices_size(0)
        {
            glGenVertexArrays(1, &this->VAO);
            glBindVertexArray(this->VAO);

            // make and bind the VBO
            glGenBuffers(1, &this->VBO);
            glBindBuffer(GL_ARRAY_BUFFER, this->VBO);

            this->defineLayout(shader_program, "vert", 3);
            this->defineLayout(shader_program, "vertTexCoord", 2, (const GLvoid*)(3 * sizeof(GLfloat)));
        }

        ~Vertex()
        {
            glDeleteBuffers(1, &this->VBO);
            glDeleteBuffers(1, &this->VAO);
        }


        void * operator new (std::size_t size)
        {
            return Memory::Provider::getMemory(Memory::PoolType::POOL_TYPE_GENERIC, size);
        }

        void  operator delete (void* ptr, std::size_t)
        {
            std::cerr << "call delete for non-delete heap memory!" << std::endl;
        }

        void setBufferData (unsigned long size, GLfloat* data)
        {
            glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
        }

    private:

        void defineLayout(GLuint sp, std::string att_name, const unsigned int size, const GLvoid* offset=nullptr)
        {
            GLint posAttrib = glGetAttribLocation(sp, att_name.c_str());
            if(posAttrib == -1){
                std::cerr << "Error find location Attribute shader!" << std::endl;
            }
            auto uPosAttrib = static_cast<GLuint>(posAttrib);
            glEnableVertexAttribArray(uPosAttrib);
            glVertexAttribPointer(uPosAttrib, size, GL_FLOAT, GL_TRUE, VERTEX_LINE_SIZE*sizeof(GLfloat), offset);
        }

    };
}
#endif //ONYX_VERTEX_HPP
