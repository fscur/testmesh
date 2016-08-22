#include "vertexBufferObject.h"

vertexBufferObject::vertexBufferObject(GLenum target, GLsizei size, void* data, GLenum type, GLsizei stride) :
    _target(target),
    _size(size),
    _data(data),
    _type(type),
    _stride(stride)
{
    glGenBuffers(1, &_id);
    glBindBuffer(target, _id);
    glBufferData(target, size, data, GL_STATIC_DRAW);
}

vertexBufferObject::~vertexBufferObject()
{
}


void vertexBufferObject::bindTo(GLuint vaoId, GLuint attribLocation)
{
    glBindVertexArray(vaoId);

    glBindBuffer(_target, _id);
    glEnableVertexAttribArray(attribLocation);
    glVertexAttribPointer(attribLocation, 3, _type, GL_FALSE, _stride, 0);

    glBindVertexArray(0);
}
