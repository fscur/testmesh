#include "vertexArrayObject.h"

vertexArrayObject::vertexArrayObject(GLuint vaoId, GLenum mode, GLsizei componentCount, GLenum componentType) :
    _vaoId(vaoId),
    _mode(mode),
    _componentCount(componentCount),
    _componentType(componentType)
{
}

vertexArrayObject::~vertexArrayObject()
{
    glDeleteVertexArrays(1, &_vaoId);
}

void vertexArrayObject::render()
{
    glBindVertexArray(_vaoId);
    glDrawElements(_mode, _componentCount, _componentType, 0);
    glBindVertexArray(0);
}
