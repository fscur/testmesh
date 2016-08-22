#include "gltfUtils.h"

#include <GL\glew.h>

size_t gltfUtils::getComponentCountFromType(const std::string& type)
{
    if (type == "SCALAR")
        return 1;
    if (type == "VEC2")
        return 2;
    if (type == "VEC3")
        return 3;
    if (type == "VEC4")
        return 4;
    if (type == "MAT2")
        return 4;
    if (type == "MAT3")
        return 9;
    if (type == "MAT4")
        return 16;
}

size_t gltfUtils::getComponentSizeFromComponentType(size_t componentType)
{
    switch (componentType)
    {
    case GL_BYTE:
        return sizeof(GL_BYTE);
    case GL_UNSIGNED_BYTE:
        return sizeof(GL_UNSIGNED_BYTE);
    case GL_SHORT:
        return sizeof(GL_SHORT);
    case GL_UNSIGNED_SHORT:
        return sizeof(GL_UNSIGNED_SHORT);
    case GL_FLOAT:
        return sizeof(GL_FLOAT);
    default:
        throw new std::exception("Unkown componentType");
    }
}
