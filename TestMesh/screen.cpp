#include "screen.h"
#include "application.h"
#include "octree.h"
#include "aabb.h"
#include <iostream> 
#include <glm\gtc\matrix_transform.hpp>
#include <SDL\SDL.h>
#include <SDL\SDL_image.h>

screen::screen(std::string name, uint width, uint height) :
	window(name, width, height)
{
	SDL_Init(SDL_INIT_VIDEO);
}

void screen::initGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	_defaultFramebuffer = new framebuffer(true);
}

void screen::createScreenQuad()
{
	auto vertices = std::vector<vertex>
	{
		vertex(glm::vec3(0.0f, 0.0f, +0.0f), glm::vec2(0.0f, 1.0f)),
		vertex(glm::vec3(1.0f, 0.0f, +0.0f), glm::vec2(1.0f, 1.0f)),
		vertex(glm::vec3(1.0f, 1.0f, +0.0f), glm::vec2(1.0f, 0.0f)),
		vertex(glm::vec3(0.0f, 1.0f, +0.0f), glm::vec2(0.0f, 0.0f))
	};

	auto indices = std::vector<uint>{ 0, 1, 2, 2, 3, 0 };

	_screenQuad = geometry::create(vertices, indices);
	_screenModelMatrix = glm::mat4(1.0f);
}

void screen::createBlurQuad()
{
	auto vertices = std::vector<vertex>
	{
		vertex(glm::vec3(-1.0f, -1.0f, +0.0f), glm::vec2(0.0f, 0.0f)),
		vertex(glm::vec3(+1.0f, -1.0f, +0.0f), glm::vec2(1.0f, 0.0f)),
		vertex(glm::vec3(+1.0f, +1.0f, +0.0f), glm::vec2(1.0f, 1.0f)),
		vertex(glm::vec3(-1.0f, +1.0f, +0.0f), glm::vec2(0.0f, 1.0f))
	};

	auto indices = std::vector<uint>{ 0, 1, 2, 2, 3, 0 };

	_blurQuad = geometry::create(vertices, indices);
}

void screen::initScreenFramebuffer()
{
	auto tex = new texture(
		_width,
		_height,
		GL_TEXTURE_2D,
		GL_RGBA8,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		nullptr,
		GL_CLAMP_TO_EDGE,
		GL_LINEAR,
		GL_LINEAR,
		false);

	tex->generate();

	_screenRT = new renderTarget(
		GL_COLOR_ATTACHMENT0,
		_width,
		_height,
		tex
	);

	_screenFramebuffer = new framebuffer();
	_screenFramebuffer->add(_screenRT);
}

void screen::initScreenShader()
{
	_screenShader = new shader("SHADER", "shader.vert", "shader.frag");

	_screenShader->addAttribute("inPosition");
	_screenShader->addAttribute("inTexCoord");
	_screenShader->addAttribute("inNormal");
	_screenShader->addAttribute("inModelMatrix");

	_screenShader->init();
	_screenShader->addUniform("mvp", 0);
	_screenShader->addUniform("inputTexture", 1);

	_screenTexture = texture::load("0.jpg");
}

void screen::createUiQuad()
{
	auto vertices = std::vector<vertex>
	{
		vertex(glm::vec3(0.0f, 0.0f, +0.0f), glm::vec2(0.0f, 1.0f)),
		vertex(glm::vec3(1.0f, 0.0f, +0.0f), glm::vec2(1.0f, 1.0f)),
		vertex(glm::vec3(1.0f, 1.0f, +0.0f), glm::vec2(1.0f, 0.0f)),
		vertex(glm::vec3(0.0f, 1.0f, +0.0f), glm::vec2(0.0f, 0.0f))
	};

	auto indices = std::vector<uint>{ 0, 1, 2, 2, 3, 0 };

	_uiQuad = geometry::create(vertices, indices);
}

void screen::initUiFramebuffer()
{
	auto tex = new texture(
		_width,
		_height,
		GL_TEXTURE_2D,
		GL_RGBA8,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		nullptr,
		GL_CLAMP_TO_EDGE,
		GL_LINEAR,
		GL_LINEAR,
		false);

	tex->generate();

	_uiRT = new renderTarget(
		GL_COLOR_ATTACHMENT0,
		_width,
		_height,
		tex
	);

	_uiFramebuffer = new framebuffer();
	_uiFramebuffer->add(_uiRT);
}

void screen::initUiShader()
{
	_uiShader = new shader("Ui", "glass.vert", "glass.frag");
	_uiShader->addAttribute("inPosition");
	_uiShader->addAttribute("inTexCoord");
	_uiShader->addAttribute("inNormal");
	_uiShader->addAttribute("inModelMatrix");
	_uiShader->init();
	_uiShader->addUniform("mvp", 0);
	_uiShader->addUniform("background", 1);
}

void screen::initBlurHShader()
{
	_blurHShader = new shader("BlurH", "blurH.vert", "blurH.frag");
	_blurHShader->addAttribute("inPosition");
	_blurHShader->addAttribute("inTexCoord");
	_blurHShader->addAttribute("inNormal");
	_blurHShader->addAttribute("inModelMatrix");
	_blurHShader->init();
	_blurHShader->addUniform("inputTexture", 0);
}

void screen::initBlurHFramebuffer()
{
	auto tex = new texture(
		_width,
		_height,
		GL_TEXTURE_2D,
		GL_RGBA8,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		nullptr,
		GL_CLAMP_TO_EDGE,
		GL_LINEAR,
		GL_LINEAR,
		false);

	tex->generate();

	_blurHRT = new renderTarget(
		GL_COLOR_ATTACHMENT0,
		_width,
		_height,
		tex
	);

	_blurHFramebuffer = new framebuffer();
	_blurHFramebuffer->add(_blurHRT);
}

void screen::initBlurVShader()
{
	_blurVShader = new shader("BlurV", "blurV.vert", "blurV.frag");
	_blurVShader->addAttribute("inPosition");
	_blurVShader->addAttribute("inTexCoord");
	_blurVShader->addAttribute("inNormal");
	_blurVShader->addAttribute("inModelMatrix");
	_blurVShader->init();
	_blurVShader->addUniform("inputTexture", 0);
}

void screen::initBlurVFramebuffer()
{
	auto tex = new texture(
		_width,
		_height,
		GL_TEXTURE_2D,
		GL_RGBA8,
		GL_RGBA,
		GL_UNSIGNED_BYTE,
		nullptr,
		GL_CLAMP_TO_EDGE,
		GL_LINEAR,
		GL_LINEAR,
		false);

	tex->generate();

	_blurVRT = new renderTarget(
		GL_COLOR_ATTACHMENT0,
		_width,
		_height,
		tex
	);

	_blurVFramebuffer = new framebuffer();
	_blurVFramebuffer->add(_blurVRT);
}

void screen::initCamera()
{
	_camera = new camera();
	_camera->setPosition(glm::vec3(0.0f, 3.0f, 4.5f));
	_camera->setTarget(glm::vec3(0.0f));
	_projectionMatrix = glm::perspective<float>(glm::half_pi<float>(), 1024.0f / 768.0f, 0.1f, 100.0f);
	_viewMatrix = glm::lookAt<float>(_camera->getPosition(), _camera->getTarget(), _camera->getUp());
}

screen::~screen()
{
	delete(_screenFramebuffer);
	delete(_uiFramebuffer);
	delete(_screenRT);
	delete(_uiRT);
}

void screen::onInit()
{
	initCamera();
	initGL();

	createScreenQuad();
	initScreenFramebuffer();
	initScreenShader();

	createBlurQuad();
	initBlurHFramebuffer();
	initBlurHShader();
	initBlurVFramebuffer();
	initBlurVShader();

	createUiQuad();
	initUiFramebuffer();
	initUiShader();
}

float t = 0.0f;

void screen::onUpdate()
{
	t += 0.01;

	_uiModelMatrix = glm::mat4(
		2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		glm::cos(t) * 4.0f, 0.0f, 0.5f, 1.0f);
}

void screen::onRender()
{
//	_screenFramebuffer->bind(GL_FRAMEBUFFER);
//	glClearColor(0.0, 0.0, 0.0, 0.0);
//	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	_screenShader->bind();
//	//_screenShader->getUniform(0).set(_projectionMatrix * _viewMatrix * _screenModelMatrix);
//	_screenQuad->render();
//	_screenShader->unbind();
//
//	_screenFramebuffer->unbind(GL_FRAMEBUFFER);
//	_screenFramebuffer->blitToDefault(_screenRT);

	_screenFramebuffer->bind(GL_FRAMEBUFFER);

	glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _screenShader->bind();
	_screenShader->getUniform(0).set(_projectionMatrix * _viewMatrix * _screenModelMatrix);
	_screenShader->getUniform(1).set(_screenTexture->id, 0);
    _screenQuad->render();
    _screenShader->unbind();

	_screenFramebuffer->unbind(GL_FRAMEBUFFER);
	_screenFramebuffer->blitToDefault(_screenRT);

	_blurHFramebuffer->bindForDrawing();
	_blurHShader->bind();
	_blurHShader->getUniform(0).set(_screenRT->tex->id, 0);
	_blurQuad->render();
	_blurHShader->unbind();
	_blurHFramebuffer->unbind(GL_FRAMEBUFFER);

	_blurVFramebuffer->bindForDrawing();
	_blurVShader->bind();
	_blurVShader->getUniform(0).set(_blurHRT->tex->id, 0);
	_blurQuad->render();
	_blurVShader->unbind();
	_blurVFramebuffer->unbind(GL_FRAMEBUFFER);
	//_blurHFramebuffer->blitToDefault(_blurHRT, 0, 0, 800, 450);


	////_uiFramebuffer->bindForDrawing();
	_defaultFramebuffer->bindForDrawing();

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glBlendColor(1, 1, 1, 1);

	_uiShader->bind();
	_uiShader->getUniform(0).set(_projectionMatrix * _viewMatrix * _uiModelMatrix);
	_uiShader->getUniform(1).set(_blurVRT->tex->id, 0);
	_blurQuad->render();
	_uiShader->unbind();

	glBlendColor(0, 0, 0, 0);
	glDisable(GL_BLEND);
	glEnable(GL_DEPTH_TEST);

	glEnable(GL_CULL_FACE);

	_uiFramebuffer->unbind(GL_FRAMEBUFFER);
}

void screen::onTick()
{
}

void screen::onClosing()
{
}