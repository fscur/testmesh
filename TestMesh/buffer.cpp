#include "buffer.h"

buffer::buffer(bufferTarget::bufferTarget target) :
    target(target),
    id(-1)
{
    glCreateBuffers(1, &id);
    bind();
}

buffer::~buffer()
{
    glDeleteBuffers(1, &id);
}

void buffer::bind()
{
    glBindBuffer(target, id);
}

void buffer::unbind()
{
    glBindBuffer(target, 0);
}

void buffer::bindBufferBase(GLuint location)
{
    glBindBufferBase(target, location, id);
}

void buffer::storage(GLsizeiptr size, void * data, bufferStorageUsage::bufferStorageUsage usage)
{
    glNamedBufferStorage(id, size, data == nullptr ? 0 : data, usage);
}

void buffer::data(GLsizeiptr size, void * data, bufferDataUsage::bufferDataUsage usage)
{
    glNamedBufferData(id, size, data == nullptr ? 0 : data, usage);
}

void buffer::subData(GLintptr offset, GLintptr size, void * data)
{
    glNamedBufferSubData(id, offset, size, data);
}

