#pragma once
#include "program.h"

#include "primitive.h"
#include "vertexBufferObject.h"
#include "elementBufferObject.h"
#include "parameterSemantic.h"

//enum semantic
//{
//    LOCAL,
//    MODEL,
//    VIEW,
//    PROJECTION,
//    MODELVIEW,
//    MODELVIEWPROJECTION,
//    MODELINVERSE,
//    VIEWINVERSE,
//    PROJECTIONINVERSE,
//    MODELVIEWINVERSE,
//    MODELVIEWPROJECTIONINVERSE,
//    MODELINVERSETRANSPOSE,
//    MODELVIEWINVERSETRANSPOSE,
//    VIEWPORT,
//    JOINTMATRIX
//};

enum parameterType
{
    TYPE_INT = 5125,
    TYPE_UNSIGNED_INT = 5126,
    TYPE_VEC2 = 35664,
    TYPE_VEC3 = 35665,
    TYPE_VEC4 = 35666,
    TYPE_MAT3 = 35675,
    TYPE_MAT4 = 35676,
    TYPE_SAMPLER2D = 35678
};

struct techniqueParameter
{
    techniqueParameter(long type, parameterSemantic semantic = parameterSemantic::NONE) :
        type(type),
        semantic(semantic)
    {
    }

    long type;
    parameterSemantic semantic;

    parameterType getType()
    {
        return (parameterType)type;
    }
};

struct techniqueUniform
{
    techniqueUniform(std::string identifier, std::string uniformName) :
        identifier(identifier),
        uniformName(uniformName)
    {
    }

    std::string identifier;
    std::string uniformName;
};

struct techniqueAttribute
{
    techniqueAttribute(std::string identifier, std::string attributeName) :
        identifier(identifier),
        attributeName(attributeName)
    {
    }

    std::string identifier;
    std::string attributeName;
};

template <typename T>
class uniformParameter
{
public:
    uniformParameter(std::string parameterName, std::string uniformName, parameterSemantic semantic) :
        parameterName(parameterName),
        uniformName(uniformName),
        semantic(semantic)
    {
    }

    std::string parameterName;
    std::string uniformName;
    parameterSemantic semantic;
    T uniformValue;
};

template <typename T>
class attributeParameter
{
public:
    attributeParameter(std::string parameterName, std::string attributeName, parameterSemantic semantic) :
        parameterName(parameterName),
        attributeName(attributeName),
        semantic(semantic)
    {
    }

    std::string parameterName;
    std::string attributeName;
    parameterSemantic semantic;
    T attributeValue;
};

class shadingTechnique
{
private:
    program* _program;
    GLuint _vao;
    primitive* _primitive;

    std::vector<techniqueUniform> _uniforms;
    std::vector<techniqueAttribute> _attributes;
    std::map<std::string, techniqueParameter*> _parameters;

    std::vector<uniformParameter<glm::vec2>*> _vec2UniformParameters;
    std::vector<uniformParameter<glm::vec3>*> _vec3UniformParameters;
    std::vector<uniformParameter<glm::vec4>*> _vec4UniformParameters;
    std::vector<uniformParameter<glm::mat3>*> _mat3UniformParameters;
    std::vector<uniformParameter<glm::mat4>*> _mat4UniformParameters;
    std::vector<uniformParameter<uint32_t>*> _uintUniformParameters;
    std::vector<uniformParameter<texture*>*> _textureUniformParameters;

    std::vector<attributeParameter<glm::vec2>*> _vec2AttributeParameters;
    std::vector<attributeParameter<glm::vec3>*> _vec3AttributeParameters;
    std::vector<attributeParameter<glm::vec4>*> _vec4AttributeParameters;
    std::vector<attributeParameter<glm::mat3>*> _mat3AttributeParameters;
    std::vector<attributeParameter<glm::mat4>*> _mat4AttributeParameters;
    std::vector<attributeParameter<uint32_t>*> _uintAttributeParameters;
    std::vector<attributeParameter<texture*>*> _textureAttributeParameters;

    std::vector<long> _enabledStates;

    std::map<std::string, glm::vec4> _vec4Values;
    std::unordered_map<parameterSemantic, glm::mat4> _mat4Semantics;

private:
    void sortParameter(std::string name, techniqueParameter* parameter);

public:
    shadingTechnique(
        std::vector<techniqueUniform> uniforms,
        std::vector<techniqueAttribute> attributes,
        std::map<std::string, techniqueParameter*> parameters,
        std::vector<long> enabledStates,
        primitive* primitive,
        program* program);
    void createVao();
    ~shadingTechnique();

    void bindValue(std::string parameterName, uint32_t value);

    void bindValue(std::string name, glm::vec4 value);
    void bindValue(std::string parameterName, glm::vec3 value);
    void bindValue(std::string parameterName, glm::mat3 value);
    void bindValue(std::string parameterName, glm::mat4 value);
    void bindValue(std::string parameterName, texture * value);
    void bindSemantic(parameterSemantic semantic, glm::vec3 value);
    void bindSemantic(parameterSemantic semantic, uint32_t value);
    void bindSemantic(parameterSemantic semantic, glm::vec4 value);
    void bindSemantic(parameterSemantic semantic, glm::mat3 value);
    void bindSemantic(parameterSemantic semantic, glm::mat4 value);

    void bind();
    void unbind();
    void render();
};
