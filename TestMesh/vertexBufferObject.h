#pragma once

#include <GL\glew.h>

class vertexBufferObject
{
private:
    GLuint _id;

    GLenum _target;
    GLsizei _size;
    void* _data;

    GLenum _type;
    GLsizei _stride;

public:
    vertexBufferObject(GLenum target, GLsizei size, void* data, GLenum type, GLsizei stride);
    ~vertexBufferObject();

    void bindTo(GLuint vaoId, GLuint attribLocation);
};
