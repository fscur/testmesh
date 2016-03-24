#pragma once

#include <freetype\ft2build.h>
#include FT_FREETYPE_H
#include <string>

class font
{
private:
    static FT_Library _freeTypeLibrary;
    static bool _freeTypeInitialized;
public:
    FT_Face _fontFace;
public:
    font(std::string name, unsigned int size);
    ~font();
    
};