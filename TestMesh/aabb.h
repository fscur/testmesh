#ifndef _AABB_H_
#define _AABB_H_

#include <glm\glm.hpp>

struct aabb
{
public:
    glm::vec3 min;
    glm::vec3 max;
    float width;
    float height;
    float depth;
    float halfWidth;
    float halfHeight;
    float halfDepth;
    aabb();
    aabb(glm::vec3 min, glm::vec3 max);
    bool contains(glm::vec3 pos);
};

#endif // !1