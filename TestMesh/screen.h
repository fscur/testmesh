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
    glm::mat4 _projectionMatrix;
    glm::mat4 _viewMatrix;

    glm::mat4 _screenModelMatrix;
	glm::mat4 _uiModelMatrix;

	camera* _camera;
    
	framebuffer* _defaultFramebuffer;
	framebuffer* _screenFramebuffer;
	framebuffer* _uiFramebuffer;
	framebuffer* _blurHFramebuffer;
	framebuffer* _blurVFramebuffer;

	renderTarget* _screenRT;
	renderTarget* _uiRT;
	renderTarget* _blurHRT;
	renderTarget* _blurVRT;

    shader* _screenShader;
	shader* _uiShader;
	shader* _blurHShader;
	shader* _blurVShader;

    geometry* _screenQuad;
	geometry* _uiQuad;
	geometry* _blurQuad;

	texture* _screenTexture;

private:
    void initGL();
    void initCamera();
    
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

public:
    screen(std::string name, uint witdh, uint height);
    ~screen(void);

    void onInit() override;
    void onClosing() override;
    void onUpdate() override;
    void onRender() override;
    void onTick() override;
};