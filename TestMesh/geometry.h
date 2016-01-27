#ifndef _GEOMETRY_H_
#define _GEOMETRY_H_

#include "TestMesh.h"
#include "vertex.h"
#include <GL/glew.h>
#include <vector>

class geometry
{
private:
    GLuint _vao;
    GLuint _verticesVbo;
    GLuint _texCoordsVbo;
    GLuint _normalsVbo;
    GLuint _indicesVbo;
    GLsizei _indicesSize;

    std::vector<vertex> _vertices;
    std::vector<uint> _indices;

private:

    static void createBuffers(
        std::vector<vertex> &vertices,
        GLfloat* &vertexBuffer,
        GLfloat* &texCoordBuffer,
        GLfloat* &normalBuffer);

    void addVertices(std::vector<vertex> &vertices, std::vector<uint> &indices);

    geometry();

public:

    ~geometry();
    static geometry* create(std::vector<vertex> &vertices, std::vector<uint> &indices);
    void render();
};

#endif