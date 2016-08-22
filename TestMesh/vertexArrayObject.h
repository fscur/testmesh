#pragma once

#include <GL\glew.h>

class vertexArrayObject
{
private:
    GLuint _vaoId;
    GLenum _mode;
    GLsizei _componentCount;
    GLenum _componentType;

public:
    vertexArrayObject(GLuint vaoId, GLenum mode, GLsizei componentCount, GLenum componentType);
    ~vertexArrayObject();

    void render();
};
