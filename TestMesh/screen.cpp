#include "screen.h"
#include "application.h"

#include "material.h"
#include "programBuilder.h"
#include "glDebugger.h"
#include "importer.h"

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

void screen::initDefaultResources()
{
    material::default = importer::importDefaultMaterial(R"(C:\Users\Patrick\Desktop\default_material.gltf)");
}

void screen::initCamera()
{
    _camera = new camera();
    _camera->setPosition(glm::vec3(0.0f, 3.0f, 5.0f));
    _camera->setTarget(glm::vec3(0.0f));
    _projectionMatrix = glm::perspective<float>(glm::half_pi<float>(), _aspect, 0.1f, 100.0f);
    _viewMatrix = glm::lookAt<float>(_camera->getPosition(), _camera->getTarget(), _camera->getUp());

    _modelMatrix = glm::mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

void screen::initScene()
{
    _material = material::default;
    _cube = new mesh(geometry::createCube(), nullptr);

    _scene = new scene();
    //_scene->add(cube);

    _program = programBuilder::buildProgram("shader.vert", "shader.frag");
}

void screen::importGltf()
{
    importer::importScene(R"(C:\Users\Patrick\Workspaces\glTF\sampleModels\Box\glTF\Box.gltf)");
}

void screen::onInit()
{
    glDebugger::enable();
    initDefaultResources();
    initScene();
    initCamera();
}

void screen::onUpdate()
{
    _viewMatrix = glm::lookAt<float>(_camera->getPosition(), _camera->getTarget(), _camera->getUp());
}

void screen::onRender()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _material->bindSemantic(parameterSemantic::MODELVIEW, _viewMatrix *  _modelMatrix);
    _material->bindSemantic(parameterSemantic::PROJECTION, _projectionMatrix);

    _material->bind();

    _cube->render();

    _material->unbind();
}

void screen::onTick()
{
    std::cout << "Fps: " << application::framesPerSecond << std::endl;
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
    delete _camera;
    delete _scene;
}