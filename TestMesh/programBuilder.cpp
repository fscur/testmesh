#include "programBuilder.h"

#include "shader.h"
#include "program.h"

#include <string>

using namespace std;

program* programBuilder::buildProgram(
    const string& vertexShaderName,
    const string& fragmentShaderName)
{
    auto vertexShader = new shader(vertexShaderName);
    auto fragmentShader = new shader(fragmentShaderName);

    auto prog = new program();
    prog->addShader(vertexShader);
    prog->addShader(fragmentShader);
    prog->compile();
    prog->link();

    return prog;
}

program* programBuilder::buildProgram(
    const char* vertexSource,
    const char* fragmentSource)
{
    auto vertexShader = new shader(vertexSource, shaderStage::vertex);
    auto fragmentShader = new shader(fragmentSource, shaderStage::fragment);

    auto prog = new program();
    prog->addShader(vertexShader);
    prog->addShader(fragmentShader);
    prog->compile();
    prog->link();

    return prog;
}
