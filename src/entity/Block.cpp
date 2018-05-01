//
// Created by luiz0tavio on 4/29/18.
//

#include "Block.hpp"


GLfloat* Entity::Block::getVertices()
{
    return this->vertices_.data();
}

unsigned int Entity::Block::getTotalVertices()
{
    return BLOCK_SIZE_VERTICES;
}