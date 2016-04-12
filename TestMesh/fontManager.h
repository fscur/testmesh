#pragma once
#include "glyph.h"
#include "font.h"
#include "texturesManager.h"
#include "types.h"
#include <freetype\ft2build.h>
#include FT_FREETYPE_H
#include <algorithm>

class fontManager
{
private:
    static FT_Library _freeTypeLibrary;
    static bool _freeTypeInitialized;

private:
    std::map<
        std::tuple<font*, ulong>,
        glyph*> _glyphCache;

    std::map<glyph*, glyphNode*> _glyphAtlas;
    glyphNode* _glyphAtlasRoot;
    GLuint _glyphAtlasId;
    GLint _glyphAtlasSize;
    texture* _glyphAtlasTexture;
    texturesManager* _texturesManager;
    textureAddress _glyphAtlasTextureAddress;

public:
    fontManager(texturesManager* texturesManager);
    ~fontManager();
    glyph* getGlyph(font* font, const ulong& glyphChar);
    GLuint getGlyphAtlasId() const { return _glyphAtlasId; }
    GLint getGlyphAtlasSize() const { return _glyphAtlasSize; }
    texture* getGlyphTexture() const { return _glyphAtlasTexture; }
};