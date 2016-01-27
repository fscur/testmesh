#include <stdio.h>
#include <SDL\SDL_image.h>
#include "texture.h"
//#include "globals.h"
//#include "path.h"

static int SDL_invert_image(int pitch, int height, void* image_pixels)
{
    int index;
    void* temp_row;
    int height_div_2;

    temp_row = (void *)malloc(pitch);
    if (nullptr == temp_row)
    {
        SDL_SetError("Not enough memory for image inversion");
        return -1;
    }
    //if height is odd, don't need to swap middle row
    height_div_2 = (int)(height * .5);
    for (index = 0; index < height_div_2; index++) {
        //uses std::string.h
        memcpy((Uint8 *)temp_row,
            (Uint8 *)(image_pixels)+
            pitch * index,
            pitch);

        memcpy(
            (Uint8 *)(image_pixels)+
            pitch * index,
            (Uint8 *)(image_pixels)+
            pitch * (height - index - 1),
            pitch);
        memcpy(
            (Uint8 *)(image_pixels)+
            pitch * (height - index - 1),
            temp_row,
            pitch);
    }
    free(temp_row);
    return 0;
}

static int SDL_InvertSurface(SDL_Surface* image)
{
    if (nullptr == image)
    {
        SDL_SetError("Surface is NULL");
        return -1;
    }
    return(SDL_invert_image(image->pitch, image->h,
        image->pixels));
}

texture::texture(GLuint id, GLuint w, GLuint h)
{
    _id = id;
    _w = w;
    _h = h;
}

texture::~texture()
{
}

void texture::bind(GLuint level)
{
    glActiveTexture(GL_TEXTURE0 + level);
    glBindTexture(_textureType, _id);
}

void texture::setParam(GLenum name, GLint value)
{
    glTexParameteri(_textureType, name, value);
}

void texture::release()
{
    glDeleteTextures(1, &_id);
}

texture* texture::fromFile(std::string fileName)
{
    SDL_Surface* surface = IMG_Load(fileName.c_str());

    SDL_InvertSurface(surface);

    GLuint id, width, height;

    width = surface->w;
    height = surface->h;

    auto format = 0;
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

    auto t = new texture(0, width, height);
    t->_textureType = GL_TEXTURE_2D;
    t->_dataFormat = format;
    t->_dataType = GL_UNSIGNED_BYTE;

    auto totalBytes = surface->format->BitsPerPixel / 8;
    auto data = malloc(surface->w * surface->h * totalBytes);
    memcpy(data, surface->pixels, surface->w * surface->h * totalBytes);

    t->_data = (unsigned char*)data;

    //GLuint64 handle = glGetTextureHandleARB(id);
    //glMakeTextureHandleResidentARB(handle);

    //t->_handle = handle;

    return t;
}

texture* texture::create(GLuint w, GLuint h, GLint internalFormat, GLint format, GLint type, GLuint level, GLvoid* data)
{
    GLuint id = 0;
    glGenTextures(1, &id);
    glBindTexture(GL_TEXTURE_2D, id);
    glTexImage2D(GL_TEXTURE_2D, level, internalFormat, w, h, 0, format, type, data);
    auto t = new texture(id, w, h);

    t->_textureType = GL_TEXTURE_2D;
    return t;
}