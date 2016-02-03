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
#include <cstdlib>
#include <iostream>
#include <ctime>

#include "bufferLockManager.h"

typedef struct
{
    GLuint count;
    GLuint instanceCount;
    GLuint firstIndex;
    GLuint baseVertex;
    GLuint baseInstance;
}
mdiCmd;

typedef struct
{
    glm::mat4 m;
    GLuint materialId;
    GLuint pad0;
    GLuint pad1;
    GLuint pad2;
}
drawData;

typedef struct
{
    float diffuseR;
    float diffuseG;
    float diffuseB;
    float diffuseA;
    float specularR;
    float specularG;
    float specularB;
    float specularA;
    GLuint64 diffuseHandle;
    GLuint64 normalHandle;
}
materialData;

bufferLockManager _bufferLockManager = bufferLockManager(true);

stopwatch _stopwatch;

SDL_Window* _window;
SDL_GLContext _glContext;
FT_Library freeLib;

const uint TRIPLE_BUFFER = 3;

std::vector<geometry*> _geometries;
std::vector<material*> _materialsLibrary;
std::vector<texture*> _diffuseTextures;
std::vector<texture*> _normalTextures;

uint _texturesCount = 2;
uint _materialsCount = 100;
uint _objectCount = 100;
uint _instanceCount = 1000;
uint _drawCount = _objectCount * _instanceCount;

std::vector<drawData> _drawData;
std::vector<vertex> _vertices;
std::vector<uint> _indices;

uint _vaoId = 0;
uint _vboId = 0;
uint _eboId = 0;
uint _drawIdBufferId = 0;
std::vector<uint> _mdiCmdBufferIds;
uint _materialsBufferId = 0;
uint _drawDataBufferId = 0;

uint _vboSize = 0;
uint _eboSize = 0;
uint _drawIdBufferSize = 0;
uint _mdiCmdBufferSize = 0;
uint _materialsBufferSize = 0;
uint _drawDataBufferSize = 0;

vertex* _vbo;
uint* _ebo;
uint* _drawIdBuffer;
mdiCmd* _mdiCmdBuffer;
materialData* _materialsBuffer;
drawData* _drawDataBuffer;

std::vector<mdiCmd*> _mdiCmdBuffers;

shader* _shader;
texture* _texture;
geometry* _geometry;

bool _isRunning = true;
bool _materialChanged = true;
bool _transformChanged = true;

glm::mat4 _projectionMatrix;
glm::mat4 _viewMatrix;

color _color = color(1.0f, 0.0f, 0.0f, 1.0f);

camera* _camera;

bool _isMouseDown = false;
glm::vec2 _mouseDownPos;
glm::vec2 _lastMousePos;
bool _rotating;
glm::vec3 _cameraPos;
float _rotationSpeed = 0.01f;
float _height = 0.0f;
float _sign = 1.0f;
uint _lastDrawRange = 0;
uint _drawRange = 0;
std::vector<GLsync> _drawFences;

float randf(float fMin, float fMax)
{
    float f = (double) rand() / RAND_MAX;
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

    if(_window == NULL)
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
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 0);

    _glContext = SDL_GL_CreateContext(_window);

    if(!_glContext)
    {
        LOG("Could not create context:" << SDL_GetError());
        return false;
    }

    SDL_GL_MakeCurrent(_window, _glContext);

    glewExperimental = GL_TRUE;

    GLenum glewInitStatus = glewInit();

    if(glewInitStatus != GLEW_OK)
    {
        LOG("Error" << glewGetErrorString(glewInitStatus))
            return false;
    }

    SDL_GL_SetSwapInterval(0);

    return true;
}

void createCubes()
{
    auto _octree = new octree(aabb(glm::vec3(-0.7, -0.7, -0.7), glm::vec3(0.7, 0.7, 0.7)), 5, 100);

    auto addVertex = [&](vertex& vertex)
    {
        uint index = -1;

        if(_octree->insert(vertex, index))
            _vertices.push_back(vertex);

        _indices.push_back(index);
    };

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

void createDrawData()
{
    for (auto i = 0; i < _drawCount; i++)
    {
        auto x = randf(-0.5f, 0.5f) * 10.0f;
        auto y = randf(-0.5f, 0.5f) * 10.0f;
        auto z = randf(-0.5f, 0.5f) * 10.0f;

        auto mat = glm::mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            x, y, z, 1.0f);

        auto matId = rand() % _materialsCount;

        auto data = drawData();
        data.m = mat;
        data.materialId = matId;
        data.pad0 = 0;
        data.pad1 = 0;
        data.pad2 = 0;

        _drawData.push_back(data);
    }
}

void createMaterials()
{
    for(auto i = 0; i < _materialsCount; i++)
    {
        auto r = rand() % _texturesCount;

        _materialsLibrary.push_back(createMaterial(_diffuseTextures[r], _normalTextures[r]));
    }
}

void createTextures()
{
    for(auto i = 0; i < _texturesCount; i++)
    {
        _diffuseTextures.push_back(texture::fromFile("diffuse.bmp"));
        _normalTextures.push_back(texture::fromFile("normal.bmp"));
    }
}

void initShader()
{
    _shader = new shader("SHADER", "shader.vert", "shader.frag");

    _shader->addAttribute("inPosition");
    _shader->addAttribute("inTexCoord");
    _shader->addAttribute("inNormal");
    _shader->addAttribute("inDrawId");

    _shader->init();
    _shader->addUniform("vp", 0);
}

void initCamera()
{
    _camera = new camera();
    _camera->setPosition(glm::vec3(0.0f, 3.0f, 20.0f));
    _camera->setTarget(glm::vec3(0.0f));
    _projectionMatrix = glm::perspective<float>(glm::half_pi<float>(), 1024.0f / 768.0f, 0.1f, 100.0f);
    _viewMatrix = glm::lookAt<float>(_camera->getPosition(), _camera->getTarget(), _camera->getUp());
}

void* newPersistentBuffer(GLenum bufferType, GLuint& bufferId, uint bufferSize)
{
    auto persistentMapFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    glCreateBuffers(1, &bufferId);
    glBindBuffer(bufferType, bufferId);
    glBufferStorage(bufferType, bufferSize, NULL, persistentMapFlags);
    return glMapBufferRange(bufferType, 0, bufferSize, persistentMapFlags);
}

void newNamedBufferData(GLenum bufferType, GLuint& bufferId, uint bufferSize, const void* data)
{
    glCreateBuffers(1, &bufferId);
    glBindBuffer(bufferType, bufferId);
    glNamedBufferData(bufferId, bufferSize, data, GL_STATIC_DRAW);
}

void fillNonPersistentBuffers()
{
    auto verticesCount = _vertices.size();
    auto indicesCount = _indices.size();
    auto tripleObjectsCount = _objectCount * TRIPLE_BUFFER;
    auto tripleDrawCount = _drawCount * TRIPLE_BUFFER;
    auto vertexSize = sizeof(vertex);
    
    _vboSize = sizeof(vertex) * verticesCount * tripleObjectsCount;
    _eboSize = sizeof(GLuint) * indicesCount * tripleObjectsCount;
    _drawIdBufferSize = sizeof(GLuint) * tripleDrawCount;
    _materialsBufferSize = sizeof(materialData) * _materialsCount;
    _vbo = new vertex[verticesCount * tripleObjectsCount];
    _ebo = new GLuint[indicesCount * tripleObjectsCount];
    _drawIdBuffer = new GLuint[tripleDrawCount];
    _materialsBuffer = new materialData[_materialsCount];

    for (uint i = 0; i < tripleObjectsCount; i++)
    {
        for (uint j = 0; j < verticesCount; j++)
        {
            auto index = i * verticesCount + j;
            _vbo[index] = _vertices[j];
        }

        for (uint j = 0; j < indicesCount; j++)
        {
            auto index = i * indicesCount + j;
            _ebo[index] = _indices[j];
        }
    }

    for(uint i = 0; i < tripleDrawCount; i++)
        _drawIdBuffer[i] = i;

    for (auto i = 0; i < _materialsCount; i++)
    {
        auto material = _materialsLibrary[i];

        auto diffuseColor = material->getDiffuseColor();
        auto specularColor = material->getSpecularColor();

        _materialsBuffer[i].diffuseR = diffuseColor.R;
        _materialsBuffer[i].diffuseG = diffuseColor.G;
        _materialsBuffer[i].diffuseB = diffuseColor.B;
        _materialsBuffer[i].diffuseA = diffuseColor.A;
        _materialsBuffer[i].specularR = specularColor.R;
        _materialsBuffer[i].specularG = specularColor.G;
        _materialsBuffer[i].specularB = specularColor.B;
        _materialsBuffer[i].specularA = specularColor.A;
        _materialsBuffer[i].diffuseHandle = material->getDiffuseTexture()->getHandle();
        _materialsBuffer[i].normalHandle = material->getNormalTexture()->getHandle();
    }

    auto indexCount = _indices.size();
    auto vertexCount = _vertices.size();

    for (auto j = 0; j < TRIPLE_BUFFER; j++)
    {
        auto mdiCmdBuffer = new mdiCmd[_objectCount];
        
        for (auto i = 0; i < _objectCount; i++)
        {
            mdiCmdBuffer[i].count = indexCount;
            mdiCmdBuffer[i].instanceCount = 0;
            mdiCmdBuffer[i].firstIndex = j * indexCount;
            mdiCmdBuffer[i].baseVertex = i * vertexCount;
            mdiCmdBuffer[i].baseInstance = i * _instanceCount;
        }

        _mdiCmdBuffers.push_back(mdiCmdBuffer);
    }
}

void createNonPersistentBuffers()
{
    glCreateVertexArrays(1, &_vaoId);
    glBindVertexArray(_vaoId);

    auto vertexSize = sizeof(vertex);
    auto floatSize = sizeof(float);

    newNamedBufferData(GL_ARRAY_BUFFER, _vboId, _vboSize, _vbo);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, vertexSize, (void*)(floatSize * 3));
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, vertexSize, (void*)(floatSize * 5));

    newNamedBufferData(GL_ARRAY_BUFFER, _drawIdBufferId, _drawIdBufferSize, _drawIdBuffer);
    glEnableVertexAttribArray(3);
    glVertexAttribIPointer(3, 1, GL_UNSIGNED_INT, 0, (void*)0);
    glVertexAttribDivisor(3, 1);

    newNamedBufferData(GL_ELEMENT_ARRAY_BUFFER, _eboId, _eboSize, _ebo);

    newNamedBufferData(GL_SHADER_STORAGE_BUFFER, _materialsBufferId, _materialsBufferSize, &_materialsBuffer[0]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _materialsBufferId);

    _mdiCmdBufferSize = _objectCount * sizeof(mdiCmd);

    for (auto i = 0; i < TRIPLE_BUFFER; i++)
    {
        uint bufferId;
        newNamedBufferData(GL_DRAW_INDIRECT_BUFFER, bufferId, _mdiCmdBufferSize, _mdiCmdBuffers[i]);
        _mdiCmdBufferIds.push_back(bufferId);
    }
}

void createPersistentBuffers()
{
    _drawDataBufferSize = _drawCount * sizeof(drawData);
    _drawDataBuffer = (drawData*)newPersistentBuffer(GL_SHADER_STORAGE_BUFFER, _drawDataBufferId, _drawDataBufferSize * TRIPLE_BUFFER);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _drawDataBufferId);
}

void fillPersistentBuffers()
{
    memcpy(_drawDataBuffer, &_drawData[0], sizeof(drawData) * _drawData.size());
}

void initGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

bool init()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        LOG("SDL could not initialize! SDL_Error: " << SDL_GetError());

    TTF_Init();
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF);

    auto error = FT_Init_FreeType(&freeLib);

    if(error)
        LOG("SDL could not initialize! SDL_Error: " << SDL_GetError());

    if(!createGLWindow())
        return false;

    initShader();
    initCamera();

    createDrawData();
    createTextures();
    createMaterials();
    createCubes();

    fillNonPersistentBuffers();
    createNonPersistentBuffers();

    createPersistentBuffers();
    fillPersistentBuffers();

    for(auto i = 0; i < TRIPLE_BUFFER; i++)
        _drawFences.push_back(GLsync());

    initGL();

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

    if(!_isMouseDown)
    {
        _lastMousePos = mousePos;
        return;
    }

    if(_isMouseDown && length(_mouseDownPos - _lastMousePos) > 5)
        _rotating = true;

    if(!_rotating)
    {
        _lastMousePos = mousePos;
        return;
    }

    float dx = (float) (_lastMousePos.x - mousePos.x);
    float dy = (float) (_lastMousePos.y - mousePos.y);

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

    while(SDL_PollEvent(&e) != 0)
    {
        switch(e.type)
        {
        case SDL_QUIT:
            _isRunning = false;
            break;
        case SDL_KEYDOWN:
            if(e.key.keysym.sym == SDLK_ESCAPE)
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

void updateModelMatricesBuffer()
{
    for (uint i = 0; i < _drawCount; i++)
        _drawData[i].m[3][1] += _height;

    memcpy(_drawDataBuffer + (_drawRange * _drawCount), &_drawData[0], sizeof(drawData) * _drawData.size());
 
    if (glm::abs(_height) > 0.5f)
        _sign *= -1.0f;

    _height += 0.01f * _sign;
}

void update()
{
    if(_camera == nullptr)
        return;

    _camera->update();

    _viewMatrix = glm::lookAt<float>(_camera->getPosition(), _camera->getTarget(), _camera->getUp());

    updateModelMatricesBuffer();
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _shader->getUniform(0).set(_projectionMatrix * _viewMatrix);
    
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, _mdiCmdBufferIds[_drawRange]);
    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, nullptr, _objectCount, 0);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, 0);

    _lastDrawRange = _drawRange;
    _drawRange = ++_drawRange % TRIPLE_BUFFER;
}

void lock()
{
    _bufferLockManager.lockRange(_lastDrawRange, 1);
}

void waitLock()
{
    _bufferLockManager.waitForLockedRange(_drawRange, 1);
}

void loop()
{
    while(_isRunning)
    {
        input();

        waitLock();

        update();

        stopwatch::MeasureInMilliseconds([&]
        {
            render();
            lock();
            glFlush();
            SDL_GL_SwapWindow(_window);
        }, "render");
    }
}

void release()
{
    glDisableVertexAttribArray(6);
    glDisableVertexAttribArray(5);
    glDisableVertexAttribArray(4);
    glDisableVertexAttribArray(3);
    glDisableVertexAttribArray(2);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(0);

    glDeleteBuffers(1, &_vboId);
    glDeleteBuffers(1, &_drawIdBufferId);
    glDeleteBuffers(1, &_eboId);
    
    for (auto i = 0; i < TRIPLE_BUFFER; i++)
    {
        glDeleteBuffers(1, &_mdiCmdBufferIds[i]);
    }

    glDeleteBuffers(1, &_materialsBufferId);
    glDeleteBuffers(1, &_drawDataBufferId);

    delete[] _vbo;
    delete[] _ebo;
    delete[] _drawIdBuffer;
    delete[] _drawDataBuffer;
    delete[] _mdiCmdBuffer;

    glDeleteProgram(_shader->getId());

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
    std::srand(std::time(0));

    if(!init())
        return -1;

    _shader->bind();

    loop();

    _shader->unbind();

    release();

    return 0;
}