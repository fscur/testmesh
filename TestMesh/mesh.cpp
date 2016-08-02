#include "mesh.h"

mesh::mesh(geometry* geometry, shadingTechnique* technique) :
    _geometry(geometry),
    _technique(technique),
    _modelMatrix(glm::mat4())
{
}

mesh::~mesh()
{
}

void mesh::render() const
{
    _geometry->render();
}
