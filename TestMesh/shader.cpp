#include "shader.h"
#include <fstream>
#include <iostream>

std::string shader::loadshaderFile(const std::string fileName)
{
    std::string filePath = fileName;

    std::string fileString = std::string();
    std::string line = std::string();

    std::ifstream file(filePath);

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

bool shader::validateshader(GLuint shader, const std::string file)
{
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    GLsizei length = 0;

    glGetShaderInfoLog(shader, BUFFER_SIZE, &length, buffer); // Ask OpenGL to give us the log associated with the shader

    if (length > 0) // If we have any information to display
    {
        std::cout << "shader " << shader << " (" << (file.c_str() ? file : "") << ") compile error: " << buffer << std::endl; // Output the information
        return false;
    }

    return true;
}

bool shader::validateProgram(GLuint program)
{
    const unsigned int BUFFER_SIZE = 512;
    char buffer[BUFFER_SIZE];
    memset(buffer, 0, BUFFER_SIZE);
    GLsizei length = 0;

    glGetProgramInfoLog(program, BUFFER_SIZE, &length, buffer); // Ask OpenGL to give us the log associated with the program
    if (length > 0) // If we have any information to display
    {
        std::cout << "Program " << program << " link error: " << buffer << std::endl; // Output the information
        return false;
    }

    glValidateProgram(program); // Get OpenGL to try validating the program
    GLint status;
    glGetProgramiv(program, GL_VALIDATE_STATUS, &status); // Find out if the shader program validated correctly
    if (status == GL_FALSE) // If there was a problem validating
    {
        std::cout << "Error validating shader " << program << std::endl; // Output which program had the error
        return false;
    }

    return true;
}

shader::shader(std::string name, std::string vFile, std::string fFile)
{
    _name = name;
    _vFile = vFile;
    _fFile = fFile;
    _initialized = false;
    _uniforms = new std::map<uint, uniform>();
}

bool shader::init()
{
    if (_initialized)
        return true;

    _initialized = true;

    _vertexshader = glCreateShader(GL_VERTEX_SHADER);
    _fragmentshader = glCreateShader(GL_FRAGMENT_SHADER);

    std::string vsText = shader::loadshaderFile(_vFile.c_str());
    std::string fsText = shader::loadshaderFile(_fFile.c_str());

    const char *vertexText = vsText.c_str();
    const char *fragmentText = fsText.c_str();

    glShaderSource(_vertexshader, 1, &vertexText, 0); // Set the source for the vertex shader to the loaded text
    glCompileShader(_vertexshader); // Compile the vertex shader
    shader::validateshader(_vertexshader, _vFile.c_str()); // Validate the vertex shader

    glShaderSource(_fragmentshader, 1, &fragmentText, 0); // Set the source for the fragment shader to the loaded text
    glCompileShader(_fragmentshader); // Compile the fragment shader
    bool result = shader::validateshader(_fragmentshader, _fFile.c_str()); // Validate the fragment shader

    if (!result)
    {
        LOG(_name);
        return false;
    }

    _id = glCreateProgram(); // Create a GLSL program
    glAttachShader(_id, _vertexshader); // Attach a vertex shader to the program
    glAttachShader(_id, _fragmentshader); // Attach the fragment shader to the program

    initAttribs();

    glLinkProgram(_id); // Link the vertex and fragment shaders in the program
    result = shader::validateProgram(_id); // Validate the shader program

    if (!result)
    {
        LOG(_name);
        return false;
    }


    return true;
}

shader::~shader()
{}

uint shader::getId()
{
    return _id;
}

void shader::addAttribute(std::string name)
{
    _attributes.push_back(name);
}

void shader::addUniform(std::string name, uint id)
{
    bind();
    (*_uniforms)[id] = uniform::create(_id, name);
    unbind();
}

uniform shader::getUniform(uint id)
{
    return (*_uniforms)[id];
}

void uniform::set(GLuint textureId, GLuint textureIndex)
{
    glActiveTexture(GL_TEXTURE0 + textureIndex);
    glBindTexture(GL_TEXTURE_2D, textureId);
    glUniform1i(_location, textureIndex);
}

void shader::initAttribs()
{
    for (unsigned int i = 0; i < _attributes.size(); i++)
        glBindAttribLocation(_id, i, _attributes[i].c_str());
}

void shader::bind()
{
    if (!_initialized)
    {
        LOG("shader not initialized:" + _name);
        return;
    }

    glUseProgram(_id);
}

void shader::unbind()
{
    if (!_initialized)
        return;

    glUseProgram(0);
}

void shader::release()
{
    if (!_initialized)
        return;

    glDetachShader(_id, _fragmentshader); // Detach the fragment shader
    glDetachShader(_id, _vertexshader); // Detach the vertex shader

    glDeleteShader(_fragmentshader); // Delete the fragment shader
    glDeleteShader(_vertexshader); // Delete the vertex shader
    glDeleteProgram(_id); // Delete the shader program
}
