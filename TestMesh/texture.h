#ifndef _TEXTURE_H_
#define _TEXTURE_H_

#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif

#include <string>
#include <vector>

class texture
{
public:
    GLuint _id;
    GLuint _textureType;
    GLuint64 _handle;
    unsigned char* _data;
    GLenum _dataType;
    GLenum _dataFormat;

public:
    GLuint _w;
    GLuint _h;

private:
    texture(GLuint id, GLuint w, GLuint h);

public:
    ~texture();

    inline GLuint getId() const { return _id; }
    inline GLuint getWidth() const { return _w; }
    inline GLuint getHeight() const { return _h; }
    inline GLuint getTextureType() const { return _textureType; }
    inline GLuint64 getHandle() const { return _handle; }
    inline unsigned char* getData() const { return _data; }
    inline GLenum getDataFormat() const { return _dataFormat; }
    inline GLenum getDataType() const { return _dataType; }
    void bind(GLuint level = 0);
    void setParam(GLenum name, GLint value);
    void release();

    static texture* fromFile(std::string fileName);
    static texture* create(GLuint w, GLuint h, GLint internalFormat = GL_RGB32F, GLint format = GL_RGBA, GLint type = GL_FLOAT, GLuint level = 0, GLvoid* data = 0);
};

#endif