//
// Created by luiz0tavio on 5/1/18.
//
#include "Player.hpp"

void Renderer::Player::move(float x, float y)
{
    auto playerXWallRight = this->vertices[0]+x;
    auto playerXWallLeft  = this->vertices[COORDINATES_BY_VERTEX]+x;

    if (playerXWallRight <= 1.0f  && playerXWallLeft >= -1.f) {
        for (int i = 0; i < SIZE_VERTICES; i += COORDINATES_BY_VERTEX)
        {
            this->vertices[i]+=x;
        }
    }
}

std::array<GLfloat, SIZE_VERTICES> Renderer::Player::getArrayVertices()
{
    return this->vertices;
};

GLfloat* Renderer::Player::getVertices()
{
    return this->vertices.data();
}

unsigned int Renderer::Player::getTotalVertices()
{
    return SIZE_VERTICES;
}
