#ifndef ONYX_UNIFORM_HPP
#define ONYX_UNIFORM_HPP

#include <GLES3/gl3.h>
#include <SDL2/SDL_surface.h>
#include <SDL2/SDL_image.h>
#include <string>
#include <glm/mat4x4.hpp> // glm::vec4
#include <glm/gtx/transform.hpp>
#include <map>

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

        void loadTexture(std::string path)
        {
            SDL_Surface* surf = IMG_Load(path.c_str());

            glGenTextures(1, &this->data[ UNIFORM_TYPE_TEXTURE ].id);
            glBindTexture(GL_TEXTURE_2D, this->data[ UNIFORM_TYPE_TEXTURE ].id);
            glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGBA8, surf->w, surf->h);

            glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, surf->w, surf->h, GL_RGBA, GL_UNSIGNED_BYTE, surf->pixels);

            free(surf);

            unsigned int error_code = glGetError();
            if (error_code != GL_NO_ERROR) {
                std::cerr << "OpenGL error glTexImage2D. Error code: " << std::to_string(error_code) << std::endl;
            }

            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

            glGenerateMipmap(GL_TEXTURE_2D);
        }

        void setUniform(GLuint shader_program, UniformType type)
        {
            GLint uniformAtt = glGetUniformLocation(shader_program, this->data[type].location);
            if (uniformAtt == -1) {
                std::cerr << "Can't find uniform key on shader loaded! Key: " << this->data[type].location << std::endl;
                return;
            }

            if (type == UNIFORM_TYPE_TEXTURE) {
                glActiveTexture(GL_TEXTURE0 + 0);
                glBindTexture(GL_TEXTURE_2D, this->data[ UNIFORM_TYPE_TEXTURE ].id);
                glUniform1i(uniformAtt, 0);
                return;
            }

            if (type == UNIFORM_TYPE_MAT4) {
                glUniformMatrix4fv(uniformAtt, 1, GL_FALSE, &this->view_camera[0][0]);
            }
        }

        void translateCamera(GLuint shader_program, const glm::vec3 &vec_translate)
        {
            this->view_camera = glm::translate(this->view_camera, vec_translate);
            this->setUniform(shader_program, UNIFORM_TYPE_MAT4);
        }
    };

}
#endif //ONYX_UNIFORM_HPP
