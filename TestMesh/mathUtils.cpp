#include "mathUtils.h"

float mathUtils::distance(glm::vec3 v0, glm::vec3 v1)
{
    double dx = v1.x - v0.x;
    double dy = v1.y - v0.y;
    double dz = v1.z - v0.z;

    double x2 = pow(dx, 2); 
    double y2 = pow(dy, 2);
    double z2 = pow(dz, 2);

    return (float)glm::sqrt(x2 + y2 + z2);
}

std::vector<glm::vec3> mathUtils::rotateAboutAxis(std::vector<glm::vec3>* points, glm::vec3 origin, glm::vec3 axis, float angle)
{
    axis = normalize(axis);

    glm::mat4 transform = getRotationMatrixAboutAnArbitraryAxis(origin, axis, angle);

    auto transformedPoints = std::vector<glm::vec3>();

    for (auto point : *points)
    {
        auto p4 = glm::vec4(point.x, point.y, point.z, 1.0f);
        auto transformed = transform * p4;
        glm::vec3 p3 = glm::vec3(transformed.x, transformed.y, transformed.z);
        transformedPoints.push_back(p3);
    }

    return transformedPoints;
}

std::vector<glm::vec3> mathUtils::rotateAboutAxis(std::vector<glm::vec3>* points, glm::vec3 axis, float angle)
{
    return rotateAboutAxis(points, glm::vec3(0.0f), axis, angle);
}

glm::vec3 mathUtils::rotateAboutAxis(glm::vec3 point, glm::vec3 origin, glm::vec3 axis, float angle)
{
    axis = normalize(axis);
    glm::mat4 transform = mathUtils::getRotationMatrixAboutAnArbitraryAxis(origin, axis, angle);
    auto p4 = glm::vec4(point.x, point.y, point.z, 1.0f);
    auto transformed = transform * p4;
    return glm::vec3(transformed.x, transformed.y, transformed.z);
}

glm::vec3 mathUtils::rotateAboutAxis(glm::vec3 point, glm::vec3 axis, float angle)
{
    return rotateAboutAxis(point, glm::vec3(0.0f), axis, angle);
}

glm::mat4 mathUtils::getRotationMatrixAboutAnArbitraryAxis(glm::vec3 origin, glm::vec3 axis, float angle)
{
    double y2 = pow(axis.y, 2);
    double z2 = pow(axis.z, 2);
    double l = glm::sqrt(y2 + z2);

    double a = axis.x;
    double b = l != 0 ? axis.y / l : 1;
    double c = l != 0 ? axis.z / l : 0;
        
    double cossine = cos(angle);
    double sine = sin(angle);

    //translate by -origin to make axis pass through the origin
    glm::mat4 t0 = glm::mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        -origin.x, -origin.y, -origin.z, 1);

    //rotate about X axis to align rotation axis with XY plane
    glm::mat4 t1 = glm::mat4(
        1, 0, 0, 0,
        0, b, -c, 0,
        0, c, b, 0,
        0, 0, 0, 1);

    //rotate about Z axis to align rotation axis with X Axis
    glm::mat4 t2 = glm::mat4(
        a, -l, 0, 0,
        l, a, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);

    //rotate about X with the desired rotation angle
    glm::mat4 t3 = glm::mat4(
        1, 0, 0, 0,
        0, cossine, sine, 0,
        0, -sine, cossine, 0,
        0, 0, 0, 1);

    //undo t2 rotation
    glm::mat4 t4 = glm::mat4(
        a, l, 0, 0,
        -l, a, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1);

    //undo t1 rotation
    glm::mat4 t5 = glm::mat4(
        1, 0, 0, 0,
        0, b, c, 0,
        0, -c, b, 0,
        0, 0, 0, 1);

    //undo t0 translation
    glm::mat4 t6 = glm::mat4(
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        origin.x, origin.y, origin.z, 1);

    glm::mat4 transform;

    if (l == 0)
        transform = t6 * t3 * t0;
    else
        transform = t6 * t5 * t4 * t3 * t2 * t1 * t0;

    return transform;
}