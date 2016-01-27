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

//#include <GL\GL.h>

stopwatch _stopwatch;

SDL_Window* _window;
SDL_GLContext _glContext;
FT_Library freeLib;

std::vector<geometry*> _geometries;
std::vector<glm::mat4> _modelMatrices;
std::vector<material*> _materials;
std::vector<texture*> _diffuseTextures;
std::vector<texture*> _normalTextures;

uint _texturesCount = 100;
uint _materialsCount = 100;
uint _instanceCount = 1000;

shader* _shader;
texture* _texture;
geometry* _geometry;

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

void initGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

geometry* createCube()
{
    auto _octree = new octree(aabb(glm::vec3(-0.7, -0.7, -0.7), glm::vec3(0.7, 0.7, 0.7)), 5, 100);
    auto vertices = std::vector<vertex>();
    auto indices = std::vector<uint>();
    auto addVertex = [&](vertex& vertex)
    {
        uint index = -1;

        if (_octree->insert(vertex, index))
            vertices.push_back(vertex);

        indices.push_back(index);
    };

    auto s = vertices.size();

    addVertex(vertex(glm::vec3(-0.5f, -0.5f, +0.5f), glm::vec2(0.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    addVertex(vertex(glm::vec3(+0.5f, -0.5f, +0.5f), glm::vec2(1.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));
    addVertex(vertex(glm::vec3(+0.5f, +0.5f, +0.5f), glm::vec2(1.0, 1.0), glm::vec3(0.0, 0.0, 1.0)));
    addVertex(vertex(glm::vec3(+0.5f, +0.5f, +0.5f), glm::vec2(1.0, 1.0), glm::vec3(0.0, 0.0, 1.0)));
    addVertex(vertex(glm::vec3(-0.5f, +0.5f, +0.5f), glm::vec2(0.0, 1.0), glm::vec3(0.0, 0.0, 1.0)));
    addVertex(vertex(glm::vec3(-0.5f, -0.5f, +0.5f), glm::vec2(0.0, 0.0), glm::vec3(0.0, 0.0, 1.0)));

    addVertex(vertex(glm::vec3(+0.5f, -0.5f, +0.5f), glm::vec2(0.0, 0.0), glm::vec3(1.0, 0.0, 0.0)));
    addVertex(vertex(glm::vec3(+0.5f, -0.5f, -0.5f), glm::vec2(1.0, 0.0), glm::vec3(1.0, 0.0, 0.0)));
    addVertex(vertex(glm::vec3(+0.5f, +0.5f, -0.5f), glm::vec2(1.0, 1.0), glm::vec3(1.0, 0.0, 0.0)));
    addVertex(vertex(glm::vec3(+0.5f, +0.5f, -0.5f), glm::vec2(1.0, 1.0), glm::vec3(1.0, 0.0, 0.0)));
    addVertex(vertex(glm::vec3(+0.5f, +0.5f, +0.5f), glm::vec2(0.0, 1.0), glm::vec3(1.0, 0.0, 0.0)));
    addVertex(vertex(glm::vec3(+0.5f, -0.5f, +0.5f), glm::vec2(0.0, 0.0), glm::vec3(1.0, 0.0, 0.0)));

    addVertex(vertex(glm::vec3(+0.5f, -0.5f, -0.5f), glm::vec2(0.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
    addVertex(vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(1.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));
    addVertex(vertex(glm::vec3(-0.5f, +0.5f, -0.5f), glm::vec2(1.0, 1.0), glm::vec3(0.0, 0.0, -1.0)));
    addVertex(vertex(glm::vec3(-0.5f, +0.5f, -0.5f), glm::vec2(1.0, 1.0), glm::vec3(0.0, 0.0, -1.0)));
    addVertex(vertex(glm::vec3(+0.5f, +0.5f, -0.5f), glm::vec2(0.0, 1.0), glm::vec3(0.0, 0.0, -1.0)));
    addVertex(vertex(glm::vec3(+0.5f, -0.5f, -0.5f), glm::vec2(0.0, 0.0), glm::vec3(0.0, 0.0, -1.0)));

    addVertex(vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, -0.5f, +0.5f), glm::vec2(1.0, 0.0), glm::vec3(-1.0, 0.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, +0.5f, +0.5f), glm::vec2(1.0, 1.0), glm::vec3(-1.0, 0.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, +0.5f, +0.5f), glm::vec2(1.0, 1.0), glm::vec3(-1.0, 0.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, +0.5f, -0.5f), glm::vec2(0.0, 1.0), glm::vec3(-1.0, 0.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(0.0, 0.0), glm::vec3(-1.0, 0.0, 0.0)));

    addVertex(vertex(glm::vec3(+0.5f, -0.5f, +0.5f), glm::vec2(0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, -0.5f, +0.5f), glm::vec2(1.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(1.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, -0.5f, -0.5f), glm::vec2(1.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
    addVertex(vertex(glm::vec3(+0.5f, -0.5f, -0.5f), glm::vec2(0.0, 1.0), glm::vec3(0.0, -1.0, 0.0)));
    addVertex(vertex(glm::vec3(+0.5f, -0.5f, +0.5f), glm::vec2(0.0, 0.0), glm::vec3(0.0, -1.0, 0.0)));

    addVertex(vertex(glm::vec3(+0.5f, +0.5f, -0.5f), glm::vec2(0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, +0.5f, -0.5f), glm::vec2(1.0, 0.0), glm::vec3(0.0, 1.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, +0.5f, +0.5f), glm::vec2(1.0, 1.0), glm::vec3(0.0, 1.0, 0.0)));
    addVertex(vertex(glm::vec3(-0.5f, +0.5f, +0.5f), glm::vec2(1.0, 1.0), glm::vec3(0.0, 1.0, 0.0)));
    addVertex(vertex(glm::vec3(+0.5f, +0.5f, +0.5f), glm::vec2(0.0, 1.0), glm::vec3(0.0, 1.0, 0.0)));
    addVertex(vertex(glm::vec3(+0.5f, +0.5f, -0.5f), glm::vec2(0.0, 0.0), glm::vec3(0.0, 1.0, 0.0)));

    return geometry::create(vertices, indices);
}

material* createMaterial(texture* diffuseTexure, texture* normalTexture)
{
    auto r = randf(0.0f, 1.0f);
    auto g = randf(0.0f, 1.0f);
    auto b = randf(0.0f, 1.0f);

    auto diffuse = color(r, g, b, 1);

    r = randf(0.0f, 1.0f);
    g = randf(0.0f, 1.0f);
    b = randf(0.0f, 1.0f);

    auto specular = color(r, g, b, 1);

    return new material(diffuse, specular, diffuseTexure, normalTexture);
}

void createMaterials(uint n)
{
    _materials.push_back(
        new material(color(0.0f, 0.0f, 0.5f, 1.0f), color(0.0f, 0.0f, 1.0f, 1.0f), _diffuseTextures[0], _diffuseTextures[0]));

    _materials.push_back(
        new material(color(0.2f, 0.0f, 0.2f, 1.0f), color(1.0f, 0.0f, 0.0f, 1.0f), _diffuseTextures[0], _diffuseTextures[0]));

    for (auto i = 0; i < n -2; i++)
    {
        auto r = rand() % _texturesCount;

        _materials.push_back(createMaterial(_diffuseTextures[r], _normalTextures[r]));
    }
}

void createTextures(uint n)
{
    for (auto i = 0; i < n; i++)
    {
        _diffuseTextures.push_back(texture::fromFile("diffuse.bmp"));
        _normalTextures.push_back(texture::fromFile("normal.bmp"));
    }
}

void createModelMatrices(uint n)
{
    for (auto v = 0; v < n; v++)
    {
        auto mat = glm::mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            (float)randf(-0.5f, 0.5f) * 10.0f, (float)randf(-0.5f, 0.5f) * 10.0f, (float)randf(-0.5f, 0.5f) * 10.0f, 1.0f);

        _modelMatrices.push_back(mat);
    }
}

void createCubes(uint n)
{
    _geometry = createCube();
    createModelMatrices(n);
    createTextures(_texturesCount);
    createMaterials(_materialsCount);
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
    _shader->addUniform("matIndex", 2);
    //_shader->addUniform("mIndex", 3);
    
    auto id = _shader->getId();

    GLuint transformsBlockIndex = glGetUniformBlockIndex(id, "TransformsBlock");
    GLuint materialsBlockIndex = glGetUniformBlockIndex(id, "MaterialsBlock");

    glGenBuffers(1, &_transformsUbo);
    glGenBuffers(1, &_materialsUbo);

    glUniformBlockBinding(id, transformsBlockIndex, 0);
    glUniformBlockBinding(id, materialsBlockIndex, 1);

    glBindBufferBase(GL_UNIFORM_BUFFER, 0, _transformsUbo);
    glBindBufferBase(GL_UNIFORM_BUFFER, 1, _materialsUbo);
}

void initTexture()
{
    _texture = texture::fromFile("C:\\test.bmp");
}

void initCamera()
{
    _camera = new camera();
    _camera->setPosition(glm::vec3(0.0f, 3.0f, 5.0f));
    _camera->setTarget(glm::vec3(0.0f));
    _projectionMatrix = glm::perspective<float>(glm::half_pi<float>(), 1024.0f / 768.0f, 0.1f, 100.0f);
    _viewMatrix = glm::lookAt<float>(_camera->getPosition(), _camera->getTarget(), _camera->getUp());
    _modelMatrix = glm::mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);
}

bool init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        LOG("SDL could not initialize! SDL_Error: " << SDL_GetError());

    TTF_Init();
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF);

    auto error = FT_Init_FreeType(&freeLib);

    if (error)
    {
        LOG("SDL could not initialize! SDL_Error: " << SDL_GetError());
    }

    if (!createGLWindow())
        return false;

    initGL();
    initTexture();
    createCubes(_instanceCount);
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

float t = 0.0f;
float i = 0.01f;

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

    if (_materialChanged)
    {
        GLint blockSize;

        auto id = _shader->getId();

        GLuint blockIndex = glGetUniformBlockIndex(id, "MaterialsBlock");

        glGetActiveUniformBlockiv(id, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

        GLubyte * blockBuffer = (GLubyte *)malloc(blockSize);

        //const GLchar *names[] = { "materials.materials" };

        //GLuint* indices = new GLuint[1]();
        //glGetUniformIndices(id, 1, names, indices);

        //GLint* offset = new GLint[1]();
        //glGetActiveUniformsiv(id, 1, indices, GL_UNIFORM_OFFSET, offset);

        for (auto i = 0; i < _materialsCount; i++)
        {
            auto material = _materials[i];
            auto diffuseColor = material->getDiffuseColor();
            auto specularColor = material->getSpecularColor();

            GLfloat colors[] = {
                diffuseColor.R,
                diffuseColor.G,
                diffuseColor.B,
                diffuseColor.A,
                specularColor.R,
                specularColor.G,
                specularColor.B,
                specularColor.A };

            GLuint64 handles[] =
            {
                material->getDiffuseTexture()->getHandle(),
                material->getNormalTexture()->getHandle()
            };

            auto arrayStartPos = 48 * i;

            memcpy(blockBuffer + arrayStartPos, colors, 32);
            memcpy(blockBuffer + arrayStartPos + 32, handles, 16);
        }

        glBindBuffer(GL_UNIFORM_BUFFER, _materialsUbo);
        glBufferData(GL_UNIFORM_BUFFER, blockSize, blockBuffer, GL_DYNAMIC_DRAW);

        _materialChanged = false;
    }

    if (_transformChanged)
    {
        GLint blockSize;

        auto id = _shader->getId();

        GLuint blockIndex = glGetUniformBlockIndex(id, "TransformsBlock");

        glGetActiveUniformBlockiv(id, blockIndex, GL_UNIFORM_BLOCK_DATA_SIZE, &blockSize);

        //GLubyte * blockBuffer = (GLubyte *)malloc(blockSize);

        //auto s = _modelMatrices.size();

        //for (auto i = 0; i < 1; i++)
        //{
        //    //auto modelMatrix = _modelMatrices[i];
        //    //auto transP = glm::transpose(modelMatrix);
        //    //float* modelMatrixPtr = glm::value_ptr(modelMatrix);

        //    GLfloat matx[] =
        //    {
        //        1.0f, 0.0f, 0.0f, 0.0f,
        //        0.0f, 1.0f, 0.0f, 0.0f,
        //        0.0f, 0.0f, 1.0f, 0.0f,
        //        0.0f, 0.0f, 0.0f, 1.0f
        //    };

        //    //memcpy(blockBuffer + 64 * i, matx, 64);
        //    memcpy(blockBuffer + 64 * i, &_modelMatrices[0], 64);
        //}

        glBindBuffer(GL_UNIFORM_BUFFER, _transformsUbo);
        //std::cout << glewGetErrorString(glGetError()) << std::endl;
        glBufferData(GL_UNIFORM_BUFFER, blockSize, &_modelMatrices[0], GL_DYNAMIC_DRAW);
        //std::cout << glewGetErrorString(glGetError()) << std::endl;
        _transformChanged = false;
    }
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _shader->getUniform(0).set(_viewMatrix);

    auto matIndex = rand() % _materialsCount;
    auto mat = _materials[matIndex];

    _shader->getUniform(2).set(matIndex);
    //_shader->getUniform(3).set((uint)i);

    _geometry->render(_instanceCount);
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

        auto s =stopwatch::Measure([] { render(); SDL_GL_SwapWindow(_window); });
        std::cout << s * 1000 << std::endl;

        //printUniformBlocks();

        //system("pause");

        frames++;
        processedTime += dt;

        if (processedTime > 1.0f)
        {
            frames = 0;
            processedTime -= 1.0;
        }

        //system("pause");
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

    _shader->bind();
    _shader->getUniform(1).set(_projectionMatrix);

    _geometry->bind();

    loop();

    _geometry->unbind();
    _shader->unbind();

    release();

    return 0;
}