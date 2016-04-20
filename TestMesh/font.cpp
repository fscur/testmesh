#include "font.h"
#include <gl\glew.h>
#include <iostream>
#include <algorithm>

FT_Library font::FreeTypeLibrary = nullptr;

font::font(std::string name, uint size)
{
    _hinting = true;
    _size = size;
    _horizontalScale = 64.0f;
    _dpi = 96.0f;

    FT_New_Face(font::FreeTypeLibrary, name.c_str(), 0, &_fontFace);
    FT_Set_Char_Size(_fontFace, _size * 64, 0, static_cast<int>(_dpi * _horizontalScale), static_cast<int>(_dpi));

    FT_Matrix matrix =
    {
        (int)((1.0 / _horizontalScale) * 0x10000L),
        (int)(0 * 0x10000L),
        (int)(0 * 0x10000L),
        (int)(1 * 0x10000L)
    };

    FT_Set_Transform(_fontFace, &matrix, NULL);


    _hasKerning = static_cast<bool>(FT_HAS_KERNING(_fontFace));
    _ascender = (_fontFace->size->metrics.ascender >> 6);
    _baseLine = (_fontFace->size->metrics.descender >> 6);
    _lineHeight = (_fontFace->size->metrics.height >> 6);
}

glyph* font::getGlyph(const ulong& glyphChar)
{
    auto glyphIndex = FT_Get_Char_Index(_fontFace, glyphChar);
    return getGlyph(glyphIndex);
}

glyph* font::getGlyph(const uint& glyphIndex)
{
    if (_glyphCache.find(glyphIndex) != _glyphCache.end())
        return _glyphCache[glyphIndex];

    FT_GlyphSlot glyphSlot = _fontFace->glyph;
    float primary = 1.f;
    float secondary = 0.0f;
    float tertiary = 0.0f;
    float norm = 1.0 / (primary + 2 * secondary + 2 * tertiary);
    
    byte lcdWeights[5];
    lcdWeights[0] = (byte)(tertiary * norm * 255);
    lcdWeights[1] = (byte)(secondary * norm * 255);
    lcdWeights[2] = (byte)(primary * norm * 255);
    lcdWeights[3] = (byte)(secondary * norm * 255);
    lcdWeights[4] = (byte)(tertiary * norm * 255);

    FT_Library_SetLcdFilter(font::FreeTypeLibrary, FT_LCD_FILTER_LIGHT);
    FT_Library_SetLcdFilterWeights(font::FreeTypeLibrary, lcdWeights);
    int flags = FT_LOAD_RENDER | FT_LOAD_TARGET_LCD;

    if (_hinting)
        flags |= FT_LOAD_FORCE_AUTOHINT;
    else
        flags |= FT_LOAD_NO_AUTOHINT | FT_LOAD_NO_HINTING;

    FT_Load_Glyph(_fontFace, glyphIndex, flags);
    
    auto buffer = glyphSlot->bitmap.buffer;
    auto w = glyphSlot->bitmap.width / 3;
    auto h = glyphSlot->bitmap.rows;

    auto g = new glyph();
    g->index = glyphIndex;
    g->width = w;
    g->height = h;
    g->offsetX = glyphSlot->bitmap_left;
    g->offsetY = glyphSlot->bitmap_top;
    g->horiBearingX = (glyphSlot->metrics.horiBearingX >> 6) / _horizontalScale;
    g->horiBearingY = (glyphSlot->metrics.horiBearingY >> 6);
    g->horiAdvance = (glyphSlot->metrics.horiAdvance >> 6) / _horizontalScale;
    g->vertBearingX = (glyphSlot->metrics.vertBearingX >> 6);
    g->vertBearingY = (glyphSlot->metrics.vertBearingY >> 6);
    g->vertAdvance = (glyphSlot->metrics.vertAdvance >> 6);
    g->data = buffer;
    _glyphCache[glyphIndex] = g;
    
    return g;
}

glm::ivec2 font::getKerning(glyph* firstGlyph, glyph* secondGlyph)
{
    if (_hasKerning && 
        firstGlyph != nullptr && 
        secondGlyph != nullptr)
    {
        FT_Vector kern;
        FT_Get_Kerning(
            _fontFace, 
            firstGlyph->index,
            secondGlyph->index,
            FT_KERNING_DEFAULT, 
            &kern);

        return glm::ivec2((kern.x >> 6) / _horizontalScale, kern.y >> 6);
    }

    return glm::ivec2(0);
}

font::~font()
{
    FT_Done_Face(_fontFace);
}