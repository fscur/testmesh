#pragma once
#include "types.h"
#include "window.h"
#include "font.h"
#include "geometry.h"
#include "shader.h"
#include "camera.h"

#include <glm\glm.hpp>

class screen :
    public window
{
private:
    bool _isMouseDown;
    glm::vec2 _mouseDownPos;
    glm::vec2 _lastMousePos; 
    glm::mat4 _projectionMatrix;
    glm::mat4 _viewMatrix;
    glm::mat4 _modelMatrix; 
    shader* _shader;
    camera* _camera;
    geometry* _geometry;
    std::vector<glm::mat4> _modelMatrices;
    font* _font;
    std::map<uint, GLuint> _glyphTextures;
    float _aspect = (float)_width / (float)_height;

private:
    void createQuad();
    void initShader();
    void initCamera();
    void initGL();

public:
    screen(std::string name, uint witdh, uint height);
    ~screen(void);

    void onInit() override;
    void onClosing() override;
    void onUpdate() override;
    void onRender() override;
    void onTick() override;
};