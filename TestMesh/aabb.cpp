#include "aabb.h"

aabb::aabb()
{
}

aabb::aabb(glm::vec3 min, glm::vec3 max)
{
    this->min = min;
    this->max = max;
    width = max.x - min.x;
    height = max.y - min.y;
    depth = max.z - min.z;
    halfWidth = width * 0.5f;
    halfHeight = height * 0.5f;
    halfDepth = depth * 0.5f;
}

bool aabb::contains(glm::vec3 pos)
{
    return
        min.x < pos.x && pos.x < max.x &&
        min.y < pos.y && pos.y < max.y &&
        min.z < pos.z && pos.z < max.z;
}
