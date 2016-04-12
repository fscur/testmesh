#pragma once
#include <glm\glm.hpp>

struct vertex
{
public:
    glm::vec3 position;
    glm::vec2 texCoord;
    glm::vec3 normal;
    glm::vec3 tangent;

public:
    vertex(
        glm::vec3 position = glm::vec3(0.0f), 
        glm::vec2 texCoord = glm::vec2(0.0f), 
        glm::vec3 normal = glm::vec3(0.0f), 
        glm::vec3 tangent = glm::vec3(0.0f)) :
        position(position),
        texCoord(texCoord),
        normal(normal),
        tangent(tangent)
    {
    }

    ~vertex() { };

    friend bool operator==(const vertex& a, const vertex& b)
    {
        return
            a.position == b.position &&
            a.texCoord == b.texCoord &&
            a.normal == b.normal &&
            a.tangent == b.tangent;
    }

    friend bool operator!=(const vertex& a, const vertex& b)
    {
        return !(a == b);
    }
};