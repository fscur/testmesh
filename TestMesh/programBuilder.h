#pragma once
#include "program.h"

#include <string>

class programBuilder
{
public:
    static program* buildProgram(
        const std::string& vertexShaderName,
        const std::string& fragmentShaderName);

    static program * buildProgram(const char * vertexSource, const char * fragmentSource);
};