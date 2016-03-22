#pragma once
#include <glm\glm.hpp>

struct ray
{
    glm::dvec3 origin;
    glm::dvec3 direction;

    ray(glm::dvec3 origin = glm::dvec3(), glm::dvec3 direction = glm::dvec3()) :
        origin(origin), direction(direction)
    {
    }
};