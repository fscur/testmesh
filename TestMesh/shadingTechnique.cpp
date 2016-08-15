#include "shadingTechnique.h"

shadingTechnique::shadingTechnique()
{
}

shadingTechnique::~shadingTechnique()
{
    for (auto& pair : _parameters)
    {
        delete pair.second;
    }

    delete _program;
}

void shadingTechnique::addAttribute(std::string attributeName, std::string match)
{
    _attributes[attributeName] = match;
}

void shadingTechnique::addProgram(program * program)
{
    _program = program;
}

void shadingTechnique::addParameter(std::string name, techniqueParameter* parameter)
{
    _parameters[name] = parameter;
}

void shadingTechnique::addEnabledState(long state)
{
    _enabledStates.push_back(state);
}

void shadingTechnique::addUniform(std::string uniformName, std::string paremeterMatch)
{
    _uniforms[uniformName] = paremeterMatch;
}

void shadingTechnique::setVec4Values(std::string name, glm::vec4 value)
{
    _vec4Values[name] = value;
}

void shadingTechnique::setMat4Semantic(parameterSemantic semantic, glm::mat4 value)
{
    _mat4Semantics[semantic] = value;
}

glm::mat4 shadingTechnique::getMat4Semantic(std::string name, bool& exists)
{
    auto parameter = _parameters[name];

    if (parameter->semantic != parameterSemantic::NONE)
    {
        if (_parameters.find(name) != _parameters.end())
        {
            exists = true;
            return _mat4Semantics[parameter->semantic];
        }
    }

    exists = false;
    return glm::mat4();
}

std::string shadingTechnique::getUniformName(std::string name)
{
    return _uniforms[name];
}

void shadingTechnique::bind()
{
    _program->bind();

    for (auto& state : _enabledStates)
        glEnable(state);

    for (auto& value : _vec4Values)
    {
        auto uniformName = getUniformName(value.first);
        auto uniformValue = value.second;
        _program->setUniform(uniformName, uniformValue);
    }

    for (auto& uniform : _uniforms)
    {
        bool existUniform = false;
        auto parameterName = uniform.first;

        auto uniformName = uniform.second;
        auto uniformValue = getMat4Semantic(parameterName, existUniform);

        if (existUniform)
            _program->setUniform(uniformName, uniformValue);
    }
}

void shadingTechnique::unbind()
{
    for (auto& state : _enabledStates)
        glDisable(state);

    _program->unbind();
}