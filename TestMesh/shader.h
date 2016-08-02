#pragma once
#include "color.h"
#include "shaderCompilationResult.h"
#include "texture.h"

#include <string>

namespace shaderStage
{
    enum shaderStage
    {
        vertex,
        tesselationControl,
        tesselationEvaluation,
        geometry,
        fragment,
        compute
    };
}

class shader
{
private:
    shaderStage::shaderStage _stage;
    GLuint _id;
    std::string _content;
    std::string _fileName;

private:
    shaderStage::shaderStage getStage(const std::string& fileName);
    std::string load(const std::string& fileName);
    shaderCompilationResult validate();

public:
    static constexpr const char* VERT_EXT = ".vert";
    static constexpr const char* FRAG_EXT = ".frag";

public:
    shader(const std::string& fileName);
    ~shader();
    shaderCompilationResult compile();

    shaderStage::shaderStage getStage() const { return _stage; }
    GLuint getId() const { return _id; }
    std::string getContent() const { return _content; }
};
