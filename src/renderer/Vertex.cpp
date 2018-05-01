//
// Created by luiz0tavio on 5/1/18.
//

#include <iostream>
#include "Vertex.hpp"

void Renderer::Vertex::setBufferData (unsigned long size, GLfloat* data)
{
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

void Renderer::Vertex::defineLayout(GLuint sp, std::string att_name, const unsigned int size, const GLvoid* offset)
{
    GLint posAttrib = glGetAttribLocation(sp, att_name.c_str());
    if(posAttrib == -1){
        std::cerr << "Error find location Attribute shader!" << std::endl;
    }
    auto uPosAttrib = static_cast<GLuint>(posAttrib);
    glEnableVertexAttribArray(uPosAttrib);
    glVertexAttribPointer(uPosAttrib, size, GL_FLOAT, GL_TRUE, VERTEX_LINE_SIZE*sizeof(GLfloat), offset);
}
