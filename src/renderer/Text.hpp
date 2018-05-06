//
// Created by luiz0tavio on 5/6/18.
//

#ifndef BREAKOUT_TEXT_HPP
#define BREAKOUT_TEXT_HPP

#include <ft2build.h>
#include <GL/glew.h>
#include <memory>
#include FT_FREETYPE_H
#include "Shader.hpp"

struct point {
    GLfloat x;
    GLfloat y;
    GLfloat s;
    GLfloat t;
};

namespace Renderer {


    class Text
    {
        float x_;
        float y_;
        float sx_;
        float sy_;
        FT_Face font_face_;

        GLuint vbo;

        GLint uniform_tex_;
        GLint attribute_coord_;
        GLint uniform_color_;

        Renderer::Shader* shader;

    public:

        Text(float x, float y, float sx, float sy, FT_Face font_face) : x_(x), y_(y), sx_(sx), sy_(sy),font_face_(font_face), vbo()
        {
            shader = new Renderer::Shader();
            shader->createGraphicShader(GL_VERTEX_SHADER, "text.vert");
            shader->createGraphicShader(GL_FRAGMENT_SHADER, "text.frag");
            shader->link();

            this->uniform_tex_     = glGetUniformLocation(shader->getShaderProgram(), "tex");
            this->attribute_coord_ = glGetAttribLocation(shader->getShaderProgram(), "coord");
            this->uniform_color_   = glGetUniformLocation(shader->getShaderProgram(), "color");

            glGenBuffers(1, &vbo);
        }

        void prepare()
        {
            shader->use();
            GLfloat black[4] = { 1, 1, 1, 1 };

            /* Set font size to 48 pixels, color to black */
            FT_Set_Pixel_Sizes(font_face_, 0, 48);
            glUniform4fv(uniform_color_, 1, black);
        }

        void draw(const char *text)
        {
            const char *p;
            FT_GlyphSlot g = this->font_face_->glyph;

            /* Create a texture that will be used to hold one "glyph" */
            GLuint tex;

            glActiveTexture(GL_TEXTURE3);
            glGenTextures(1, &tex);
            glBindTexture(GL_TEXTURE_2D, tex);
            glUniform1i(uniform_tex_, 3);

            /* We require 1 byte alignment when uploading texture data */
            glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

            /* Clamping to edges is important to prevent artifacts when scaling */
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

            /* Linear filtering usually looks best for text */
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

            /* Set up the VBO for our vertex data */
            glEnableVertexAttribArray(attribute_coord_);
            glBindBuffer(GL_ARRAY_BUFFER, vbo);
            glVertexAttribPointer(attribute_coord_, 4, GL_FLOAT, GL_FALSE, 0, 0);

            auto x_tmp = x_;
            auto y_tmp = y_;

            /* Loop through all characters */
            for (p = text; *p; p++) {
                /* Try to load and render the character */
                if (FT_Load_Char(this->font_face_, *p, FT_LOAD_RENDER))
                    continue;

                /* Upload the "bitmap", which contains an 8-bit grayscale image, as an alpha texture */
                glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, g->bitmap.width, g->bitmap.rows, 0, GL_ALPHA, GL_UNSIGNED_BYTE, g->bitmap.buffer);

                /* Calculate the vertex and texture coordinates */
                float x2 = x_tmp + g->bitmap_left * sx_;
                float y2 = -y_tmp - g->bitmap_top * sy_;
                float w  = g->bitmap.width * sx_;
                float h  = g->bitmap.rows * sy_;

                point box[4] = {
                        {    x2,     -y2, 0, 0},
                        {x2 + w,     -y2, 1, 0},
                        {    x2, -y2 - h, 0, 1},
                        {x2 + w, -y2 - h, 1, 1},
                };

                /* Draw the character on the screen */
                glBufferData(GL_ARRAY_BUFFER, sizeof box, box, GL_DYNAMIC_DRAW);
                glDrawArrays(GL_TRIANGLE_STRIP, 0, 4);

                /* Advance the cursor to the start of the next character */
                x_tmp += (g->advance.x >> 6) * sx_;
                y_tmp += (g->advance.y >> 6) * sy_;
            }

            glDisableVertexAttribArray(attribute_coord_);
            glDeleteTextures(1, &tex);
        }

    };
}


#endif //BREAKOUT_TEXT_HPP
