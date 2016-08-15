#pragma once
#include "material.h"

#include <string>

class importer
{
public:
    static void importScene(std::string path);
    static material* importDefaultMaterial(std::string path);
};
