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
    float _dpi;
    float _horizontalScale;
    float _baseLine;
    float _ascender;
    float _lineHeight;
    bool _hasKerning;

    std::map<glyph*, glyphNode*> _glyphAtlas;
    glyphNode* _glyphAtlasRoot;
    GLuint _glyphAtlasId;
    GLint _glyphAtlasSize;
    texture* _glyphAtlasTexture;

public:
    font(std::string name, uint size);
    ~font();
    glyph* getGlyph(const ulong& glyphChar);
    glyph* getGlyph(const uint& glyphIndex);
    glm::ivec2 getKerning(glyph* firstGlyph, glyph* secondGlyph);

    int getLineHeight() const { return _lineHeight; }
    GLuint getGlyphAtlasId() const { return _glyphAtlasId; }
    GLint getGlyphAtlasSize() const { return _glyphAtlasSize; }
    texture* getGlyphTexture() const { return _glyphAtlasTexture; }
};