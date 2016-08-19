#include "vertexArrayObject.h"

vertexArrayObject::vertexArrayObject(GLuint vaoId, GLsizei componentCount, GLsizei componentType) :
    _vaoId(vaoId),
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
    glDrawElements(GL_TRIANGLES, _componentCount, _componentType, 0);
    glBindVertexArray(0);
}
