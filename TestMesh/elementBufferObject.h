#pragma once

#include <GL\glew.h>

class elementBufferObject
{
private:
    GLuint _id;
    GLenum _target;
    GLsizei _size;
    void* _data;

    GLuint _componentCount;
    GLenum _componentType;

public:
    elementBufferObject(GLenum target, GLsizei size, void* data, GLuint componentCount, GLenum componentType);
    ~elementBufferObject();
    void bindTo(GLuint vaoId);
    GLuint getComponentCount();
    GLenum getComponentType();
};
