//
// Created by luiz0tavio on 4/29/18.
//

#include "Ball.hpp"

void Entity::Ball::moveBall()
{
    int i;
    for (i = 0; i < BALL_SIZE_VERTICES; i += COORDINATES_BY_VERTEX) {
        vertices_[i] += speed_ * direction_[0]; // x
        vertices_[i + 1] += speed_ * direction_[1]; // y
    }
}

bool Entity::Ball::checkWallCollision()
{
    auto ballXLeft   = this->vertices_[COORDINATES_BY_VERTEX];
    auto ballXRight  = this->vertices_[0];
    auto ballYTop    = this->vertices_[1];
    auto ballYBottom = this->vertices_[1];

    // Check wall collision
    if (ballXLeft >= 0.80f)    direction_[0] = -1;
    if (ballXRight <= -0.80f)  direction_[0] =  1;
    if (ballYTop >= 1.f)       direction_[1] = -1;
    if (ballYBottom <= -0.80f) return false;

    return true;
}

bool Entity::Ball::checkObjectCollision(std::array<GLfloat, BALL_SIZE_VERTICES> object, float direction_effect = 1.0f)
{
    auto objectXRight  = object[0];
    auto objectYTop    = object[1];
    auto objectXLeft   = object[COORDINATES_BY_VERTEX];
    auto objectYBottom = object[COORDINATES_BY_VERTEX+1];

    auto ballXRight  = this->vertices_[0];
    auto ballYTop    = this->vertices_[1];
    auto ballXLeft   = this->vertices_[COORDINATES_BY_VERTEX];
    auto ballYBottom = this->vertices_[COORDINATES_BY_VERTEX+1];

    if (ballXLeft < objectXRight && ballXRight > objectXLeft) {

        if (ballYTop-0.02f > objectYBottom && ballYTop+0.02f < objectYTop) {
            if(direction_[1] == -1) {
                direction_[0] = direction_[0] == 1 ? -1 : 1;
            } else {
                direction_[1] = -1;
            }
            direction_[0] *= direction_effect;
            return true;
        }

        if (ballYBottom+0.025f < objectYTop && ballYBottom-0.025f > objectYBottom) {
            if(direction_[1] == 1) {
                direction_[0] = direction_[0] == 1 ? -1 : 1;
            } else {
                direction_[1] = 1;
            }
            direction_[0] *= direction_effect;
            return true;
        }
    }

    return false;

}

unsigned int Entity::Ball::getTotalVertices()
{
    return BALL_SIZE_VERTICES;
}

GLfloat* Entity::Ball::getVertices()
{
    return this->vertices_.data();
}