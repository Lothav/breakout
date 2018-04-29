//
// Created by luiz0tavio on 4/29/18.
//

#include "Block.hpp"


GLfloat* Renderer::Block::getVertices()
{
    return this->vertices_.data();
}

unsigned int Renderer::Block::getTotalVertices()
{
    return SIZE_VERTICES;
}