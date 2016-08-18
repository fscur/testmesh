#pragma once
#include "program.h"

#include <string>

class programBuilder
{
public:
    static program* buildProgramFromFile(
        const std::string& vertexShaderName,
        const std::string& fragmentShaderName);

    static program * buildProgramFromSource(const char * vertexSource, const char * fragmentSource);
};