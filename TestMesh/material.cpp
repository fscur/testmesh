#include "material.h"

material* material::default = nullptr;

material::material(std::string name) :
    _name(name)
{
}

material::~material()
{
    delete _technique;
}

void material::add(shadingTechnique* technique)
{
    _technique = technique;
}

void material::addValue(std::string name, glm::vec4 value)
{
    _vec4Values[name] = value;
}

void material::setMat4Semantic(parameterSemantic semantic, glm::mat4 value)
{
    _mat4Semantics[semantic] = value;
}

void material::bind()
{
    //position ?

    //modelViewMatrix
    //projectionMatrix
    for (auto& semanticValue : _mat4Semantics)
        _technique->setMat4Semantic((parameterSemantic)semanticValue.first, semanticValue.second);

    //emission
    for (auto& value : _vec4Values)
        _technique->setVec4Values(value.first, value.second);

    _technique->bind();
}

void material::unbind()
{
    _technique->unbind();
}
