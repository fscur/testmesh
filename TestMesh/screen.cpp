#include "screen.h"
#include "application.h"

#include "material.h"
#include "programBuilder.h"
#include "glDebugger.h"
#include "importer.h"
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
}

void screen::initScene()
{
    auto cube = new mesh(geometry::createCube(), material::default);

    _scene = new scene((float)_width, (float)_height);
    _scene->add(cube);

    _camera = _scene->getCamera();
}

void screen::importGltf()
{
    _scene = importer::importScene(R"(C:\Users\Patrick\Workspaces\glTF\sampleModels\Box\glTF\Box.gltf)");
    _camera = _scene->getCamera();
}

void screen::onInit()
{
    glDebugger::enable();
    initDefaultResources();
    //initScene();
    importGltf();
    initCamera();
}

void screen::onUpdate()
{
}

void screen::onRender()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _scene->render();
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