//
// Created by luiz0tavio on 5/1/18.
//
#include "Player.hpp"

void Entity::Paddle::move(float x, float y)
{
    auto playerXWallRight = this->vertices[0]+x;
    auto playerXWallLeft  = this->vertices[COORDINATES_BY_VERTEX]+x;

    if (playerXWallRight <= 1.0f  && playerXWallLeft >= -1.f) {
        is_moving = true;
        for (int i = 0; i < SIZE_VERTICES; i += COORDINATES_BY_VERTEX)
        {
            this->vertices[i]+=x;
        }
    } else {
        is_moving = false;
    }
}

std::array<GLfloat, SIZE_VERTICES> Entity::Paddle::getArrayVertices()
{
    return this->vertices;
};

GLfloat* Entity::Paddle::getVertices()
{
    return this->vertices.data();
}

unsigned int Entity::Paddle::getTotalVertices()
{
    return SIZE_VERTICES;
}
