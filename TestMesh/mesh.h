#pragma once
#include "geometry.h"
#include "shadingTechnique.h"

class mesh
{
private:
    geometry* _geometry;
    shadingTechnique* _technique;
    glm::mat4 _modelMatrix;

public:
    mesh(geometry* geometry, shadingTechnique* technique);
    ~mesh();

    void render() const;
};

