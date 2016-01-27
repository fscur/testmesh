#include "camera.h"
#include "mathUtils.h"


camera::camera()
{
    _right = glm::vec3(1.0f, 0.0f, 0.0f);
    _up = glm::vec3(0.0f, 1.0f, 0.0f);
    _direction = glm::vec3(0.0f, 0.0f, 1.0f);
}

camera::~camera(void)
{
}

void camera::setPosition(glm::vec3 position)
{
    _position = position;
    _direction = _target - _position;
    update();
}

void camera::setTarget(glm::vec3 target)
{
    _target = target;
    _direction = _target - _position;
    update();
}

void camera::update()
{
    _direction = normalize(_direction);

    _right = cross(_direction, glm::vec3(0.0f, 1.0f, 0.0f));
    _up = cross(_right, _direction);

    _right = normalize(_right);
    _up = normalize(_up);
    _direction = normalize(_direction);
}


void camera::translate(glm::vec3 translation)
{
    _position += translation;
}

void camera::orbit(glm::vec3 origin, glm::vec3 axis, float angle)
{
    _position = mathUtils::rotateAboutAxis(_position, origin, axis, angle);
	_direction = _target - _position;
	update();
}

void camera::dolly(float amount)
{
    glm::vec3 offset = _direction * amount;
    translate(offset);
}

void camera::strafe(float amount)
{
    glm::vec3 offset = _right * amount;
    translate(offset);
}