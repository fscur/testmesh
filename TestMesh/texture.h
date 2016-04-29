#pragma once
#include <GL\glew.h>
#include "types.h"
#include <SDL\SDL.h>

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

	texture* load(std::string fileName)
	{
		SDL_Surface* surface = IMG_Load(fileName.c_str());

		SDL_InvertSurface(surface);

		GLuint id, width, height;
		glGenTextures(1, &id);

		width = surface->w;
		height = surface->h;

		// "Bind" the newly created texture : all future texture functions will modify this texture
		glBindTexture(GL_TEXTURE_2D, id);

		GLuint format = 0;
		switch (surface->format->BitsPerPixel)
		{
		case 24:
			if (surface->format->Rmask == 255)
				format = GL_RGB;
			else
				format = GL_BGR;
			break;
		case 32:
			if (surface->format->Rmask == 255)
				format = GL_RGBA;
			else
				format = GL_BGRA;
			break;
		default:
			break;
		}

		// Give the image to OpenGL
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

		// Give the image to OpenGL
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, format, GL_UNSIGNED_BYTE, surface->pixels);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glGenerateMipmap(GL_TEXTURE_2D);

		SDL_FreeSurface(surface);

		auto t = new texture(id, width, height);
		t->_textureType = GL_TEXTURE_2D;

		GLuint64 handle = glGetTextureHandleARB(id);
		glMakeTextureHandleResidentARB(handle);

		t->_handle = handle;

		return t;
	}

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