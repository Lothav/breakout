//
// Created by luiz0tavio on 5/1/18.
//

#include "Uniform.hpp"

void Renderer::Uniform::loadTexture(std::string path, GLenum format)
{
    SDL_Surface* surf = IMG_Load(path.c_str());

    glGenTextures(1, &this->data[ UNIFORM_TYPE_TEXTURE ].id);
    glBindTexture(GL_TEXTURE_2D, this->data[ UNIFORM_TYPE_TEXTURE ].id);
    glTexStorage2D(GL_TEXTURE_2D, 1, GL_RGB8, surf->w, surf->h);
    glTexSubImage2D(GL_TEXTURE_2D, 0, 0 ,0, surf->w, surf->h, format, GL_UNSIGNED_BYTE, surf->pixels);

    free(surf);

    unsigned int error_code = glGetError();
    if (error_code != GL_NO_ERROR) {
        std::cerr << "OpenGL error glTexImage2D. Error code: " << std::to_string(error_code) << std::endl;
    }

    glGenerateMipmap(GL_TEXTURE_2D);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);

}

void Renderer::Uniform::setUniform(GLuint shader_program, UniformType type)
{
    GLint uniformAtt = glGetUniformLocation(shader_program, this->data[type].location);
    if (uniformAtt == -1) {
        std::cerr << "Can't find uniform key on shader loaded! Key: " << this->data[type].location << std::endl;
        return;
    }

    if (type == UNIFORM_TYPE_TEXTURE) {
        glActiveTexture(GL_TEXTURE0 + this->texture_count);
        glBindTexture(GL_TEXTURE_2D, this->data[ UNIFORM_TYPE_TEXTURE ].id);
        glUniform1i(uniformAtt, this->texture_count);
        return;
    }

    if (type == UNIFORM_TYPE_MAT4) {
        glUniformMatrix4fv(uniformAtt, 1, GL_FALSE, &this->view_camera[0][0]);
    }
}

void Renderer::Uniform::translateCamera(GLuint shader_program, const glm::vec3 &vec_translate)
{
    this->view_camera = glm::translate(this->view_camera, vec_translate);
    this->setUniform(shader_program, UNIFORM_TYPE_MAT4);
}
