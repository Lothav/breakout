//
// Created by luiz0tavio on 4/29/18.
//

#ifndef BREAKOUT_BALL_HPP
#define BREAKOUT_BALL_HPP

#include <GLES3/gl3.h>
#include <array>

#define COORDINATES_BY_VERTEX 5   // 5 coords each (x, y, z, u, v)
#define BALL_VERTICES 6         // 6 points (two triangles

#define BALL_SIZE_VERTICES BALL_VERTICES*COORDINATES_BY_VERTEX

#define BALL_WIDTH  0.06f
#define BALL_HEIGHT 0.06f

namespace Renderer
{
    class Ball
    {
    private:

        float x_;
        float y_;
        float speed_;

        std::array<GLfloat, BALL_SIZE_VERTICES> vertices_;

    public:
        Ball(float x, float y, float speed): x_(x), y_(y), speed_(speed)
        {
            vertices_ = {
                    // Triangles                                                      c ___
                    x + (BALL_WIDTH/2), y + (BALL_HEIGHT/2), 0.0f, 1.0f, 0.0f, // a    |  /a
                    x - (BALL_WIDTH/2), y - (BALL_HEIGHT/2), 0.0f, 0.0f, 1.0f, // b    | /
                    x - (BALL_WIDTH/2), y + (BALL_HEIGHT/2), 0.0f, 0.0f, 0.0f, // c   b|/

                    x + (BALL_WIDTH/2), y + (BALL_HEIGHT/2), 0.0f, 1.0f, 0.0f, // d       /|d
                    x - (BALL_WIDTH/2), y - (BALL_HEIGHT/2), 0.0f, 0.0f, 1.0f, // e      / |
                    x + (BALL_WIDTH/2), y - (BALL_HEIGHT/2), 0.0f, 1.0f, 1.0f, // f    e/__|f
            };
        }

        void moveBall() {

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
