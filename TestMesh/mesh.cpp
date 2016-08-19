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

void mesh::render() const
{
    _vao->render();
}
