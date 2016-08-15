#pragma once
#include "geometry.h"
#include "material.h"

class mesh
{
private:
    geometry* _geometry;
    material* _material;
    glm::mat4 _modelMatrix;

public:
    mesh(geometry* geometry, material* material);
    ~mesh();

    void render() const;
};

