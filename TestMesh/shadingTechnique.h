#pragma once
#include "program.h"

class shadingTechnique
{
private:
    program* _program;
public:
    shadingTechnique(program* program);
    ~shadingTechnique();
};

