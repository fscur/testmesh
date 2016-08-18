#include "shader.h"

#include <fstream>

using namespace std;

shader::shader(const string& fileName) :
    _fileName(fileName)
{
    _stage = getStage(fileName);

    _content = load(_fileName);
    _source = _content.c_str();

    create();
    compile();
}

shader::shader(const char * source, shaderStage::shaderStage stage)
{
    _stage = stage;
    _source = source;
    create();
    compile();
}

shader::~shader()
{
    glDeleteShader(_id);
}

void shader::create()
{
    switch (_stage)
    {
    case shaderStage::vertex:
        _id = glCreateShader(GL_VERTEX_SHADER);
        break;
    case shaderStage::tesselationControl:
        _id = glCreateShader(GL_TESS_CONTROL_SHADER);
        break;
    case shaderStage::tesselationEvaluation:
        _id = glCreateShader(GL_TESS_EVALUATION_SHADER);
        break;
    case shaderStage::geometry:
        _id = glCreateShader(GL_GEOMETRY_SHADER);
        break;
    case shaderStage::fragment:
        _id = glCreateShader(GL_FRAGMENT_SHADER);
        break;
    case shaderStage::compute:
        _id = glCreateShader(GL_COMPUTE_SHADER);
        break;
    default:
        break;
    }
}

shaderStage::shaderStage shader::getStage(const string& fileName)
{
    auto ext = fileName.rfind('.');
    if (ext == string::npos)
    {
        return shaderStage::vertex;
    }

    string suffix = fileName.substr(ext + 1, string::npos);

    if (suffix == "vert")
        return shaderStage::vertex;
    else if (suffix == "tesc")
        return shaderStage::tesselationControl;
    else if (suffix == "tese")
        return shaderStage::tesselationEvaluation;
    else if (suffix == "geom")
        return shaderStage::geometry;
    else if (suffix == "frag")
        return shaderStage::fragment;
    else if (suffix == "comp")
        return shaderStage::compute;

    return shaderStage::vertex;
}

string shader::load(const string& fileName)
{
    string fileString;
    string line;

    std::ifstream file(fileName);

    if (file.is_open())
    {
        while (!file.eof())
        {
            getline(file, line);
            fileString.append(line);
            fileString.append("\n");
        }

        file.close();
    }

    return fileString;
}

shaderCompilationResult shader::compile()
{
    glShaderSource(_id, 1, &_source, 0);
    glCompileShader(_id);

    return validate();
}

shaderCompilationResult shader::validate()
{
    GLint success = 0;
    glGetShaderiv(_id, GL_COMPILE_STATUS, &success);

    const uint BUFFER_SIZE = 512;
    char shaderLogBuffer[BUFFER_SIZE] = { 0 };
    GLsizei shaderLogLength = 0;

    glGetShaderInfoLog(_id, BUFFER_SIZE, &shaderLogLength, shaderLogBuffer);

    auto succeeded = success == GL_TRUE;
    string resultMessage = "";

    if (shaderLogLength > 0)
    {
        resultMessage = string(shaderLogBuffer);
    }

    return shaderCompilationResult(succeeded, _id, _fileName, resultMessage);
}