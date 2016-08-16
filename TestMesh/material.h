#pragma once
#include "program.h"
#include "shadingTechnique.h"

#include <glm\glm.hpp>

#include <string>

class material
{
private:
    std::string _name;
    std::map<std::string, uint32_t> _uintValues;
    std::map<std::string, glm::vec3> _vec3Values;
    std::map<std::string, glm::vec4> _vec4Values;
    std::map<std::string, glm::mat3> _mat3Values;
    std::map<std::string, glm::mat4> _mat4Values;

    std::unordered_map<parameterSemantic, glm::uint32_t> _uintSemantics;
    std::unordered_map<parameterSemantic, glm::vec3> _vec3Semantics;
    std::unordered_map<parameterSemantic, glm::vec4> _vec4Semantics;
    std::unordered_map<parameterSemantic, glm::mat3> _mat3Semantics;
    std::unordered_map<parameterSemantic, glm::mat4> _mat4Semantics;

    shadingTechnique* _technique;

public:
    static material* default;

public:
    material(std::string name, shadingTechnique* technique);
    ~material();

    void addValue(std::string name, uint32_t value);

    void addValue(std::string name, glm::vec3 value);
    void addValue(std::string name, glm::vec4 value);
    void addValue(std::string name, glm::mat3 value);
    void addValue(std::string name, glm::mat4 value);
    void bindSemantic(parameterSemantic semantic, uint32_t value);
    void bindSemantic(parameterSemantic semantic, glm::vec3 value);
    void bindSemantic(parameterSemantic semantic, glm::vec4 value);
    void bindSemantic(parameterSemantic semantic, glm::mat3 value);
    void bindSemantic(parameterSemantic semantic, glm::mat4 value);

    void bind();
    void unbind();
};

