#pragma once
#include "geometry.h"
#include "material.h"
#include "vertexArrayObject.h"

class mesh
{
public:
    material* _material;
    glm::mat4 _modelMatrix;
    vertexArrayObject* _vao;

public:
    mesh(material* material, vertexArrayObject* vao);
    ~mesh();

    void render() const;
};

