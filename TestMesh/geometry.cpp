#include "geometry.h"

geometry::geometry()
{
    _vao = 0;

    _verticesVbo = 0;
    _texCoordsVbo = 0;
    _normalsVbo = 0;
    _indicesVbo = 0;
    _modelMatricesVbo = 0;
}

geometry::~geometry()
{
    glDeleteBuffers(1, &_verticesVbo);
    glDeleteBuffers(1, &_texCoordsVbo);
    glDeleteBuffers(1, &_normalsVbo);
    glDeleteBuffers(1, &_indicesVbo);
    glDeleteVertexArrays(1, &_vao);
}

geometry* geometry::create(std::vector<vertex> &vertices, std::vector<uint> &indices, std::vector<glm::mat4> &modelMatrices)
{
    auto g = new geometry();
    g->addVertices(vertices, indices, modelMatrices);
    return g;
}

void geometry::addVertices(std::vector<vertex> &vertices, std::vector<uint> &indices, std::vector<glm::mat4> &modelMatrices)
{
    _vertices = vertices;
    _indices = indices;

    GLuint verticesSize = vertices.size() * 3 * sizeof(GLfloat);
    GLuint texCoordsSize = vertices.size() * 2 * sizeof(GLfloat);
    GLuint normalsSize = vertices.size() * 3 * sizeof(GLfloat);
    _indicesCount = indices.size();
    _indicesSize = _indicesCount * sizeof(GLuint);
    GLfloat* vertexBuffer = new GLfloat[vertices.size() * 3];
    GLfloat* texCoordBuffer = new GLfloat[vertices.size() * 2];
    GLfloat* normalBuffer = new GLfloat[vertices.size() * 3];

    createBuffers(vertices, vertexBuffer, texCoordBuffer, normalBuffer);

    glGenVertexArrays(1, &_vao);
    glBindVertexArray(_vao);

    glGenBuffers(1, &_verticesVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _verticesVbo);
    glBufferData(GL_ARRAY_BUFFER, verticesSize, vertexBuffer, GL_STATIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer((GLuint)0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &_texCoordsVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _texCoordsVbo);
    glBufferData(GL_ARRAY_BUFFER, texCoordsSize, texCoordBuffer, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer((GLuint)1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &_normalsVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _normalsVbo);
    glBufferData(GL_ARRAY_BUFFER, normalsSize, normalBuffer, GL_STATIC_DRAW);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer((GLuint)2, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &_modelMatricesVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _modelMatricesVbo);

    for (unsigned int i = 0; i < 4; i++) {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4),
            (const GLvoid*)(sizeof(GLfloat) * i * 4));
        glVertexAttribDivisor(3 + i, 1);
    }

    auto instances = modelMatrices.size();
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * instances, &modelMatrices[0], GL_DYNAMIC_DRAW);

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

void geometry::bind()
{
    glBindVertexArray(_vao);
}

void geometry::unbind()
{
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);

    glBindVertexArray(0);
}

void geometry::render(uint instances, const std::vector<glm::mat4> &modelMatrices)
{
    /*glBindBuffer(GL_ARRAY_BUFFER, _modelMatricesVbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * instances, &modelMatrices[0], GL_DYNAMIC_DRAW);*/

    glDrawElementsInstanced(GL_TRIANGLES, _indicesCount, GL_UNSIGNED_INT, 0, instances);
}
