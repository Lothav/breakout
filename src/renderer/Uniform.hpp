#ifndef ONYX_UNIFORM_HPP
#define ONYX_UNIFORM_HPP

#include <GLES3/gl3.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <glm/mat4x4.hpp> // glm::vec4
#include <glm/gtx/transform.hpp>
#include <map>
#include "../memory/Provider.hpp"

enum UniformType {
    UNIFORM_TYPE_TEXTURE,
    UNIFORM_TYPE_MAT4
};

struct UniformData
{
    const GLchar* location;
    GLuint id;
};

namespace Renderer
{
    class Uniform
    {
    private:

        glm::mat4 view_camera;
        std::map<UniformType, UniformData> data;

    public:

        Uniform() : view_camera(glm::mat4())
        {
            this->data[UNIFORM_TYPE_TEXTURE] = UniformData{.location = "tex", .id = 0};
            this->data[UNIFORM_TYPE_MAT4]    = UniformData{.location = "view", .id = 0};
        }

        ~Uniform()
        {
            glDeleteTextures(1, &this->data[ UNIFORM_TYPE_TEXTURE ].id);
        }

        void * operator new (std::size_t size)
        {
            return Memory::Provider::getMemory(Memory::PoolType::POOL_TYPE_GENERIC, size);
        }

        void  operator delete (void* ptr, std::size_t)
        {
            std::cerr << "call delete for non-delete heap memory!" << std::endl;
        }

        void loadTexture(std::string path);

        void setUniform(GLuint shader_program, UniformType type);

        void translateCamera(GLuint shader_program, const glm::vec3 &vec_translate);
    };

}
#endif //ONYX_UNIFORM_HPP
