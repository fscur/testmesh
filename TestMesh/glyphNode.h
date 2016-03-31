#pragma once
#include "glyph.h"
#include "rectangle.h"

class glyphNode
{
private:
    glyphNode* _left;
    glyphNode* _right;
    glyph* _glyph;

public:
    rectangle rect;

private:
    bool isLeaf();
    glyphNode* insert(glyph* glyph, glyphNode* node);

public:
    glyphNode(rectangle rect);

    glyphNode* insert(glyph* glyph);
};