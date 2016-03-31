#include "font.h"
#include <gl\glew.h>
#include <iostream>
#include <algorithm>

FT_Library font::_freeTypeLibrary;
bool font::_freeTypeInitialized;

font::font(std::string name, uint size)
{
    _size = size;

    if (!_freeTypeInitialized)
    {
        FT_Init_FreeType(&_freeTypeLibrary);
        _freeTypeInitialized = true;
    }

    FT_New_Face(_freeTypeLibrary, name.c_str(), 0, &_fontFace);
    FT_Set_Char_Size(_fontFace, 0, _size * 64, 96, 96);
    
    _hasKerning = static_cast<bool>(FT_HAS_KERNING(_fontFace));
    _baseLine = _fontFace->size->metrics.descender >> 6;
    _ascender = _fontFace->size->metrics.ascender >> 6;
    _lineHeight = _fontFace->size->metrics.height >> 6;

    auto glyphAtlasSize = 0;
    glGetIntegerv(GL_MAX_TEXTURE_SIZE, &glyphAtlasSize);

    _glyphAtlasSize = std::min(glyphAtlasSize, 128);
    _glyphAtlasRoot = new glyphNode(rectangle(0, 0, _glyphAtlasSize, _glyphAtlasSize));

    glGenTextures(1, &_glyphAtlasId);
    glBindTexture(GL_TEXTURE_2D, _glyphAtlasId);
    glTexImage2D(
        GL_TEXTURE_2D,
        0,
        GL_R8,
        _glyphAtlasSize,
        _glyphAtlasSize,
        0,
        GL_RED,
        GL_UNSIGNED_BYTE,
        NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
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
    FT_Load_Glyph(_fontFace, glyphIndex, FT_LOAD_RENDER);

    auto w = glyphSlot->metrics.width >> 6;
    auto h = glyphSlot->metrics.height >> 6;

    auto g = new glyph();
    g->index = glyphIndex;
    g->width = w;
    g->height = h;
    g->horiBearingX = glyphSlot->metrics.horiBearingX >> 6;
    g->horiBearingY = glyphSlot->metrics.horiBearingY >> 6;
    g->horiAdvance = glyphSlot->metrics.horiAdvance >> 6;
    g->vertBearingX = glyphSlot->metrics.vertBearingX >> 6;
    g->vertBearingY = glyphSlot->metrics.vertBearingY >> 6;
    g->vertAdvance = glyphSlot->metrics.vertAdvance >> 6;

    _glyphCache[glyphIndex] = g;
    
    if (w == 0 || h == 0)
        return g;

    auto glyphNode = _glyphAtlasRoot->insert(g);
    _glyphAtlas[g] = glyphNode;

    auto x = glyphNode->rect.x;
    auto y = glyphNode->rect.y;
    auto r = 1.0f / (float)_glyphAtlasSize;

    g->texPos = glm::vec2((float)x * r, (float)y * r);
    g->texSize = glm::vec2((float)w * r, (float)h * r);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTextureSubImage2D(
        _glyphAtlasId, 
        0, 
        x, 
        y, 
        w, 
        h, 
        GL_RED, 
        GL_UNSIGNED_BYTE, 
        glyphSlot->bitmap.buffer);
    glGenerateMipmap(GL_TEXTURE_2D);

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