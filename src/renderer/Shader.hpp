#ifndef ONYX_SHADER_HPP
#define ONYX_SHADER_HPP

#include <GLES3/gl3.h>
#include <fstream>
#include <cassert>
#include <vector>
#include "../memory/Pool.hpp"
#include "../memory/Provider.hpp"
#include "../memory/Allocator.hpp"

namespace Renderer {

    class Shader
    {

    private:
        GLuint shaderProgram;
        std::vector<GLuint, Memory::Allocator<GLuint>> shaders;

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

        void  operator delete (void* ptr, std::size_t) {}

        void createGraphicShader(GLenum, std::string);

        GLuint getShaderProgram();

        void beginProgram();

    private:

        void compileShader(GLuint);

        std::string loadShaderFileContent(std::string);

    };

}

#endif //ONYX_SHADER_HPP
