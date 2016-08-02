#pragma once

#include <GL\glew.h>

#include <vector>
#include <string>

#include "shaderCompilationResult.h"

class programCompilationResult
{
private:
    bool _succeeded;
    std::vector<shaderCompilationResult> _shadersResult;

public:
    programCompilationResult() :
        _succeeded(true)
    {
    }

    void add(shaderCompilationResult shaderResult)
    {
        _succeeded &= shaderResult.getSucceeded();
        _shadersResult.push_back(shaderResult);
    }

    std::string toString()
    {
        auto message = std::string();

        for (auto& result : _shadersResult)
            message.append(result.toString());

        return message;
    }

    bool getSucceeded()
    {
        return _succeeded;
    }
};