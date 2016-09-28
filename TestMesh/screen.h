#pragma once
#include "types.h"
#include "window.h"
#include "font.h"
#include "geometry.h"
#include "shader.h"
#include "camera.h"

#include "framebuffer.h"
#include <glm\glm.hpp>

class screen :
    public window
{
private:
	glm::vec2 _resolution;
    glm::mat4 _projectionMatrix;
    glm::mat4 _viewMatrix;

    glm::mat4 _sceneModelMatrix;
	glm::mat4 _uiModelMatrix;

	int _blur;

	camera* _camera;
	shader* _shader;
	geometry* _cube;

private:
    void initGL();
    void initCamera();
	void createCube();
    void initShader();

public:
    screen(std::string name, uint witdh, uint height);
    ~screen(void);

    void onInit() override;
    void onClosing() override;
    void onUpdate() override;
    void onRender() override;
    void onTick() override;
};