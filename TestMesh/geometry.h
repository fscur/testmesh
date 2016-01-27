#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "TestMesh.h"
#include "vertex.h"
#include <GL\glew.h>
#include <vector>

class geometry
{
private:
    GLuint _max_instances = 1000;
    GLuint _vao;
    GLuint _verticesVbo;
    GLuint _texCoordsVbo;
    GLuint _normalsVbo;
    GLuint _indicesVbo;
    GLuint _modelMatricesVbo;
    GLsizei _indicesSize;
    GLsizei _indicesCount;

    std::vector<vertex> _vertices;
    std::vector<uint> _indices;
    bool _binded;

private:

    static void createBuffers(
        std::vector<vertex> &vertices,
        GLfloat* &vertexBuffer,
        GLfloat* &texCoordBuffer,
        GLfloat* &normalBuffer);

    void addVertices(std::vector<vertex> &vertices, std::vector<uint> &indices, std::vector<glm::mat4> &modelMatrices);

    geometry();

public:

    ~geometry();
    static geometry* create(std::vector<vertex> &vertices, std::vector<uint> &indices, std::vector<glm::mat4> &modelMatrices);
    void bind();
    void unbind();
    void render(uint instances, const std::vector<glm::mat4> &modelMatrices);
};

#endif