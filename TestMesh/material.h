#ifndef _MATERIAL_H_
#define _MATERIAL_H_

#include "color.h"
#include "texture.h"

struct material
{
private:
    color _diffuseColor;
    color _specularColor;
    texture* _diffuseTexture;
    texture* _normalTexture;

public:
    material(color diffuseColor, color specularColor, texture * diffuseTexture, texture * normalTexture) :
        _diffuseColor(diffuseColor), 
        _specularColor(specularColor), 
        _diffuseTexture(diffuseTexture), 
        _normalTexture(normalTexture)
    {
    }

    ~material() {}

    inline color getDiffuseColor() const { return _diffuseColor; }
    inline color getSpecularColor() const { return _specularColor; }
    inline texture* getDiffuseTexture() const { return _diffuseTexture; }
    inline texture* getNormalTexture() const { return _normalTexture; }
};

#endif
