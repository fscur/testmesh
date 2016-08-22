#include "shadingTechnique.h"

shadingTechnique::shadingTechnique(
    std::vector<techniqueUniform> uniforms,
    std::vector<techniqueAttribute> attributes,
    std::map<std::string, techniqueParameter*> parameters,
    std::vector<long> enabledStates,
    primitive* primitive,
    program* program) :
    _uniforms(uniforms),
    _attributes(attributes),
    _parameters(parameters),
    _enabledStates(enabledStates),
    _primitive(primitive),
    _program(program)
{
    for (auto& parameter : parameters)
    {
        sortParameter(parameter.first, parameter.second);
    }

    createVao();

    //for (auto& vbo : vbos)
    //{
    //    vbo.bind();
    //    vbo->setLocation();
    //    vbo->

    //}

    //

    //glBindBuffer(normalsBufferView.target, normalsVbo);
    //glEnableVertexAttribArray(normalsLocation);
    //glVertexAttribPointer(normalsLocation, 3, normalsAcessor.componentType, GL_FALSE, normalsAcessor.byteStride, 0);

    //glBindBuffer(verticesBufferView.target, verticesVbo);
    //glEnableVertexAttribArray(verticesLocation);
    //glVertexAttribPointer(verticesLocation, 3, verticesAcessor.componentType, GL_FALSE, verticesAcessor.byteStride, 0);

    //glBindBuffer(indicesBufferView.target, indicesEbo);
}

void shadingTechnique::createVao()
{
    glGenVertexArrays(1, &_vao);
    for (auto& attribute : _attributes)
    {
        auto attributeLocation = _program->getAttributeLocation(attribute.attributeName);

        auto attributeParameter = _parameters.find(attribute.identifier)->second;
        auto vbo = _primitive->findVbo(attributeParameter->semantic);

        vbo->bindTo(_vao, attributeLocation);
    }

    _primitive->getEbo()->bindTo(_vao);
}

shadingTechnique::~shadingTechnique()
{
    for (auto& pair : _parameters)
    {
        delete pair.second;
    }

    for (auto& uniform : _mat3UniformParameters)
        delete uniform;

    for (auto& uniform : _mat4UniformParameters)
        delete uniform;

    for (auto& uniform : _vec3UniformParameters)
        delete uniform;

    for (auto& uniform : _vec4UniformParameters)
        delete uniform;

    for (auto& uniform : _uintUniformParameters)
        delete uniform;

    for (auto& attribute : _vec3AttributeParameters)
        delete attribute;

    for (auto& attribute : _vec4AttributeParameters)
        delete attribute;

    for (auto& attribute : _mat3AttributeParameters)
        delete attribute;

    for (auto& attribute : _mat4AttributeParameters)
        delete attribute;

    for (auto& attribute : _uintAttributeParameters)
        delete attribute;

    delete _program;
}

void shadingTechnique::sortParameter(std::string parameterName, techniqueParameter* parameter)
{
    for (auto& uniform : _uniforms)
    {
        if (uniform.identifier == parameterName)
        {
            switch (parameter->getType())
            {
            case parameterType::TYPE_UNSIGNED_INT:
                _uintUniformParameters.push_back(new uniformParameter<uint32_t>(parameterName, uniform.uniformName, parameter->semantic));
                return;
            case parameterType::TYPE_VEC3:
                _vec3UniformParameters.push_back(new uniformParameter<glm::vec3>(parameterName, uniform.uniformName, parameter->semantic));
                return;
            case parameterType::TYPE_VEC4:
                _vec4UniformParameters.push_back(new uniformParameter<glm::vec4>(parameterName, uniform.uniformName, parameter->semantic));
                return;
            case parameterType::TYPE_MAT3:
                _mat3UniformParameters.push_back(new uniformParameter<glm::mat3>(parameterName, uniform.uniformName, parameter->semantic));
                return;
            case parameterType::TYPE_MAT4:
                _mat4UniformParameters.push_back(new uniformParameter<glm::mat4>(parameterName, uniform.uniformName, parameter->semantic));
                return;
            default:
                break;
            }
        }
    }

    for (auto& attribute : _attributes)
    {
        if (attribute.identifier == parameterName)
        {
            switch (parameter->getType())
            {
            case parameterType::TYPE_UNSIGNED_INT:
                _uintAttributeParameters.push_back(new attributeParameter<uint32_t>(parameterName, attribute.attributeName, parameter->semantic));
                return;
            case parameterType::TYPE_VEC3:
                _vec3AttributeParameters.push_back(new attributeParameter<glm::vec3>(parameterName, attribute.attributeName, parameter->semantic));
                return;
            case parameterType::TYPE_VEC4:
                _vec4AttributeParameters.push_back(new attributeParameter<glm::vec4>(parameterName, attribute.attributeName, parameter->semantic));
                return;
            case parameterType::TYPE_MAT3:
                _mat3AttributeParameters.push_back(new attributeParameter<glm::mat3>(parameterName, attribute.attributeName, parameter->semantic));
                return;
            case parameterType::TYPE_MAT4:
                _mat4AttributeParameters.push_back(new attributeParameter<glm::mat4>(parameterName, attribute.attributeName, parameter->semantic));
                return;
            default:
                break;
            }
        }
    }

    throw new std::exception("Parameter not found");
}

void shadingTechnique::bindValue(std::string parameterName, uint32_t value)
{
    for (auto& uniformParameter : _uintUniformParameters)
    {
        if (uniformParameter->parameterName == parameterName)
        {
            uniformParameter->uniformValue = value;
            return;
        }
    }

    for (auto& attributeParameter : _uintAttributeParameters)
    {
        if (attributeParameter->parameterName == parameterName)
        {
            attributeParameter->attributeValue = value;
            return;
        }
    }

    throw new std::exception("Parameter not found");
}

void shadingTechnique::bindValue(std::string parameterName, glm::vec3 value)
{
    for (auto& uniformParameter : _vec3UniformParameters)
    {
        if (uniformParameter->parameterName == parameterName)
        {
            uniformParameter->uniformValue = value;
            return;
        }
    }

    for (auto& attributeParameter : _vec3AttributeParameters)
    {
        if (attributeParameter->parameterName == parameterName)
        {
            attributeParameter->attributeValue = value;
            return;
        }
    }

    throw new std::exception("Parameter not found");
}

void shadingTechnique::bindValue(std::string parameterName, glm::vec4 value)
{
    for (auto& uniformParameter : _vec4UniformParameters)
    {
        if (uniformParameter->parameterName == parameterName)
        {
            uniformParameter->uniformValue = value;
            return;
        }
    }

    for (auto& attributeParameter : _vec4AttributeParameters)
    {
        if (attributeParameter->parameterName == parameterName)
        {
            attributeParameter->attributeValue = value;
            return;
        }
    }

    throw new std::exception("Parameter not found");
}

void shadingTechnique::bindValue(std::string parameterName, glm::mat3 value)
{
    for (auto& uniformParameter : _mat3UniformParameters)
    {
        if (uniformParameter->parameterName == parameterName)
        {
            uniformParameter->uniformValue = value;
            return;
        }
    }

    for (auto& attributeParameter : _mat3AttributeParameters)
    {
        if (attributeParameter->parameterName == parameterName)
        {
            attributeParameter->attributeValue = value;
            return;
        }
    }

    throw new std::exception("Parameter not found");
}

void shadingTechnique::bindValue(std::string parameterName, glm::mat4 value)
{
    for (auto& uniformParameter : _mat4UniformParameters)
    {
        if (uniformParameter->parameterName == parameterName)
        {
            uniformParameter->uniformValue = value;
            return;
        }
    }

    for (auto& attributeParameter : _mat4AttributeParameters)
    {
        if (attributeParameter->parameterName == parameterName)
        {
            attributeParameter->attributeValue = value;
            return;
        }
    }

    throw new std::exception("Parameter not found");
}

void shadingTechnique::bindSemantic(parameterSemantic semantic, glm::vec3 value)
{
    for (auto& uniformParameter : _vec3UniformParameters)
    {
        if (uniformParameter->semantic == semantic)
        {
            uniformParameter->uniformValue = value;
            return;
        }
    }

    for (auto& attributeParameter : _vec3AttributeParameters)
    {
        if (attributeParameter->semantic == semantic)
        {
            attributeParameter->attributeValue = value;
            return;
        }
    }

    throw new std::exception("Parameter semantic not found");
}

void shadingTechnique::bindSemantic(parameterSemantic semantic, uint32_t value)
{
    for (auto& uniformParameter : _uintUniformParameters)
    {
        if (uniformParameter->semantic == semantic)
        {
            uniformParameter->uniformValue = value;
            return;
        }
    }

    for (auto& attributeParameter : _uintAttributeParameters)
    {
        if (attributeParameter->semantic == semantic)
        {
            attributeParameter->attributeValue = value;
            return;
        }
    }

    throw new std::exception("Parameter semantic not found");
}

void shadingTechnique::bindSemantic(parameterSemantic semantic, glm::vec4 value)
{
    for (auto& uniformParameter : _vec4UniformParameters)
    {
        if (uniformParameter->semantic == semantic)
        {
            uniformParameter->uniformValue = value;
            return;
        }
    }

    for (auto& attributeParameter : _vec4AttributeParameters)
    {
        if (attributeParameter->semantic == semantic)
        {
            attributeParameter->attributeValue = value;
            return;
        }
    }

    throw new std::exception("Parameter semantic not found");
}

void shadingTechnique::bindSemantic(parameterSemantic semantic, glm::mat3 value)
{
    for (auto& uniformParameter : _mat3UniformParameters)
    {
        if (uniformParameter->semantic == semantic)
        {
            uniformParameter->uniformValue = value;
            return;
        }
    }

    for (auto& attributeParameter : _mat3AttributeParameters)
    {
        if (attributeParameter->semantic == semantic)
        {
            attributeParameter->attributeValue = value;
            return;
        }
    }

    throw new std::exception("Parameter semantic not found");
}

void shadingTechnique::bindSemantic(parameterSemantic semantic, glm::mat4 value)
{
    for (auto& uniformParameter : _mat4UniformParameters)
    {
        if (uniformParameter->semantic == semantic)
        {
            uniformParameter->uniformValue = value;
            return;
        }
    }

    for (auto& attributeParameter : _mat4AttributeParameters)
    {
        if (attributeParameter->semantic == semantic)
        {
            attributeParameter->attributeValue = value;
            return;
        }
    }

    throw new std::exception("Parameter semantic not found");
}

void shadingTechnique::bind()
{
    _program->bind();

    for (auto& state : _enabledStates)
        glEnable(state);

    for (auto& uniformParameter : _uintUniformParameters)
        _program->setUniform(uniformParameter->uniformName, uniformParameter->uniformValue);

    for (auto& uniformParameter : _vec3UniformParameters)
        _program->setUniform(uniformParameter->uniformName, uniformParameter->uniformValue);

    for (auto& uniformParameter : _vec4UniformParameters)
        _program->setUniform(uniformParameter->uniformName, uniformParameter->uniformValue);

    for (auto& uniformParameter : _mat3UniformParameters)
        _program->setUniform(uniformParameter->uniformName, uniformParameter->uniformValue);

    for (auto& uniformParameter : _mat4UniformParameters)
        _program->setUniform(uniformParameter->uniformName, uniformParameter->uniformValue);

    glBindVertexArray(_vao);
}

void shadingTechnique::unbind()
{
    for (auto& state : _enabledStates)
        glDisable(state);

    _program->unbind();

    glBindVertexArray(0);
}

void shadingTechnique::render()
{
    _primitive->render();
}
