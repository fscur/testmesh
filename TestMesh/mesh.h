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

    void bindSemantic(parameterSemantic semantic, uint32_t value);
    void bindSemantic(parameterSemantic semantic, glm::vec3 value);
    void bindSemantic(parameterSemantic semantic, glm::vec4 value);
    void bindSemantic(parameterSemantic semantic, glm::mat3 value);
    void bindSemantic(parameterSemantic semantic, glm::mat4 value);

    void render() const;
};
