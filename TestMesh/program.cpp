#include "program.h"

#include <iostream>

program::program()
{
    _id = glCreateProgram();
}

program::~program()
{
    unbind();

    for (auto& shader : _shaders)
    {
        glDetachShader(_id, shader->getId());
        delete shader;
    }

    glDeleteProgram(_id);
}

void program::addShader(shader* shader)
{
    _shaders.push_back(shader);
    glAttachShader(_id, shader->getId());
}

void program::addBuffer(buffer* buffer)
{
    auto location = getBufferLocation(buffer);

    if (location == -1)
        __debugbreak();

    _buffers[location] = buffer;
}

void program::addAttribute(const std::string& attribute)
{
    _attributes.push_back(attribute);
}

void program::addUniform(uint location, std::string name)
{
    _uniformsNames[location] = name;
    _uniformsLocations[name] = location;
}

void program::setUniform(uint location, GLuint value)
{
    glUniform1i(location, value);
}

void program::setUniform(uint location, GLfloat value)
{
    glUniform1f(location, value);
}

void program::setUniform(uint location, color value)
{
    glUniform4f(location, value.R, value.G, value.B, value.A);
}

void program::setUniform(uint location, glm::mat3 value)
{
    glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
}

void program::setUniform(uint location, glm::mat4 value)
{
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

void program::setUniform(uint location, glm::vec2 value)
{
    glUniform2f(location, value.x, value.y);
}

void program::setUniform(uint location, glm::vec3 value)
{
    glUniform3f(location, value.x, value.y, value.z);
}

void program::setUniform(uint location, glm::vec4 value)
{
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void program::setUniform(uint location, bool value)
{
    glUniform1f(location, value ? 1.0f : 0.0f);
}

void program::setUniform(uint location, int value)
{
    glUniform1i(location, value);
}

void program::setUniform(uint location, GLuint64 value)
{
    glUniformHandleui64ARB(location, value);
}

void program::setUniform(uint location, std::vector<GLint> value)
{
    glUniform1iv(location, static_cast<GLsizei>(value.size()), &value[0]);
}

void program::setUniform(uint location, std::vector<GLuint64> value)
{
    glUniformHandleui64vARB(location, static_cast<GLsizei>(value.size()), value.data());
}

void program::setUniform(std::string name, GLuint value)
{
    int location = glGetUniformLocation(_id, name.c_str());
    glUniform1i(location, value);
}

void program::setUniform(std::string name, glm::mat3 value)
{
    int location = glGetUniformLocation(_id, name.c_str());
    glUniformMatrix3fv(location, 1, GL_FALSE, &value[0][0]);
}

void program::setUniform(std::string name, glm::mat4 value)
{
    int location = glGetUniformLocation(_id, name.c_str());
    glUniformMatrix4fv(location, 1, GL_FALSE, &value[0][0]);
}

void program::setUniform(std::string name, glm::vec2 value)
{
    int location = glGetUniformLocation(_id, name.c_str());
    glUniform2f(location, value.x, value.y);
}

void program::setUniform(std::string name, glm::vec3 value)
{
    int location = glGetUniformLocation(_id, name.c_str());
    glUniform3f(location, value.x, value.y, value.z);
}

void program::setUniform(std::string name, glm::vec4 value)
{
    int location = glGetUniformLocation(_id, name.c_str());
    glUniform4f(location, value.x, value.y, value.z, value.w);
}

void program::bindTextureToUniform(std::string name, texture* value)
{
    int location = glGetUniformLocation(_id, name.c_str());
    GLuint texId;
    glGenTextures(1, &texId);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texId);

    glTexImage2D(GL_TEXTURE_2D, 0, value->internalFormat, value->w, value->h, 0, value->dataFormat, value->dataType, value->data);
}

int32_t program::getAttributeLocation(std::string name)
{
    return glGetAttribLocation(_id, name.c_str());
}

void program::link()
{
    glLinkProgram(_id);

#if _DEBUG
    if (!validate())
        __debugbreak();
#endif
}

bool program::validate()
{
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    GLsizei length = 0;

    glGetProgramInfoLog(_id, BUFFER_SIZE, &length, buffer);

    if (length > 0)
        std::cout << "Program " + std::to_string(_id) + " link info:\n" + buffer << std::endl;

    GLint isLinked = 0;

    glGetProgramiv(_id, GL_LINK_STATUS, &isLinked);

    return isLinked == GL_TRUE;
}

GLint program::getBufferLocation(const buffer * const buffer) const
{
    GLenum programInterface = 0;
    switch (buffer->getTarget())
    {
    case bufferTarget::shader:
        programInterface = GL_SHADER_STORAGE_BLOCK;
        break;
    case bufferTarget::uniform:
        programInterface = GL_UNIFORM_BLOCK;
        break;
    default:
        break;
    }

    GLint numUniforms = 0;
    glGetProgramInterfaceiv(_id, programInterface, GL_ACTIVE_RESOURCES, &numUniforms);
    auto index = glGetProgramResourceIndex(_id, programInterface, buffer->getName().c_str());

    const GLenum props[1] = { GL_BUFFER_BINDING };
    GLint values[1] = { -1 };
    glGetProgramResourceiv(_id, programInterface, index, 1, props, 1, NULL, values);

    return values[0];
}

void program::bind()
{
    glUseProgram(_id);

    for (auto& pair : _buffers)
        pair.second->bindBufferBase(pair.first);
}

void program::unbind()
{
    glUseProgram(0);
}

programCompilationResult program::compile()
{
    auto compilationResult = programCompilationResult();

    for (auto shader : _shaders)
        compilationResult.add(shader->compile());

    return compilationResult;
}