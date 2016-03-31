#include "testMesh.h"
#include "font.h"
#include "geometry.h"
#include "material.h"
#include "shader.h"
#include "color.h"
#include "mathUtils.h"
#include "camera.h"
#include "texture.h"
#include "octree.h"
#include "stopwatch.h"
#include "texturesManager.h"
#include "buffer.h"

#include <sdl\sdl.h>
#include <sdl\sdl_ttf.h>
#include <sdl\sdl_image.h>

#include <glm\gtc\type_ptr.hpp>

#include <windows.h>

#include <algorithm>
#include <memory>

stopwatch _stopwatch;

SDL_Window* _window;
SDL_GLContext _glContext;

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

struct glyphBlock
{
    int textureUnit;
    float texturePage;
};

geometry* _quad;
font* _font;
texturesManager* _texManager;
buffer* _glyphBuffer;
glyphBlock _glyphBlock;
GLuint _glyphTexture;
int _width = 3200;
int _height = 1800;
float _aspect = (float)_width/(float)_height;

float randf(float fMin, float fMax)
{
    float f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

extern "C" {
    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

bool createGLWindow()
{
    _window = SDL_CreateWindow(
        "Testgeometry",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        _width,
        _height,
        SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL);

    if (_window == NULL)
    {
        LOG("Window could not be created! SDL_Error: " << SDL_GetError());
        return false;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 5);

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

void initGL()
{
    glClearColor(1.0f, 0.8f, 0.0f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void initTexturesManager()
{
    _texManager = new texturesManager(false, false, 1);
}

void initFont()
{
    _font = new font("Consola.ttf", 192);
    auto glyph = _font->getGlyph(65);

    glGenTextures(1, &_glyphTexture);
    glBindTexture(GL_TEXTURE_2D, _glyphTexture);
    //phi::size<GLuint>(_texWidth, _texHeight), GL_RGBA, GL_RED, GL_UNSIGNED_BYTE
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, glyph->w0, glyph->h0, 0, GL_RED, GL_UNSIGNED_BYTE, NULL);
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glTexSubImage2D(GL_TEXTURE_2D, 0, 0, 0, glyph->w, glyph->h, GL_RED, GL_UNSIGNED_BYTE, glyph->data);
    glGenerateMipmap(GL_TEXTURE_2D);
    glActiveTexture(GL_TEXTURE0 + 0);
    glBindTexture(GL_TEXTURE_2D, _glyphTexture);


    _modelMatrix = glm::mat4(
        glyph->w0/(((float)_width * 0.5f)) * _aspect, 0.0f, 0.0f, 0.0f,
        0.0f, glyph->h0 / (((float)_height * 0.5f)), 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

void initGlyphBuffer()
{
    _glyphBuffer = new buffer(bufferTarget::uniform);

    _glyphBuffer->data(
        sizeof(glyphBlock),
        &_glyphBlock,
        bufferDataUsage::dynamicDraw);

    _glyphBuffer->bindBufferBase(0);
}

void createQuad()
{
    auto vertices = std::vector<vertex>
    {
        vertex(glm::vec3(-0.5f, -0.5f, +0.0f), glm::vec2(0.0f, 1.0f)),
        vertex(glm::vec3(+0.5f, -0.5f, +0.0f), glm::vec2(1.0f, 1.0f)),
        vertex(glm::vec3(+0.5f, +0.5f, +0.0f), glm::vec2(1.0f, 0.0f)),
        vertex(glm::vec3(-0.5f, +0.5f, +0.0f), glm::vec2(0.0f, 0.0f))
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
    _shader->addUniform("m", 0);
    _shader->addUniform("v", 1);
    _shader->addUniform("p", 2);
    _shader->addUniform("glyphTexture", 3);
}

void initCamera()
{
    _camera = new camera();
    _camera->setPosition(glm::vec3(0.0f, 0.0f, 5.0f));
    _camera->setTarget(glm::vec3(0.0f));
    _camera->update();
    //_projectionMatrix = glm::perspective<float>(glm::half_pi<float>(), 1024.0f / 768.0f, 0.1f, 100.0f);
    auto size = 1.0f;
    _projectionMatrix = glm::ortho<float>(-size * _aspect, size * _aspect, -size, size, 0, 1000);
    _viewMatrix = glm::lookAt<float>(_camera->getPosition(), _camera->getTarget(), _camera->getUp());
}

bool init()
{
    initSDL();

    if (!createGLWindow())
        return false;

    initGL();
    //initTexturesManager();
    initFont();
    //initGlyphBuffer();
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

void update()
{
    /*if (_camera == nullptr)
        return;

    auto x = glm::cos(t);
    auto z = glm::sin(t);
    
    t += i;

    auto pos = glm::vec3(x, 0.2f, z) * 10.0f;
    _camera->setPosition(pos);
    _camera->update();

    _viewMatrix = glm::lookAt<float>(_camera->getPosition(), _camera->getTarget(), _camera->getUp());*/
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    _shader->bind();

    _shader->getUniform(0).set(_modelMatrix);
    _shader->getUniform(1).set(_viewMatrix);
    _shader->getUniform(2).set(_projectionMatrix);
    _shader->getUniform(3).set(_glyphTexture, 0);
    //_shader->getUniform(2).set(_texManager->units);

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