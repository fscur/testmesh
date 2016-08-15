#pragma once
#include "program.h"

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

enum parameterSemantic
{
    NONE,
    LOCAL,
    MODEL,
    PROJECTION,
    MODELVIEW,
    MODELVIEWPROJECTION,
    MODELINVERSE,
    VIEWINVERSE,
    PROJECTIONINVERSE,
    MODELVIEWINVERSE,
    MODELVIEWPROJECTIONINVERSE,
    MODELINVERSETRANSPOSE,
    MODELVIEWINVERSETRANSPOSE,
    VIEWPORT,
    JOINTMATRIX,
    POSITION,
    NORMAL,
    TEXCOORD,
    COLOR,
    JOINT,
    WEIGHT
};


namespace std
{
    template<>
    struct hash< ::parameterSemantic>
    {
        typedef ::parameterSemantic argument_type;
        typedef std::underlying_type< argument_type >::type underlying_type;
        typedef std::hash< underlying_type >::result_type result_type;
        result_type operator()(const argument_type& arg) const
        {
            std::hash< underlying_type > hasher;
            return hasher(static_cast< underlying_type >(arg));
        }
    };
}

struct techniqueParameter
{
    techniqueParameter(long type, parameterSemantic semantic = parameterSemantic::NONE) :
        type(type),
        semantic(semantic)
    {
    }

    long type;
    parameterSemantic semantic;
};

class shadingTechnique
{
private:
    program* _program;

    std::map<std::string, std::string> _attributes;
    std::map<std::string, std::string> _uniforms;
    std::map<std::string, techniqueParameter*> _parameters;

    std::vector<long> _enabledStates;

    std::map<std::string, glm::vec4> _vec4Values;
    std::unordered_map<parameterSemantic, glm::mat4> _mat4Semantics;

private:
    glm::mat4 getMat4Semantic(std::string name, bool& exists);

    std::string getUniformName(std::string name);

public:
    shadingTechnique();
    ~shadingTechnique();

    void addAttribute(std::string attributeName, std::string match);
    void addProgram(program* program);
    void addParameter(std::string name, techniqueParameter* parameter);
    void addEnabledState(long state);
    void addUniform(std::string uniformName, std::string paremeterMatch);

    void setVec4Values(std::string name, glm::vec4 value);

    void setMat4Semantic(parameterSemantic semantic, glm::mat4 value);

    void bind();
    void unbind();
};
