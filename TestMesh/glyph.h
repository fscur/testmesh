#pragma once
#include "types.h"
#include <glm\glm.hpp>

struct glyphInfo
{
    glm::vec2 pos;
    float pad0;
    float pad1;
    glm::vec2 size;
    float pad2;
    float pad3;
    float shift;
    float pad4;
    float pad5;
    float pad6;
};

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
