#pragma once
#include "shader.h"
#include "buffer.h"
#include "programCompilationResult.h"

#include <glm\glm.hpp>

#include <string>
#include <vector>
#include <unordered_map>
#include <map>


struct uniform
{
    GLint location;
    std::string name;
};

class program
{
private:
    uint _id;
    std::vector<shader*> _shaders;

    std::map<unsigned int, GLuint> _uniforms;
    std::map<unsigned int, std::string> _uniformsNames;
    std::map<std::string, unsigned int> _uniformsLocations;
    std::vector<std::string> _attributes;
    std::unordered_map<GLuint, buffer*> _buffers;

private:
    bool validate();
    GLint getBufferLocation(const buffer* const buffer) const;

public:
    program();
    ~program();

    void addShader(shader* shader);
    void addBuffer(buffer* buffer);
    void addAttribute(const std::string& attribute);

    void addUniform(uint location, std::string name);
    void setUniform(uint location, GLuint value);
    void setUniform(uint location, GLfloat value);
    void setUniform(uint location, color value);
    void setUniform(uint location, glm::mat3 value);
    void setUniform(uint location, glm::mat4 value);
    void setUniform(uint location, glm::vec2 value);
    void setUniform(uint location, glm::vec3 value);
    void setUniform(uint location, glm::vec4 value);
    void setUniform(uint location, bool value);
    void setUniform(uint location, int value);
    void setUniform(uint location, GLuint64 value);
    void setUniform(uint location, std::vector<GLint> value);
    void setUniform(uint location, std::vector<GLuint64> value);

    void setUniform(std::string name, GLuint value);
    void setUniform(std::string name, GLfloat value);
    void setUniform(std::string name, color value);
    void setUniform(std::string name, glm::mat3 value);
    void setUniform(std::string name, glm::mat4 value);
    void setUniform(std::string name, glm::vec2 value);
    void setUniform(std::string name, glm::vec3 value);
    void setUniform(std::string name, glm::vec4 value);
    void setUniform(std::string name, bool value);
    void setUniform(std::string name, int value);
    void setUniform(std::string name, GLuint64 value);
    void setUniform(std::string name, std::vector<GLint> value);
    void setUniform(std::string name, std::vector<GLuint64> value);

    void link();
    void bind();
    void unbind();
    programCompilationResult compile();

    GLuint getId() const { return _id; }
};
