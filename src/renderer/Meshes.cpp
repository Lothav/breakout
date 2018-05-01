//
// Created by luiz0tavio on 5/1/18.
//

#include "Meshes.hpp"

void Renderer::Meshes::insert(GLfloat* mesh, unsigned int meshes_size)
{
    for(unsigned int i = 0; i < meshes_size; i++) this->meshes.push_back(mesh[i]);
}

void Renderer::Meshes::clear()
{
    this->meshes.clear();
}

GLfloat * Renderer::Meshes::get()
{
    return this->meshes.data();
}

unsigned long Renderer::Meshes::getByteSize()
{
    return this->meshes.size() * sizeof(GLfloat);
}

unsigned long Renderer::Meshes::getSize()
{
    return this->meshes.size();
}