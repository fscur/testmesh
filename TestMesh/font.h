#pragma once

#include "types.h"
#include "texture.h"
#include <freetype\ft2build.h>
#include FT_FREETYPE_H
#include FT_LCD_FILTER_H
#include <string>
#include <map>
#include "glyphNode.h"
#include "glyph.h"

class font
{
public:
    static FT_Library FreeTypeLibrary;

private:
    std::map<uint, glyph*> _glyphCache;
    FT_Face _fontFace;
    uint _size;
    bool _hinting;
    float _dpi;
    float _horizontalScale;
    float _baseLine;
    float _ascender;
    float _lineHeight;
    bool _hasKerning;

public:
    font(std::string name, uint size);
    ~font();
    glyph* getGlyph(const ulong& glyphChar);
    glyph* getGlyph(const uint& glyphIndex);
    glm::ivec2 getKerning(glyph* firstGlyph, glyph* secondGlyph);
    int getLineHeight() const { return _lineHeight; }
};