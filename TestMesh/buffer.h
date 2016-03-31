#pragma once
#include <gl\glew.h>

namespace bufferTarget
{
    enum bufferTarget
    {
        none = GL_NONE,
        array = GL_ARRAY_BUFFER,
        element = GL_ELEMENT_ARRAY_BUFFER,
        uniform = GL_UNIFORM_BUFFER,
        shader = GL_SHADER_STORAGE_BUFFER,
        drawIndirect = GL_DRAW_INDIRECT_BUFFER
    };
}

namespace bufferDataUsage
{
    enum bufferDataUsage
    {
        streamDraw = GL_STREAM_DRAW,
        streamRead = GL_STREAM_READ,
        streamCopy = GL_STREAM_COPY,
        staticDraw = GL_STATIC_DRAW,
        staticRead = GL_STATIC_READ,
        staticCopy = GL_STATIC_COPY,
        dynamicDraw = GL_DYNAMIC_DRAW,
        dynamicRead = GL_DYNAMIC_READ,
        dynamicCopy = GL_DYNAMIC_COPY
    };
}

namespace bufferStorageUsage
{
    enum bufferStorageUsage
    {
        none = GL_NONE,
        dynamic = GL_DYNAMIC_STORAGE_BIT,
        read = GL_MAP_READ_BIT,
        write = GL_MAP_WRITE_BIT,
        persistent = GL_MAP_PERSISTENT_BIT,
        coherent = GL_MAP_COHERENT_BIT,
        client = GL_CLIENT_STORAGE_BIT
    };
}

inline bufferStorageUsage::bufferStorageUsage operator|(bufferStorageUsage::bufferStorageUsage a, bufferStorageUsage::bufferStorageUsage b)
{
    return static_cast<bufferStorageUsage::bufferStorageUsage>(static_cast<int>(a) | static_cast<int>(b));
}

class buffer
{
protected:
    GLuint id;
    bufferTarget::bufferTarget target;

public:
    buffer(bufferTarget::bufferTarget  target);
    virtual ~buffer();

    void bind();
    void unbind();
    void bindBufferBase(GLuint location);
    void storage(GLsizeiptr size, void* data, bufferStorageUsage::bufferStorageUsage usage);
    void data(GLsizeiptr size, void* data, bufferDataUsage::bufferDataUsage usage);
    void subData(GLintptr offset, GLintptr size, void* data);
};