#pragma once
#include "types.h"
#include <glm\glm.hpp>

struct glyph
{
    uint index;
    float width;
    float height;
    float offsetX;
    float offsetY;
    float horiBearingX;
    float horiBearingY;
    float horiAdvance;
    float vertBearingX;
    float vertBearingY;
    float vertAdvance;
    glm::vec2 texPos;
    glm::vec2 texSize;
    void* data;
};
