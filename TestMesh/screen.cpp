#include "screen.h"
#include "application.h"

#include "glDebugger.h"

#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

#include <iostream>

screen::screen(std::string name, uint width, uint height) :
    window(name, width, height)
{
}

screen::~screen()
{
}

void screen::initCamera()
{
    _camera = new camera();
    _camera->setPosition(glm::vec3(0.0f, 3.0f, 5.0f));
    _camera->setTarget(glm::vec3(0.0f));
    _projectionMatrix = glm::perspective<float>(glm::half_pi<float>(), 1024.0f / 768.0f, 0.1f, 100.0f);
    _viewMatrix = glm::lookAt<float>(_camera->getPosition(), _camera->getTarget(), _camera->getUp());

    _modelMatrix = glm::mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

void screen::initCube()
{
    _cube = geometry::createCube();
    _modelMatrix = glm::mat4();
}

void screen::initShader()
{
    _shader = new shader("shader", "shader.vert", "shader.frag");
    _shader->addAttribute("inPosition");

    _shader->init();
    _shader->addUniform("model", 0);
    _shader->addUniform("view", 1);
    _shader->addUniform("projection", 2);
}

void screen::onInit()
{
    glDebugger::enable();
    initCamera();
    initCube();
    initShader();
}

void screen::onUpdate()
{
    _viewMatrix = glm::lookAt<float>(_camera->getPosition(), _camera->getTarget(), _camera->getUp());
}

void screen::onRender()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _shader->bind();

    _shader->getUniform(0).set(_modelMatrix);
    _shader->getUniform(1).set(_viewMatrix);
    _shader->getUniform(2).set(_projectionMatrix);

    _cube->render();

    _shader->unbind();
}

void screen::onTick()
{
}

void screen::onMouseDown(mouseEventArgs* eventArgs)
{
    _isMouseDown = true;
    _mouseDownPos = glm::vec2(eventArgs->x, eventArgs->y);
}

void screen::onMouseMove(mouseEventArgs* eventArgs)
{
    glm::vec2 mousePos = glm::vec2(eventArgs->x, eventArgs->y);

    if (!_isMouseDown)
    {
        _lastMousePos = mousePos;
        return;
    }

    if (_isMouseDown && length(_mouseDownPos - _lastMousePos) > 5)
        _isRotating = true;

    if (!_isRotating)
    {
        _lastMousePos = mousePos;
        return;
    }

    float dx = (float)(_lastMousePos.x - mousePos.x);
    float dy = (float)(_lastMousePos.y - mousePos.y);

    dx *= _smoothingFactor;
    dy *= _smoothingFactor;

    _camera->orbit(_camera->getTarget(), _camera->getUp(), dx);
    _camera->orbit(_camera->getTarget(), _camera->getRight(), dy);

    _lastMousePos = mousePos;
}

void screen::onMouseUp(mouseEventArgs* eventArgs)
{
    _isRotating = false;
    _isMouseDown = false;
}

void screen::onMouseWheel(mouseEventArgs * eventArgs)
{
    _camera->dolly(eventArgs->wheelDelta * _smoothingFactor);
}

void screen::onClosing()
{
    delete _shader;
    delete _cube;
    delete _camera;
}