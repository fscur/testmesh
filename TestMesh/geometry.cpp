#include "geometry.h"

geometry::geometry()
{
    _vao = 0;

    _verticesVbo = 0;
    _texCoordsVbo = 0;
    _normalsVbo = 0;
    _indicesVbo = 0;
}

geometry::~geometry()
{
    glDeleteBuffers(1, &_verticesVbo);
    glDeleteBuffers(1, &_texCoordsVbo);
    glDeleteBuffers(1, &_normalsVbo);
    glDeleteBuffers(1, &_indicesVbo);
    glDeleteVertexArrays(1, &_vao);
}

geometry* geometry::create(std::vector<vertex> &vertices, std::vector<uint> &indices)
{
    auto g = new geometry();
    g->addVertices(vertices, indices);
    return g;
}

void geometry::addVertices(std::vector<vertex> &vertices, std::vector<uint> &indices)
{
    _vertices = vertices;
    _indices = indices;

    GLuint verticesSize = vertices.size() * 3 * sizeof(GLfloat);
    GLuint texCoordsSize = vertices.size() * 2 * sizeof(GLfloat);
    GLuint normalsSize = vertices.size() * 3 * sizeof(GLfloat);
    _indicesSize = indices.size() * sizeof(GLuint);

    GLfloat* vertexBuffer = new GLfloat[vertices.size() * 3];
    GLfloat* texCoordBuffer = new GLfloat[vertices.size() * 2];
    GLfloat* normalBuffer = new GLfloat[vertices.size() * 3];

    createBuffers(vertices, vertexBuffer, texCoordBuffer, normalBuffer);

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glGenBuffers(1, &_verticesVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _verticesVbo);
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertexBuffer, GL_STATIC_DRAW);

    glGenBuffers(1, &_texCoordsVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _texCoordsVbo);
    glBufferData(GL_ARRAY_BUFFER, texCoordsSize, texCoordBuffer, GL_STATIC_DRAW);

    glGenBuffers(1, &_normalsVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _normalsVbo);
    glBufferData(GL_ARRAY_BUFFER, normalsSize, normalBuffer, GL_STATIC_DRAW);

    glGenBuffers(1, &_indicesVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesVbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indicesSize, &indices[0], GL_STATIC_DRAW);

    delete[] vertexBuffer;
    delete[] texCoordBuffer;
    delete[] normalBuffer;
}

void geometry::createBuffers(std::vector<vertex> &vertices,
    GLfloat* &vertexBuffer,
    GLfloat* &texCoordBuffer,
    GLfloat* &normalBuffer)
{
    unsigned int vIndex = 0;
    unsigned int tIndex = 0;
    unsigned int nIndex = 0;

    for (auto vertex : vertices)
    {
        GLfloat x = vertex.GetPosition().x;
        GLfloat y = vertex.GetPosition().y;
        GLfloat z = vertex.GetPosition().z;

        GLfloat u = vertex.GetTexCoord().x;
        GLfloat v = vertex.GetTexCoord().y;

        GLfloat r0 = vertex.GetNormal().x;
        GLfloat s0 = vertex.GetNormal().y;
        GLfloat t0 = vertex.GetNormal().z;

        vertexBuffer[vIndex++] = x;
        vertexBuffer[vIndex++] = y;
        vertexBuffer[vIndex++] = z;

        texCoordBuffer[tIndex++] = u;
        texCoordBuffer[tIndex++] = v;

        normalBuffer[nIndex++] = r0;
        normalBuffer[nIndex++] = s0;
        normalBuffer[nIndex++] = t0;
    }
}

void geometry::render()
{
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, _verticesVbo);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, _texCoordsVbo);
    glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, _normalsVbo);
    glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    /*glDrawArrays(GL_POINTS, 0, _vertices.size());
    glEnable(GL_PROGRAM_POINT_SIZE);
   */
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesVbo);
    glDrawElements(GL_TRIANGLES, _indicesSize, GL_UNSIGNED_INT, 0);



    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}
