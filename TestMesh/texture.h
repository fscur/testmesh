#pragma once
#include <GL\glew.h>
#include "types.h"

struct texture
{
public:
    GLuint id;
    GLuint64 handle;
    GLuint w;
    GLuint h;
    GLenum type;
    GLenum internalFormat;
    GLenum dataFormat;
    GLenum dataType;
    byte* data;
    GLint wrapMode;
    GLint minFilter;
    GLint magFilter;
    bool generateMipmaps;

public:
    texture(
        uint w, 
        uint h, 
        GLenum type = GL_TEXTURE_2D,
        GLenum internalFormat = GL_RGBA8,
        GLenum dataFormat = GL_BGRA,
        GLenum dataType = GL_UNSIGNED_BYTE,
        byte* data = nullptr,
        GLint wrapMode = GL_REPEAT,
        GLint minFilter = GL_LINEAR_MIPMAP_LINEAR,
        GLint magFilter = GL_LINEAR,
        bool generateMipmaps = true) :
        w(w),
        h(h),
        type(type),
        internalFormat(internalFormat),
        dataFormat(dataFormat),
        dataType(dataType),
        data(data),
        wrapMode(wrapMode),
        minFilter(minFilter),
        magFilter(magFilter),
        generateMipmaps(generateMipmaps)
    {
    }

    ~texture() {}

	void generate()
	{
		glGenTextures(1, &id);
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, internalFormat, w, h, 0, dataFormat, dataType, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, wrapMode);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, magFilter);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, minFilter);

		if (generateMipmaps)
			glGenerateMipmap(GL_TEXTURE_2D);
	}
};