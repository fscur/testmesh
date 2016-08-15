#pragma once
#include "program.h"
#include "shadingTechnique.h"

#include <glm\glm.hpp>

#include <string>

class material
{
private:
    std::string _name;
    std::map<std::string, glm::vec4> _vec4Values;

    std::unordered_map<parameterSemantic, glm::mat4> _mat4Semantics;

    shadingTechnique* _technique;

public:
    static material* default;

public:
    material(std::string name);
    ~material();

    void add(shadingTechnique* technique);
    void addValue(std::string name, glm::vec4 value);
    void setMat4Semantic(parameterSemantic semantic, glm::mat4 value);
    void bind();
    void unbind();
};

