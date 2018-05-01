//
// Created by luiz0tavio on 4/29/18.
//

#ifndef BREAKOUT_BALL_HPP
#define BREAKOUT_BALL_HPP

#include <GLES3/gl3.h>
#include <array>
#include <iostream>

#define COORDINATES_BY_VERTEX 5   // 5 coords each (x, y, z, u, v)
#define BALL_VERTICES 6         // 6 points (two triangles

#define BALL_SIZE_VERTICES BALL_VERTICES*COORDINATES_BY_VERTEX

#define BALL_WIDTH  0.06f
#define BALL_HEIGHT 0.06f

namespace Renderer
{
    class Ball {
    private:

        float x_;
        float y_;
        float speed_;
        std::array<float, 2> direction_;

        std::array<GLfloat, BALL_SIZE_VERTICES> vertices_;

    public:
        Ball(float x, float y, float speed) : x_(x), y_(y), speed_(speed) {
            speed_ = 0.0005f;
            direction_ = {1.0f, .1f};
            vertices_ = {
                    // Triangles                                                      c ___
                    x + (BALL_WIDTH / 2), y + (BALL_HEIGHT / 2), 0.0f, 1.0f, 0.0f, // a    |  /a
                    x - (BALL_WIDTH / 2), y - (BALL_HEIGHT / 2), 0.0f, 0.0f, 1.0f, // b    | /
                    x - (BALL_WIDTH / 2), y + (BALL_HEIGHT / 2), 0.0f, 0.0f, 0.0f, // c   b|/

                    x + (BALL_WIDTH / 2), y + (BALL_HEIGHT / 2), 0.0f, 1.0f, 0.0f, // d       /|d
                    x - (BALL_WIDTH / 2), y - (BALL_HEIGHT / 2), 0.0f, 0.0f, 1.0f, // e      / |
                    x + (BALL_WIDTH / 2), y - (BALL_HEIGHT / 2), 0.0f, 1.0f, 1.0f, // f    e/__|f
            };
        }

        void moveBall() {
            int i;
            for (i = 0; i < BALL_SIZE_VERTICES; i += COORDINATES_BY_VERTEX) {
                vertices_[i] += speed_ * direction_[0]; // x
                vertices_[i + 1] += speed_ * direction_[1]; // y
            }
        }

        void checkWallCollision()
        {
            auto ballXLeft   = this->vertices_[COORDINATES_BY_VERTEX];
            auto ballXRight  = this->vertices_[0];
            auto ballYTop    = this->vertices_[1];
            auto ballYBottom = this->vertices_[1];

            // Check wall collision
            if (ballXLeft >= 0.95f)    direction_[0] = -1;
            if (ballXRight <= -0.95f)  direction_[0] =  1;
            if (ballYTop >= 1.f)       direction_[1] = -1;
            if (ballYBottom <= -0.95f) direction_[1] =  1;
        }

        void checkObjectCollision(std::array<GLfloat, BALL_SIZE_VERTICES> object)
        {
            auto objectXRight  = object[0];
            auto objectYTop    = object[1];
            auto objectXLeft   = object[COORDINATES_BY_VERTEX];
            auto objectYBottom = object[COORDINATES_BY_VERTEX+1];

            auto ballXRight  = this->vertices_[0];
            auto ballYTop    = this->vertices_[1];
            auto ballXLeft   = this->vertices_[COORDINATES_BY_VERTEX];
            auto ballYBottom = this->vertices_[COORDINATES_BY_VERTEX+1];

            if (ballXLeft > objectXRight || ballXRight < objectXLeft) {
                return;
            }

            if (ballYTop > objectYBottom && ballYTop < objectYTop) {
                direction_[1] = -1;
                return;
            }

            if (ballYBottom < objectYTop && ballYBottom > objectYBottom) {
                direction_[1] = 1;
                return;
            }

        }

        unsigned int getTotalVertices()
        {
            return BALL_SIZE_VERTICES;
        }

        GLfloat* getVertices()
        {
            return this->vertices_.data();
        }

    };
}
#endif //BREAKOUT_BALL_HPP
