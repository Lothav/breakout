#ifndef ONYX_SHADER_HPP
#define ONYX_SHADER_HPP

#include <GLES3/gl3.h>
#include <fstream>
#include <cassert>
#include <vector>
#include "../memory/Pool.hpp"
#include "../memory/Provider.hpp"

namespace Renderer {

    class Shader
    {

    private:
        GLuint shaderProgram;
        std::vector<GLuint> shaders;

    public:

        Shader() : shaderProgram(glCreateProgram()) {}

        ~Shader()
        {
            glDeleteProgram(this->shaderProgram);
            for (auto shader : shaders) {
                glDeleteShader(shader);
            }
        }

        void* operator new (std::size_t size)
        {
            return Memory::Provider::getMemory(Memory::PoolType::POOL_TYPE_GENERIC, size);
        }

        void  operator delete (void* ptr, std::size_t)
        {
            std::cerr << "call delete for non-delete heap memory!" << std::endl;
        }

        void createGraphicShader(GLenum, std::string);
        GLuint getShaderProgram();
        void beginProgram();

    private:

        void compileShader(GLuint);
        std::string loadShaderFileContent(std::string);

    };

}

#endif //ONYX_SHADER_HPP
