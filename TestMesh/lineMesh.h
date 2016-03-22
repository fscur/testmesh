#ifndef _PHI_LINE_MESH_H_
#define _PHI_LINE_MESH_H_

#if WIN32
#include <GL/glew.h>
#else
#include <OpenGL/gl3.h>
#endif
#include <vector>
#include <glm/glm.hpp>

class lineMesh
{
private:
    GLuint _vao;
    GLuint _positionsVbo;
    GLuint _indicesVbo;

    GLfloat* _positionsBuffer;
    GLuint* _indicesBuffer;

    GLuint _indicesCount;

    GLuint _pSize;
    GLuint _iSize;

    std::vector<glm::vec3> _positions;
    std::vector<GLuint> _indices;

private:
    void storeBuffers();

protected:
    lineMesh();
    void addData(std::vector<glm::vec3> positions, std::vector<GLuint>& indices);

public:

    ~lineMesh();

    static lineMesh* create(std::string name, std::vector<glm::vec3> &positions, std::vector<GLuint> &indices);
    static lineMesh* create(
        GLuint positionsCount,
        GLfloat* positionsBuffer,
        GLuint indicesCount,
        GLuint* indicesBuffer);

    void bind();
    void render();
    void unbind();
    void change(std::vector<glm::vec3> &positions, std::vector<GLuint> &indices);
};

#endif