#include "vertex.h"

int vertex::SIZE = 8;

vertex::vertex(glm::vec3 position, glm::vec2 texCoord, glm::vec3 normal) :
    _position(position), _texCoord(texCoord), _normal(normal)
{
}

vertex::~vertex()
{}