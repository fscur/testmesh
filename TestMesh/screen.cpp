#include "screen.h"
#include "application.h"
#include "octree.h"
#include "aabb.h"
#include <iostream> 
#include <glm\gtc\matrix_transform.hpp>
#include <SDL\SDL.h>
#include <SDL\SDL_image.h>

using namespace std;

screen::screen(std::string name, uint width, uint height) :
    window(name, width, height),
    _resolution(glm::vec2(_width, _height))
{
    SDL_Init(SDL_INIT_VIDEO);
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF);
}

screen::~screen()
{
    delete(_camera);
    delete(_shader);
    delete(_cube);
}

void screen::initGL()
{
    //glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
}

void screen::createCube()
{
    auto vertices = vector<vertex>
    {
        vertex(glm::vec3(+0.5f, +0.5f, +0.5f)), //0
        vertex(glm::vec3(+0.5f, -0.5f, +0.5f)), //1
        vertex(glm::vec3(-0.5f, -0.5f, +0.5f)), //2
        vertex(glm::vec3(-0.5f, +0.5f, +0.5f)), //3
        vertex(glm::vec3(+0.5f, +0.5f, -0.5f)), //4
        vertex(glm::vec3(+0.5f, -0.5f, -0.5f)), //5
        vertex(glm::vec3(-0.5f, -0.5f, -0.5f)), //6
        vertex(glm::vec3(-0.5f, +0.5f, -0.5f)) //7
    };

    auto indices = vector<uint>
    {
        0, 1, 2, 2, 3, 0,
        4, 5, 1, 1, 0, 4,
        7, 6, 5, 5, 4, 7,
        3, 2, 6, 6, 7, 3,
        4, 0, 3, 3, 7, 4,
        1, 5, 6, 6, 2, 1
    };

    _cube = geometry::create(vertices, indices);
}

void screen::initShader()
{
    _shader = new shader("SHADER", "shader.vert", "shader.frag");
    _shader->addAttribute("inPosition");
    _shader->init();
    _shader->addUniform("mvp", 0);
}

void screen::initCamera()
{
    _camera = new camera();
    _camera->setPosition(glm::vec3(3.0f, 2.0f, 4.5f));
    _camera->setTarget(glm::vec3(0.0f));
    _projectionMatrix = glm::perspective<float>(glm::half_pi<float>(), 1600.0f / 900.0f, 0.1f, 100.0f);
    _viewMatrix = glm::lookAt<float>(_camera->getPosition(), _camera->getTarget(), _camera->getUp());
}

void screen::onInit()
{
    initGL();
    initCamera();
    initShader();
    createCube();
}

void screen::onUpdate()
{
}

void screen::onRender()
{
    //glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE);
    //glCullFace(GL_FRONT);

    glClear(GL_COLOR_BUFFER_BIT || GL_DEPTH_BUFFER_BIT);

    _shader->bind();
    _shader->getUniform(0).set(_projectionMatrix * _viewMatrix);
    _cube->render();
    _shader->unbind();
}

void screen::onTick()
{
}

void screen::onClosing()
{
}