#include "mesh.h"

mesh::mesh(material* material, vertexArrayObject* vao) :
    _material(material),
    _vao(vao),
    _modelMatrix(glm::mat4())
{
}

mesh::~mesh()
{
    delete _material;
    delete _vao;
}

void mesh::bindSemantic(parameterSemantic semantic, uint32_t value)
{
    _material->bindSemantic(semantic, value);
}

void mesh::bindSemantic(parameterSemantic semantic, glm::vec3 value)
{
    _material->bindSemantic(semantic, value);
}

void mesh::bindSemantic(parameterSemantic semantic, glm::vec4 value)
{
    _material->bindSemantic(semantic, value);
}

void mesh::bindSemantic(parameterSemantic semantic, glm::mat3 value)
{
    _material->bindSemantic(semantic, value);
}

void mesh::bindSemantic(parameterSemantic semantic, glm::mat4 value)
{
    _material->bindSemantic(semantic, value);
}

void mesh::render() const
{
    _material->bind();
    _vao->render();
    _material->unbind();
}
