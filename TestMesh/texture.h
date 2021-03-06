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
private:
    GLuint _id;
    GLuint _textureType;

protected:
    GLuint _w;
    GLuint _h;

private:
    texture(GLuint id, GLuint w, GLuint h);

public:
    ~texture();

    GLuint getId() const { return _id; }
    GLuint getWidth() const { return _w; }
    GLuint getHeight() const { return _h; }
    GLuint gettextureType() const { return _textureType; }

    void bind(GLuint level = 0);
    void setParam(GLenum name, GLint value);
    void release();

    static texture* fromFile(std::string fileName);
    static texture* create(GLuint w, GLuint h, GLint internalFormat = GL_RGB32F, GLint format = GL_RGBA, GLint type = GL_FLOAT, GLuint level = 0, GLvoid* data = 0);
};

#endif