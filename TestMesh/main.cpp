#include "testMesh.h"

#include "geometry.h"
#include "material.h"
#include "shader.h"
#include "color.h"
#include "mathUtils.h"
#include "camera.h"
#include "texture.h"
#include "octree.h"
#include "stopwatch.h"

#include <SDL/SDL.h>
#include <SDL/SDL_TTF.h>
#include <SDL/SDL_image.h>
#include <freetype\ft2build.h>
#include FT_FREETYPE_H
#include <glm\gtc\type_ptr.hpp>
#include <algorithm>
#include <memory>

stopwatch _stopwatch;

SDL_Window* _window;
SDL_GLContext _glContext;
FT_Library _freeTypeLibrary;

std::vector<geometry*> _geometries;
std::vector<glm::mat4> _modelMatrices;
std::vector<material*> _materials;
std::vector<texture*> _diffuseTextures;
std::vector<texture*> _normalTextures;

uint _texturesCount = 5;
uint _materialsCount = 5;

shader* _shader;
texture* _texture;
bool _isRunning = true;
bool _materialChanged = true;
bool _transformChanged = true;

glm::mat4 _projectionMatrix;
glm::mat4 _viewMatrix;
glm::mat4 _modelMatrix;

color _color = color(1.0f, 0.0f, 0.0f, 1.0f);

camera* _camera;

bool _isMouseDown = false;
glm::vec2 _mouseDownPos;
glm::vec2 _lastMousePos;
bool _rotating;
glm::vec3 _cameraPos;
float _rotationSpeed = 0.01f;

GLuint _materialsUbo;
GLuint _transformsUbo;
float t = 0.0f;
float i = 0.01f;

geometry* _quad;

float randf(float fMin, float fMax)
{
    float f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

#include <windows.h>

extern "C" {
    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

bool createGLWindow()
{
    _window = SDL_CreateWindow(
        "Testgeometry",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        1024,
        768,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if (_window == NULL)
    {
        LOG("Window could not be created! SDL_Error: " << SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 4);

    SDL_GL_SetAttribute(SDL_GL_RED_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_GREEN_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_BLUE_SIZE, 8);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 32);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    _glContext = SDL_GL_CreateContext(_window);

    if (!_glContext)
    {
        LOG("Could not create context:" << SDL_GetError());
        return false;
    }

    glewExperimental = GL_TRUE;

    GLenum glewInitStatus = glewInit();

    if (glewInitStatus != GLEW_OK)
    {
        LOG("Error" << glewGetErrorString(glewInitStatus))
            return false;
    }

    SDL_GL_SetSwapInterval(0);

    return true;
}

void initSDL()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        LOG("SDL could not initialize! SDL_Error: " << SDL_GetError());

    TTF_Init();
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF);
}

void initFreeType()
{
    auto error = FT_Init_FreeType(&_freeTypeLibrary);

    if (error)
        LOG("SDL could not initialize! SDL_Error: " << SDL_GetError());
}

void initGL()
{
    glClearColor(1.0f, 0.0f, 0.0f, 0.5f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void createQuad()
{
    auto vertices = std::vector<vertex>
    {
        vertex(glm::vec3(-0.5f, -0.5f, +0.0f)),
        vertex(glm::vec3(+0.5f, -0.5f, +0.0f)),
        vertex(glm::vec3(+0.5f, +0.5f, +0.0f)),
        vertex(glm::vec3(-0.5f, +0.5f, +0.0f))
    };

    auto indices = std::vector<uint> { 0, 1, 2, 2, 3, 0 };

    _quad = geometry::create(vertices, indices);
}

void initShader()
{
    _shader = new shader("SHADER", "shader.vert", "shader.frag");

    _shader->addAttribute("inPosition");
    _shader->addAttribute("inTexCoord");
    _shader->addAttribute("inNormal");

    _shader->init();
    _shader->addUniform("v", 0);
    _shader->addUniform("p", 1);
}

void initCamera()
{
    _camera = new camera();
    _camera->setPosition(glm::vec3(0.0f, 3.0f, 5.0f));
    _camera->setTarget(glm::vec3(0.0f));
    _projectionMatrix = glm::perspective<float>(glm::half_pi<float>(), 1024.0f / 768.0f, 0.1f, 100.0f);
    _viewMatrix = glm::lookAt<float>(_camera->getPosition(), _camera->getTarget(), _camera->getUp());
}

bool init()
{
    initSDL();
    initFreeType();

    if (!createGLWindow())
        return false;

    initGL();
    createQuad();
    initShader();
    initCamera();

    return true;
}

void mouseDown(SDL_Event e)
{
    _isMouseDown = true;
    _mouseDownPos = glm::vec2(e.motion.x, e.motion.y);
}

void mouseMove(SDL_Event e)
{
    glm::vec2 mousePos = glm::vec2(e.motion.x, e.motion.y);

    if (!_isMouseDown)
    {
        _lastMousePos = mousePos;
        return;
    }

    if (_isMouseDown && length(_mouseDownPos - _lastMousePos) > 5)
        _rotating = true;

    if (!_rotating)
    {
        _lastMousePos = mousePos;
        return;
    }

    float dx = (float)(_lastMousePos.x - mousePos.x);
    float dy = (float)(_lastMousePos.y - mousePos.y);

    dx *= _rotationSpeed;
    dy *= _rotationSpeed;

    _camera->orbit(_camera->getTarget(), _camera->getUp(), dx);
    _camera->orbit(_camera->getTarget(), _camera->getRight(), dy);

    _lastMousePos = mousePos;
}

void mouseUp(SDL_Event e)
{
    _rotating = false;
    _isMouseDown = false;
}

void mouseWheel(SDL_Event e)
{
    _camera->dolly(e.wheel.y);
}

void input()
{
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0)
    {
        switch (e.type)
        {
        case SDL_QUIT:
            _isRunning = false;
            break;
        case SDL_KEYDOWN:
            if (e.key.keysym.sym == SDLK_ESCAPE)
                _isRunning = false;
            break;
        case SDL_MOUSEBUTTONDOWN:
            mouseDown(e);
            break;
        case SDL_MOUSEMOTION:
            mouseMove(e);
            break;
        case SDL_MOUSEBUTTONUP:
            mouseUp(e);
            break;
        case SDL_MOUSEWHEEL:
            mouseWheel(e);
            break;
        default:
            break;
        }
    }
}

void printUniformBlocks()
{
    GLint numBlocks;
    GLint nameLen;

    std::vector<std::string> nameList;
    auto id = _shader->getId();
    glGetProgramiv(id, GL_ACTIVE_UNIFORM_BLOCKS, &numBlocks);
    nameList.reserve(numBlocks);

    std::cout << "found " << numBlocks << " block in shader" << std::endl;

    for (int blockIx = 0; blockIx < numBlocks; blockIx++) {
        glGetActiveUniformBlockiv(id, blockIx, GL_UNIFORM_BLOCK_NAME_LENGTH, &nameLen);

        std::vector<GLchar> name;
        name.resize(nameLen);
        glGetActiveUniformBlockName(id, blockIx, nameLen, NULL, &name[0]);

        nameList.push_back(std::string());
        nameList.back().assign(name.begin(), name.end() - 1); //Remove the null terminator.

    }

    for (unsigned int il = 0; il < nameList.size(); il++) {
        std::cout << "Block name: " << nameList[il] << std::endl;
    }

}

void update()
{
    if (_camera == nullptr)
        return;

    auto x = glm::cos(t);
    auto z = glm::sin(t);
    
    t += i;

    auto pos = glm::vec3(x, 0.2f, z) * 10.0f;
    _camera->setPosition(pos);
    _camera->update();

    _viewMatrix = glm::lookAt<float>(_camera->getPosition(), _camera->getTarget(), _camera->getUp());
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _shader->bind();

    _shader->getUniform(0).set(_viewMatrix);
    _shader->getUniform(1).set(_projectionMatrix);

    _quad->render();

    _shader->unbind();
}

void loop()
{
    Uint32 now = 0;
    Uint32 last = SDL_GetTicks();
    double dt = 0;
    double processedTime = 0.0;
    unsigned int frames = 0;

    Uint32 inputCost = 0;
    Uint32 updateCost = 0;
    Uint32 renderCost = 0;

    while (_isRunning)
    {
        now = SDL_GetTicks();
        dt = (double)(now - last) / 1000.0;
        last = now;

        input();
        update();

        //auto s =stopwatch::Measure([] { 
            render(); 
            SDL_GL_SwapWindow(_window); 
        //});
        //std::cout << s * 1000 << std::endl;

        frames++;
        processedTime += dt;

        if (processedTime > 1.0f)
        {
            frames = 0;
            processedTime -= 1.0;
        }
    }
}

void release()
{
    SDL_GL_DeleteContext(_glContext);
    _glContext = NULL;

    SDL_DestroyWindow(_window);
    _window = NULL;

    IMG_Quit();
    TTF_Quit();
    SDL_Quit();

    delete _shader;
    delete _camera;
}

int main(int argc, char* args[])
{
    if (!init())
        return -1;

    loop();

    release();

    return 0;
}