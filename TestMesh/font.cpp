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
}

font::~font()
{

}