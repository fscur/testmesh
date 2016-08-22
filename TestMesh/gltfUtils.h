#pragma once
#include <string>

class gltfUtils
{
public:
    gltfUtils() = delete;

    static size_t getComponentCountFromType(const std::string& type);
    static size_t getComponentSizeFromComponentType(size_t componentType);
};

