#include "geometry.h"

#include "octree.h"

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

geometry* geometry::create(std::vector<vertex> &vertices, std::vector<short> &indices)
{
    auto g = new geometry();
    g->addVertices(vertices, indices);
    return g;
}

void geometry::addVertices(std::vector<vertex> &vertices, std::vector<short> &indices)
{
    _vertices = vertices;
    _indices = indices;

    GLuint verticesSize = vertices.size() * 3 * sizeof(GLfloat);
    GLuint texCoordsSize = vertices.size() * 2 * sizeof(GLfloat);
    GLuint normalsSize = vertices.size() * 3 * sizeof(GLfloat);
    _indicesSize = indices.size() * sizeof(GLshort);

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

    glGenBuffers(1, &_normalsVbo);
    glBindBuffer(GL_ARRAY_BUFFER, _normalsVbo);
    glBufferData(GL_ARRAY_BUFFER, normalsSize, normalBuffer, GL_STATIC_DRAW);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer((GLuint)1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //glGenBuffers(1, &_texCoordsVbo);
    //glBindBuffer(GL_ARRAY_BUFFER, _texCoordsVbo);
    //glBufferData(GL_ARRAY_BUFFER, texCoordsSize, texCoordBuffer, GL_STATIC_DRAW);
    //glEnableVertexAttribArray(2);
    //glVertexAttribPointer((GLuint)2, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glGenBuffers(1, &_indicesVbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesVbo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, _indicesSize, &indices[0], GL_STATIC_DRAW);

    glBindVertexArray(0);

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
    glBindVertexArray(_vao);
    glDrawElements(GL_TRIANGLES, _indicesSize, GL_UNSIGNED_SHORT, 0);
    glBindVertexArray(0);
}

geometry* geometry::createCube()
{
    auto _octree = new octree(aabb(glm::vec3(-0.7, -0.7, -0.7), glm::vec3(0.7, 0.7, 0.7)), 5, 100);
    auto vertices = std::vector<vertex>();
    auto indices = std::vector<short>();
    auto addVertex = [&](vertex& vertex)
    {
        uint index = -1;

        if (_octree->insert(vertex, index))
            vertices.push_back(vertex);

        indices.push_back(index);
    };

    auto s = vertices.size();

    addVertex(vertex(glm::vec3(-0.5f, -0.5f, +0.5f), glm::vec2(0.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    addVertex(vertex(glm::vec3(+0.5f, -0.5f, +0.5f), glm::vec2(1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    addVertex(vertex(glm::vec3(+0.5f, +0.5f, +0.5f), glm::vec2(1.0, 1.0), glm::vec3(0.0, 0.0, 1.0)));
    addVertex(vertex(glm::vec3(+0.5f, +0.5f, +0.5f), glm::vec2(1.0, 1.0), glm::vec3(0.0, 0.0, 1.0)));
    addVertex(vertex(glm::vec3(-0.5f, +0.5f, +0.5f), glm::vec2(0.0, 1.0), glm::vec3(0.0, 0.0, 1.0)));
    addVertex(vertex(glm::vec3(-0.5f, -0.5f, +0.5f), glm::vec2(0.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));

    addVertex(vertex(glm::vec3(+0.5f, -0.5f, +0.5f), glm::vec2(0.0, 0.0), glm::vec3(1.0, 0.0, 0.0)));
    addVertex(vertex(glm::vec3(+0.5f, -0.5f, -0.5f), glm::vec2(1.0, 0.0), glm::vec3(1.0, 0.0, 0.0)));
    addVertex(vertex(glm::vec3(+0.5f, +0.5f, -0.5f), glm::vec2(1.0, 1.0), glm::vec3(1.0, 0.0, 0.0)));
    addVertex(vertex(glm::vec3(+0.5f, +0.5f, -0.5f), glm::vec2(1.0, 1.0), glm::vec3(1.0, 0.0, 0.0)));
    addVertex(vertex(glm::vec3(+0.5f, +0.5f, +0.5f), glm::vec2(0.0, 1.0), glm::vec3(1.0, 0.0, 0.0)));
    addVertex(vertex(glm::vec3(+0.5f, -0.5f, +0.5f), glm::vec2(0.0, 0.0), glm::vec3(1.0, 0.0, 0.0)));

    addVertex(vertex(glm::vec3(+0.5f, -0.5f, -0.5f), glm::vec2(0.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
    addVertex(vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
    addVertex(vertex(glm::vec3(-0.5f, +0.5f, -0.5f), glm::vec2(1.0, 1.0), glm::vec3(0.0, 0.0, -1.0)));
    addVertex(vertex(glm::vec3(-0.5f, +0.5f, -0.5f), glm::vec2(1.0, 1.0), glm::vec3(0.0, 0.0, -1.0)));
    addVertex(vertex(glm::vec3(+0.5f, +0.5f, -0.5f), glm::vec2(0.0, 1.0), glm::vec3(0.0, 0.0, -1.0)));
    addVertex(vertex(glm::vec3(+0.5f, -0.5f, -0.5f), glm::vec2(0.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));

    addVertex(vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, -0.5f, +0.5f), glm::vec2(1.0, 0.0), glm::vec3(-1.0, 0.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, +0.5f, +0.5f), glm::vec2(1.0, 1.0), glm::vec3(-1.0, 0.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, +0.5f, +0.5f), glm::vec2(1.0, 1.0), glm::vec3(-1.0, 0.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, +0.5f, -0.5f), glm::vec2(0.0, 1.0), glm::vec3(-1.0, 0.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0)));

    addVertex(vertex(glm::vec3(+0.5f, -0.5f, +0.5f), glm::vec2(0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, -0.5f, +0.5f), glm::vec2(1.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(1.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(1.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
    addVertex(vertex(glm::vec3(+0.5f, -0.5f, -0.5f), glm::vec2(0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
    addVertex(vertex(glm::vec3(+0.5f, -0.5f, +0.5f), glm::vec2(0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));

    addVertex(vertex(glm::vec3(+0.5f, +0.5f, -0.5f), glm::vec2(0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, +0.5f, -0.5f), glm::vec2(1.0, 0.0), glm::vec3(0.0, 1.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, +0.5f, +0.5f), glm::vec2(1.0, 1.0), glm::vec3(0.0, 1.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, +0.5f, +0.5f), glm::vec2(1.0, 1.0), glm::vec3(0.0, 1.0, 0.0)));
    addVertex(vertex(glm::vec3(+0.5f, +0.5f, +0.5f), glm::vec2(0.0, 1.0), glm::vec3(0.0, 1.0, 0.0)));
    addVertex(vertex(glm::vec3(+0.5f, +0.5f, -0.5f), glm::vec2(0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)));

    return geometry::create(vertices, indices);
}
