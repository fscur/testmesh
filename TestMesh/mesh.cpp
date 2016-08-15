#include "mesh.h"

mesh::mesh(geometry* geometry, material* material) :
    _geometry(geometry),
    _material(material),
    _modelMatrix(glm::mat4())
{
}

mesh::~mesh()
{
    delete _material;
    delete _geometry;
}

void mesh::render() const
{
    _geometry->render();
}
