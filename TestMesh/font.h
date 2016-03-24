#pragma once

#include "types.h"
#include "texture.h"
#include <freetype\ft2build.h>
#include FT_FREETYPE_H
#include <string>

class font
{
private:
    static FT_Library _freeTypeLibrary;
    static bool _freeTypeInitialized;

private:
    FT_Face _fontFace;
    uint _size;

public:
    font(std::string name, unsigned int size);
    ~font();

    texture* getGlyph(uint glyphIndex);
};