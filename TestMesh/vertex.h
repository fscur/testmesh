#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "TestMesh.h"
#include <glm/glm.hpp>

struct vertex
{
public:
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec2 texCoord = glm::vec2(0.0f);
    glm::vec3 normal = glm::vec3(0.0f);

public:
    vertex(glm::vec3 p = glm::vec3(0.0f), glm::vec2 t = glm::vec2(0.0f), glm::vec3 n = glm::vec3(0.0f)) :
        position(p), texCoord(t), normal(n)
    {
    }

    friend bool operator==(const vertex& a, const vertex& b)
    {
        return
            a.position == b.position &&
            a.texCoord == b.texCoord &&
            a.normal == b.normal;
    }

    friend bool operator!=(const vertex& a, const vertex& b)
    {
        return !(a == b);
    }
};

#endif