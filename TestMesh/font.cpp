#include "font.h"
#include <iostream>

FT_Library font::_freeTypeLibrary;
bool font::_freeTypeInitialized;

font::font(std::string name, unsigned int size)
{
    if (!_freeTypeInitialized)
    {
        FT_Init_FreeType(&_freeTypeLibrary);
        _freeTypeInitialized = true;
    }

    FT_New_Face(_freeTypeLibrary, name.c_str(), 0, &_fontFace);

    FT_Set_Pixel_Sizes(_fontFace, 0, _size);
}

texture* font::getGlyph(uint glyphIndex)
{
    FT_GlyphSlot g = _fontFace->glyph;

    FT_Load_Char(_fontFace, glyphIndex, FT_LOAD_RENDER);

    return new texture(
        g->bitmap.width,
        g->bitmap.rows,
        GL_TEXTURE_2D,
        GL_RGBA8,
        GL_BGRA,
        GL_UNSIGNED_BYTE,
        g->bitmap.buffer,
        GL_CLAMP_TO_EDGE,
        GL_LINEAR_MIPMAP_LINEAR,
        GL_LINEAR,
        true);
}

font::~font()
{

}