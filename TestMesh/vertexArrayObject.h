#pragma once

#include <GL\glew.h>

class vertexArrayObject
{
private:
    GLuint _vaoId;
    GLsizei _componentCount;
    GLsizei _componentType;

public:
    vertexArrayObject(GLuint vaoId, GLsizei componentCount, GLsizei componentType);
    ~vertexArrayObject();

    void render();
};

