#include "material.h"

material* material::default = nullptr;

material::material(std::string name, shadingTechnique* technique) :
    _name(name),
    _technique(technique)
{
}

material::~material()
{
    delete _technique;
}

void material::addValue(std::string name, uint32_t value)
{
    _uintValues[name] = value;
}

void material::addValue(std::string name, glm::vec3 value)
{
    _vec3Values[name] = value;
}

void material::addValue(std::string name, glm::vec4 value)
{
    _vec4Values[name] = value;
}

void material::addValue(std::string name, glm::mat3 value)
{
    _mat3Values[name] = value;
}

void material::addValue(std::string name, glm::mat4 value)
{
    _mat4Values[name] = value;
}

void material::bindSemantic(parameterSemantic semantic, uint32_t value)
{
    _uintSemantics[semantic] = value;
}

void material::bindSemantic(parameterSemantic semantic, glm::vec3 value)
{
    _vec3Semantics[semantic] = value;
}

void material::bindSemantic(parameterSemantic semantic, glm::vec4 value)
{
    _vec4Semantics[semantic] = value;
}

void material::bindSemantic(parameterSemantic semantic, glm::mat3 value)
{
    _mat3Semantics[semantic] = value;
}

void material::bindSemantic(parameterSemantic semantic, glm::mat4 value)
{
    _mat4Semantics[semantic] = value;
}

void material::bind()
{
    for (auto& semanticValue : _uintSemantics)
        _technique->bindSemantic((parameterSemantic)semanticValue.first, semanticValue.second);

    for (auto& semanticValue : _vec3Semantics)
        _technique->bindSemantic((parameterSemantic)semanticValue.first, semanticValue.second);

    for (auto& semanticValue : _vec4Semantics)
        _technique->bindSemantic((parameterSemantic)semanticValue.first, semanticValue.second);

    for (auto& semanticValue : _mat3Semantics)
        _technique->bindSemantic((parameterSemantic)semanticValue.first, semanticValue.second);

    for (auto& semanticValue : _mat4Semantics)
        _technique->bindSemantic((parameterSemantic)semanticValue.first, semanticValue.second);

    for (auto& value : _uintValues)
        _technique->bindValue(value.first, value.second);

    for (auto& value : _vec3Values)
        _technique->bindValue(value.first, value.second);

    for (auto& value : _vec4Values)
        _technique->bindValue(value.first, value.second);

    for (auto& value : _mat3Values)
        _technique->bindValue(value.first, value.second);

    for (auto& value : _mat4Values)
        _technique->bindValue(value.first, value.second);

    _technique->bind();
}

void material::unbind()
{
    _technique->unbind();
}

void material::render()
{
    bind();
    _technique->render();
    unbind();
}
