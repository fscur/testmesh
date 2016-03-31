#pragma once
#include <vector>
#include <map>
#include "textureContainer.h"

class texturesManager
{
private:
    std::map<
        std::tuple<GLsizei, GLsizei, GLsizei, GLenum, GLint, GLint, GLint>,
        std::vector<textureContainer*>> _containers;

    bool _bindless;
    bool _sparse;

    GLint _currentTextureUnit;
    GLint _maxTextureUnits;
    size_t _maxContainerSize;

public:
    std::vector<GLint> units;
    std::vector<GLuint64> handles;

public:
    texturesManager(bool bindless, bool sparse, size_t containerSize);
    ~texturesManager();

    textureAddress add(texture* texture);
    void reserveContainer(textureContainerLayout layout, size_t size);
};