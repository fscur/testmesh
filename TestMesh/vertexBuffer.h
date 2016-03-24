#pragma once
#include "buffer.h"
#include "vertex.h"
#include <vector>

struct vertexAttrib
{
public:
    GLuint location;
    GLuint size;
    GLenum type;
    GLsizei stride;
    const void* offset;
    GLuint divisor;
public:

    vertexAttrib(
        GLuint location = -1,
        GLuint size = 0,
        GLenum type = GL_NONE,
        GLsizei stride = 0,
        const void* offset = (const void*)0,
        GLuint divisor = 0) :
        location(location),
        size(size),
        type(type),
        stride(stride),
        offset(offset),
        divisor(divisor)
    {
    }
};

class vertexBuffer :
    public buffer
{
public:
    vertexBuffer(std::vector<vertexAttrib> attribs);
};