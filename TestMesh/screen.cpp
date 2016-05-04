#include "screen.h"
#include "application.h"
#include "octree.h"
#include "aabb.h"
#include <iostream> 
#include <glm\gtc\matrix_transform.hpp>
#include <SDL\SDL.h>
#include <SDL\SDL_image.h>

float t = 0.0f;

screen::screen(std::string name, uint width, uint height) :
	window(name, width, height),
	_resolution(glm::vec2(_width, _height))
{
	SDL_Init(SDL_INIT_VIDEO);
	IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF);
	_blur = 2;
}

void screen::initGL()
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);

	_defaultFramebuffer = new framebuffer(true);
}

void screen::createSceneQuad()
{
	auto vertices = std::vector<vertex>
	{
		vertex(glm::vec3(0.0f, 0.0f, +0.0f), glm::vec2(0.0f, 1.0f)),
		vertex(glm::vec3(1.0f, 0.0f, +0.0f), glm::vec2(1.0f, 1.0f)),
		vertex(glm::vec3(1.0f, 1.0f, +0.0f), glm::vec2(1.0f, 0.0f)),
		vertex(glm::vec3(0.0f, 1.0f, +0.0f), glm::vec2(0.0f, 0.0f))
	};

	auto indices = std::vector<uint>{ 0, 1, 2, 2, 3, 0 };

	_sceneQuad = geometry::create(vertices, indices);
	_sceneModelMatrix = glm::mat4(
		3.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 3.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 3.0f, 0.0f,
		0.0f, 0.0f, 0.0f, 1.0f);
}

void screen::initSceneFramebuffer()
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
		GL_LINEAR_MIPMAP_LINEAR,
		GL_LINEAR,
		true);

	tex->generate();

	_sceneRT = new renderTarget(
		GL_COLOR_ATTACHMENT0,
		_width,
		_height,
		tex
	);

	_sceneFramebuffer = new framebuffer();
	_sceneFramebuffer->add(_sceneRT);
}

void screen::initSceneShader()
{
	_sceneShader = new shader("SHADER", "shader.vert", "shader.frag");
	_sceneShader->addAttribute("inPosition");
	_sceneShader->addAttribute("inTexCoord");
	_sceneShader->addAttribute("inNormal");
	_sceneShader->addAttribute("inModelMatrix");
	_sceneShader->init();
	_sceneShader->addUniform("mvp", 0);
	_sceneShader->addUniform("inputTexture", 1);
	_sceneTexture = texture::load("0.jpg");
}

void screen::createScreenQuad()
{
	auto vertices = std::vector<vertex>
	{
		vertex(glm::vec3(-1.0f, -1.0f, +0.0f), glm::vec2(0.0f, 0.0f)),
		vertex(glm::vec3(+1.0f, -1.0f, +0.0f), glm::vec2(1.0f, 0.0f)),
		vertex(glm::vec3(+1.0f, +1.0f, +0.0f), glm::vec2(1.0f, 1.0f)),
		vertex(glm::vec3(-1.0f, +1.0f, +0.0f), glm::vec2(0.0f, 1.0f))
	};

	auto indices = std::vector<uint>{ 0, 1, 2, 2, 3, 0 };

	_screenQuad = geometry::create(vertices, indices);
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
		GL_LINEAR_MIPMAP_LINEAR,
		GL_LINEAR,
		true);

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
	_screenShader = new shader("SHADER", "screen.vert", "screen.frag");

	_screenShader->addAttribute("inPosition");
	_screenShader->addAttribute("inTexCoord");

	_screenShader->init();
	_screenShader->addUniform("inputTexture", 1);
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

void screen::initBlurHShader()
{
	_blurHShader = new shader("BlurH", "blurH.vert", "blurH.frag");
	_blurHShader->addAttribute("inPosition");
	_blurHShader->addAttribute("inTexCoord");
	_blurHShader->addAttribute("inNormal");
	_blurHShader->addAttribute("inModelMatrix");
	_blurHShader->init();
	_blurHShader->addUniform("inputTexture", 0);
	_blurHShader->addUniform("resolution", 1);
	_blurHShader->addUniform("level", 2);
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
		GL_LINEAR_MIPMAP_LINEAR,
		GL_LINEAR,
		true);

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
	_blurVShader->addUniform("resolution", 1);
	_blurVShader->addUniform("level", 2);
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
		GL_LINEAR_MIPMAP_LINEAR,
		GL_LINEAR,
		true);

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

void screen::initCamera()
{
	_camera = new camera();
	_camera->setPosition(glm::vec3(3.0f, 2.0f, 4.5f));
	_camera->setTarget(glm::vec3(0.0f));
	_projectionMatrix = glm::perspective<float>(glm::half_pi<float>(), 1600.0f / 900.0f, 0.1f, 100.0f);
	_viewMatrix = glm::lookAt<float>(_camera->getPosition(), _camera->getTarget(), _camera->getUp());
}

screen::~screen()
{
	delete(_sceneFramebuffer);
	delete(_uiFramebuffer);
	delete(_sceneRT);
	delete(_uiRT);
}

void screen::onInit()
{
	initCamera();
	initGL();

	createSceneQuad();
	initSceneFramebuffer();
	initSceneShader();

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

void screen::onUpdate()
{
	t += 0.01;

	_uiModelMatrix = glm::mat4(
		2.0f, 0.0f, 0.0f, 0.0f,
		0.0f, 2.0f, 0.0f, 0.0f,
		0.0f, 0.0f, 1.0f, 0.0f,
		glm::cos(t) * 4.0f, 0.0f, 0.5f, 1.0f);
}

void screen::renderBlur(renderTarget* renderTarget)
{
	_blurHFramebuffer->bindForDrawing();
	_blurHShader->bind();
	_blurHShader->getUniform(0).set(renderTarget->tex->id, 0);
	_blurHShader->getUniform(1).set(_resolution);
	_blurHShader->getUniform(2).set(_blur);
	_blurQuad->render();
	_blurHShader->unbind();
	_blurHFramebuffer->unbind(GL_FRAMEBUFFER);

	_blurVFramebuffer->bindForDrawing();
	_blurVShader->bind();
	_blurVShader->getUniform(0).set(_blurHRT->tex->id, 0);
	_blurVShader->getUniform(1).set(_resolution);
	_blurVShader->getUniform(2).set(_blur);
	_blurQuad->render();
	_blurVShader->unbind();
	_blurVFramebuffer->unbind(GL_FRAMEBUFFER);

	/*for (size_t i = 0; i < 10; i++)
	{
		_blurHFramebuffer->bindForDrawing();
		_blurHShader->bind();
		_blurHShader->getUniform(0).set(_blurVRT->tex->id, 0);
		_blurHShader->getUniform(1).set(_resolution);
		_blurQuad->render();
		_blurHShader->unbind();
		_blurHFramebuffer->unbind(GL_FRAMEBUFFER);

		_blurVFramebuffer->bindForDrawing();
		_blurVShader->bind();
		_blurVShader->getUniform(0).set(_blurHRT->tex->id, 0);
		_blurVShader->getUniform(1).set(_resolution);
		_blurQuad->render();
		_blurVShader->unbind();
		_blurVFramebuffer->unbind(GL_FRAMEBUFFER);
	}*/
}

void screen::renderUi()
{
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

	_sceneFramebuffer->bind(GL_FRAMEBUFFER);
	
	glClearColor(1.0, 1.0, 1.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _sceneShader->bind();
	_sceneShader->getUniform(0).set(_projectionMatrix * _viewMatrix * _sceneModelMatrix);
	_sceneShader->getUniform(1).set(_sceneTexture->id, 0);
    _sceneQuad->render();
    _sceneShader->unbind();

	_sceneFramebuffer->blitToDefault(_sceneRT);

	renderBlur(_sceneRT);

	glActiveTexture(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, _blurVRT->tex->id);
	glGenerateMipmap(GL_TEXTURE_2D);

	_screenFramebuffer->bindForDrawing();
	_screenShader->bind();
	_screenShader->getUniform(0).set(_blurVRT->tex->id, 0);
	_screenQuad->render();
	_screenShader->unbind();


	renderBlur(_screenRT);
	//_screenFramebuffer->unbind(GL_FRAMEBUFFER);


	renderUi();
}

void screen::onTick()
{
}

void screen::onClosing()
{
}