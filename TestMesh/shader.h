#ifndef _SHADER_H_
#define _SHADER_H_

#include "TestMesh.h"
#include "texture.h"
#include "color.h"
#include <glm/glm.hpp>

#include <string>
#include <vector>
#include <map>

class uniform
{
private:
    GLuint _location;
public:
    uniform() {}
    ~uniform() {}

    static uniform create(GLuint shaderId, std::string name)
    {
        uniform u;
        u._location = glGetUniformLocation(shaderId, name.c_str());

        return u;
    }

    void set(GLuint value) { glUniform1ui(_location, value); }
    void set(GLint value) { glUniform1i(_location, value); }
    void set(GLfloat value) { glUniform1f(_location, value); }
    void set(color value) { glUniform4f(_location, value.R, value.G, value.B, value.A); }
    void set(glm::mat3 value) { glUniformMatrix3fv(_location, 1, GL_FALSE, &value[0][0]); }
    void set(glm::mat4 value) { glUniformMatrix4fv(_location, 1, GL_FALSE, &value[0][0]); }
    void set(glm::vec2 value) { glUniform2f(_location, value.x, value.y); }
    void set(glm::vec3 value) { glUniform3f(_location, value.x, value.y, value.z); }
    void set(glm::vec4 value) { glUniform4f(_location, value.x, value.y, value.z, value.w); }
    void set(std::vector<GLint> value) { glUniform1iv(_location, static_cast<GLsizei>(value.size()), value.data()); }

    void set(GLuint textureId, GLuint textureIndex);
};

class shader
{
private:
    uint _id;

    uint _vertexshader;
    uint _fragmentshader;

    bool _initialized;

    std::string _name;
    std::string _vFile;
    std::string _fFile;
    std::string _gFile;

    std::vector<std::string> _attributes;
    std::map<uint, uniform>* _uniforms;

private:
    static std::string loadshaderFile(const std::string fileName);
    static bool validateshader(GLuint shader, const std::string file = 0);
    static bool validateProgram(GLuint program);

protected:
    shader() {}

public:
    shader(std::string name, std::string vFile, std::string fFile);
    ~shader();

    bool init();

    void initAttribs();

    void addAttribute(std::string name);
    void addUniform(std::string name, uint id);
    uniform getUniform(uint id);

    void bind();
    void unbind();

    uint getId();

    void release();
};

#endif