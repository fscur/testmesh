#include "screen.h"
#include "application.h"
#include "octree.h"
#include "aabb.h"
#include <iostream> 
#include <glm\gtc\matrix_transform.hpp>

screen::screen(std::string name, uint width, uint height) :
    window(name, width, height)
{
}

void screen::createQuad()
{
    auto vertices = std::vector<vertex>
    {
        vertex(glm::vec3(0.0f, 0.0f, +0.0f), glm::vec2(0.0f, 1.0f)),
        vertex(glm::vec3(1.0f, 0.0f, +0.0f), glm::vec2(1.0f, 1.0f)),
        vertex(glm::vec3(1.0f, 1.0f, +0.0f), glm::vec2(1.0f, 0.0f)),
        vertex(glm::vec3(0.0f, 1.0f, +0.0f), glm::vec2(0.0f, 0.0f))
    };

    auto indices = std::vector<uint>{ 0, 1, 2, 2, 3, 0 };

    _geometry = geometry::create(vertices, indices);
    _modelMatrix = glm::mat4(1.0f);
}

void screen::initGL()
{
    glClearColor(0.8f, 0.0f, 0.0f, 1.0f);
}

void screen::initShader()
{
    _shader = new shader("SHADER", "shader.vert", "shader.frag");

    _shader->addAttribute("inPosition");
    _shader->addAttribute("inTexCoord");
    _shader->addAttribute("inNormal");
    _shader->addAttribute("inModelMatrix");

    _shader->init();
    _shader->addUniform("mvp", 0);
    _shader->addUniform("diffuseTextureArrayIndex", 1);
    _shader->addUniform("diffuseTexturePageIndex", 2);
    _shader->addUniform("textureArrays", 3);
}

void screen::initCamera()
{
    _camera = new camera();
    _camera->setPosition(glm::vec3(0.0f, 3.0f, 5.0f));
    _camera->setTarget(glm::vec3(0.0f));
    _projectionMatrix = glm::perspective<float>(glm::half_pi<float>(), 1024.0f / 768.0f, 0.1f, 100.0f);
    _viewMatrix = glm::lookAt<float>(_camera->getPosition(), _camera->getTarget(), _camera->getUp());
}

screen::~screen()
{
}

void screen::onInit()
{
    initCamera();
    initShader();
    initGL();
    createQuad();
}

void screen::onUpdate()
{
}

void screen::onRender()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _shader->bind();

    _shader->unbind();
}

void screen::onTick()
{
}

void screen::onClosing()
{
}