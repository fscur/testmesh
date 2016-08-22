#include "elementBufferObject.h"

elementBufferObject::elementBufferObject(GLenum target, GLsizei size, void* data, GLuint componentCount, GLenum componentType) :
    _target(target),
    _size(size),
    _data(data),
    _componentCount(componentCount),
    _componentType(componentType)
{
    glGenBuffers(1, &_id);
    glBindBuffer(target, _id);
    glBufferData(target, size, data, GL_STATIC_DRAW);
}

elementBufferObject::~elementBufferObject()
{
}

void elementBufferObject::bindTo(GLuint vaoId)
{
    glBindVertexArray(vaoId);
    glBindBuffer(_target, _id);
    glBindVertexArray(0);
}

GLuint elementBufferObject::getComponentCount()
{
    return _componentCount;
}

GLenum elementBufferObject::getComponentType()
{
    return _componentType;
}
