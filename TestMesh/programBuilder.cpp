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
