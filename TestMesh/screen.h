#pragma once
#include "types.h"
#include "window.h"
#include "geometry.h"
#include "shader.h"
#include "camera.h"
#include "scene.h"
#include "program.h"

#include <glm\glm.hpp>

class screen :
    public window
{
private:
    bool _isMouseDown;
    bool _isRotating;
    float _smoothingFactor = 0.01f;

    glm::vec2 _mouseDownPos;
    glm::vec2 _lastMousePos; 
    glm::mat4 _projectionMatrix;
    glm::mat4 _viewMatrix;
    glm::mat4 _modelMatrix; 
    camera* _camera;
    scene* _scene;
    program* _program;
    float _aspect = (float)_width / (float)_height;

    mesh* _cube;
    material* _material;

private:
    void initDefaultResources();
    void initCamera();
    void initScene();
    void importGltf();

public:
    screen(std::string name, uint witdh, uint height);
    ~screen(void);

    void onInit() override;
    void onClosing() override;
    void onUpdate() override;
    void onRender() override;
    void onTick() override;

    virtual void onMouseDown(mouseEventArgs* eventArgs) override;
    virtual void onMouseUp(mouseEventArgs* eventArgs) override;
    virtual void onMouseMove(mouseEventArgs* eventArgs) override;
    virtual void onMouseWheel(mouseEventArgs* eventArgs) override;
};