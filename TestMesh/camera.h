#ifndef _CAMERA_H_
#define _CAMERA_H_

#include "TestMesh.h"
#include <glm/glm.hpp>

class camera
{

private:
    glm::vec3 _position;
    glm::vec3 _right;
    glm::vec3 _up;
    glm::vec3 _direction;
    glm::vec3 _target;

public:
    camera();

    ~camera(void);

    glm::vec3 getPosition() const { return _position; }
    void setPosition(glm::vec3 position);

    glm::vec3 getTarget() const { return _target; }
    void setTarget(glm::vec3 target);

    glm::vec3 getDirection() const { return _direction; }
    glm::vec3 getRight() const { return _right; }
    glm::vec3 getUp() const { return _up; }

    bool init();
    void update();
    void translate(glm::vec3 translation);
    void orbit(glm::vec3 origin, glm::vec3 axis, float angle);
    void dolly(float amount);
    void strafe(float amount);
};
#endif

