#ifndef _VERTEX_H_
#define _VERTEX_H_

#include "TestMesh.h"
#include <glm/glm.hpp>

class vertex
{
private:
    glm::vec3 _position;
    glm::vec2 _texCoord;
    glm::vec3 _normal;
public:
    static int SIZE;
public:
    vertex(glm::vec3 position = glm::vec3(0.0f), glm::vec2 texCoord = glm::vec2(0.0f), glm::vec3 normal = glm::vec3(0.0f));
    ~vertex();

    glm::vec3 GetPosition() const { return _position; }
    void SetPosition(glm::vec3 position) { _position = position; }
    glm::vec2 GetTexCoord() const { return _texCoord; }
    void SetTexCoord(glm::vec2 texCoord) { _texCoord = texCoord; }
    glm::vec3 GetNormal() const { return _normal; }
    void SetNormal(glm::vec3 normal) { _normal = normal; }

    friend bool operator==(const vertex& a, const vertex& b)
    {
        return
            a._position == b._position &&
            a._texCoord == b._texCoord &&
            a._normal == b._normal;
    }

    friend bool operator!=(const vertex& a, const vertex& b)
    {
        return !(a == b);
    }
};

#endif