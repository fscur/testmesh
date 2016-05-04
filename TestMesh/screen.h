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

	framebuffer* _sceneFramebuffer;
	framebuffer* _screenFramebuffer;
	framebuffer* _defaultFramebuffer;
	framebuffer* _uiFramebuffer;
	framebuffer* _blurHFramebuffer;
	framebuffer* _blurVFramebuffer;

	renderTarget* _sceneRT;
	renderTarget* _screenRT;
	renderTarget* _uiRT;
	renderTarget* _blurHRT;
	renderTarget* _blurVRT;

	shader* _sceneShader;
    shader* _screenShader;
	shader* _uiShader;
	shader* _blurHShader;
	shader* _blurVShader;

	geometry* _sceneQuad;
    geometry* _screenQuad;
	geometry* _uiQuad;
	geometry* _blurQuad;

	texture* _sceneTexture;

private:
    void initGL();
    void initCamera();
    
	void createSceneQuad();
    void initSceneShader();
	void initSceneFramebuffer();

	void createScreenQuad();
	void initScreenShader();
	void initScreenFramebuffer();

	void createUiQuad();
	void initUiShader();
	void initUiFramebuffer();

	void createBlurQuad();
	void initBlurHShader();
	void initBlurVShader();
	void initBlurHFramebuffer();
	void initBlurVFramebuffer();

	void renderBlur(renderTarget* renderTarget);
	void renderUi();

public:
    screen(std::string name, uint witdh, uint height);
    ~screen(void);

    void onInit() override;
    void onClosing() override;
    void onUpdate() override;
    void onRender() override;
    void onTick() override;
};