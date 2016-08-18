#pragma once
#include "material.h"
#include "scene.h"

#include <string>

class importer
{
public:
    static scene* importScene(std::string path);
    static material* importDefaultMaterial();
};
