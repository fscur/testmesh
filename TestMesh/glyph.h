#pragma once
#include "types.h"
#include <glm\glm.hpp>

struct glyph
{
    uint index;
    int  width;
    int  height;
    int  horiBearingX;
    int  horiBearingY;
    int  horiAdvance;
    int  vertBearingX;
    int  vertBearingY;
    int  vertAdvance;
    glm::vec2 texPos;
    glm::vec2 texSize;
};
