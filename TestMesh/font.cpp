#include "font.h"
#include <gl\glew.h>
#include <iostream>
#include <algorithm>

FT_Library font::FreeTypeLibrary = nullptr;

font::font(std::string name, uint size)
{
    _size = size;
    _horizontalScale = 100.0f;
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

    byte lcdWeights[5];
    lcdWeights[0] = 0x10;
    lcdWeights[1] = 0x40;
    lcdWeights[2] = 0x70;
    lcdWeights[3] = 0x40;
    lcdWeights[4] = 0x10;

    FT_Library_SetLcdFilter(font::FreeTypeLibrary, FT_LCD_FILTER_LIGHT);
    FT_Library_SetLcdFilterWeights(font::FreeTypeLibrary, lcdWeights);
    FT_Load_Glyph(_fontFace, glyphIndex, FT_LOAD_RENDER | FT_LOAD_FORCE_AUTOHINT | FT_LOAD_TARGET_LCD);
    
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

        return glm::ivec2(kern.x >> 6, kern.y >> 6);
    }

    return glm::ivec2(0);
}

font::~font()
{
    FT_Done_Face(_fontFace);
}