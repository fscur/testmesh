#pragma once
#include <map>
#include <vector>
#include "texture.h"
#include "textureAddress.h"

struct textureContainerLayout
{
    GLsizei w;
    GLsizei h;
    GLsizei levels;
    GLenum internalFormat;
    GLint wrapMode;
    GLint minFilter;
    GLint magFilter;

    textureContainerLayout(
        GLsizei w = 0,
        GLsizei h = 0,
        GLsizei levels = 0,
        GLenum internalFormat = GL_RGBA8,
        GLint wrapMode = GL_REPEAT,
        GLint minFilter = GL_LINEAR_MIPMAP_LINEAR,
        GLint magFilter = GL_LINEAR) :
        w(w),
        h(h),
        levels(levels),
        internalFormat(internalFormat),
        wrapMode(wrapMode),
        minFilter(minFilter),
        magFilter(magFilter)
    {
    }
};

class textureContainer
{
private:
    textureContainerLayout _layout;
    size_t _maxTextures;
    size_t _freeSpace;
    GLint _unit;
    bool _bindless;
    bool _sparse;

private:
    void create();
    void load(texture* texture);

public:
    GLuint id;
    GLuint64 handle;
    std::map<texture*, textureAddress> texturesAddresses;
    std::vector<texture*> textures;

public:
    textureContainer(
        textureContainerLayout layout,
        size_t maxTextures,
        GLint unit,
        bool bindless,
        bool sparse);

    ~textureContainer();

    bool add(texture* texture, textureAddress& textureAddress);
};