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
//#include <GL\GL.h>

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

stopwatch _stopwatch;

SDL_Window* _window;
SDL_GLContext _glContext;
FT_Library freeLib;

const uint TRIPLE_BUFFER = 3;

std::vector<geometry*> _geometries;

std::vector<material*> _materialsLibrary;
std::vector<materialData> _materialsBuffer;
//std::vector<drawData> _drawDataBuffer;
std::vector<texture*> _diffuseTextures;
std::vector<texture*> _normalTextures;

uint _texturesCount = 2;
uint _materialsCount = 100;
uint _objectCount = 100;
uint _instanceCount = 1000;
uint _drawCount = _objectCount * _instanceCount;

std::vector<vertex> _vertices;
std::vector<uint> _indices;

uint _vaoId = 0;
uint _positionsBufferId = 0;
uint _texCoordsBufferId = 0;
uint _drawIndexBufferId = 0;
uint _modelMatricesBufferId = 0;
uint _indicesBufferId = 0;
uint _mdiCmdBufferId = 0;
uint _materialsBufferId = 0;
uint _drawDataBufferId = 0;

uint _positionsBufferSize = 0;
uint _texCoordsBufferSize = 0;
uint _drawIndexBufferSize = 0;
uint _indicesBufferSize = 0;
uint _modelMatricesBufferSize = 0;
uint _mdiCmdBufferSize = 0;
uint _drawDataBufferSize = 0;

float* _positionsBuffer;
float* _texCoordsBuffer;
float* _normalsBuffer;
uint* _drawIndexBuffer;
glm::mat4* _modelMatricesBuffer;
uint* _indicesBuffer;
mdiCmd* _mdiCmdBuffer;
drawData* _drawDataBuffer;

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

float t = 0.00f;
float i = 0.00f;
float height = 0.0f;
bool isDecreasingHeight = false;
uint _lastDrawRange = 0;
uint _drawRange = 0;
std::vector<GLsync> _drawFences;

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

void initGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
}

void createCubes()
{
    auto _octree = new octree(aabb(glm::vec3(-0.7, -0.7, -0.7), glm::vec3(0.7, 0.7, 0.7)), 5, 100);

    auto addVertex = [&](vertex& vertex)
    {
        uint index = -1;

        if (_octree->insert(vertex, index))
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

void createMaterials()
{
    for (auto i = 0; i < _materialsCount; i++)
    {
        auto r = rand() % _texturesCount;

        _materialsLibrary.push_back(createMaterial(_diffuseTextures[r], _normalTextures[r]));
    }

    for (auto i = 0; i < _materialsCount; i++)
    {
        auto material = _materialsLibrary[i];
        auto data = materialData();
        auto diffuseColor = material->getDiffuseColor();
        data.diffuseR = diffuseColor.R;
        data.diffuseG = diffuseColor.G;
        data.diffuseB = diffuseColor.B;
        data.diffuseA = diffuseColor.A;

        auto specularColor = material->getSpecularColor();
        data.specularR = specularColor.R;
        data.specularG = specularColor.G;
        data.specularB = specularColor.B;
        data.specularA = specularColor.A;

        data.diffuseHandle = material->getDiffuseTexture()->getHandle();
        data.normalHandle = material->getNormalTexture()->getHandle();

        _materialsBuffer.push_back(data);
    }
}

void createTextures()
{
    for (auto i = 0; i < _texturesCount; i++)
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
    _shader->addAttribute("inDrawId");
    _shader->addAttribute("inModelMatrix");
    //_shader->addAttribute("inNormal");

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

void createDrawDataBuffer()
{
 //   _drawDataBuffer = new drawData[_drawCount];

    /*for (auto i = 0; i < _drawCount; i++)
    {
        auto x = randf(-0.5f, 0.5f) * 10.0f;
        auto y = randf(-0.5f, 0.5f) * 10.0f;
        auto z = randf(-0.5f, 0.5f) * 10.0f;

        auto mat = glm::mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            x, y, z, 1.0f);

        auto data = drawData();

        data.m = mat;
        data.materialId = rand() % _materialsCount;
        data.pad0 = 0;
        data.pad1 = 0;
        data.pad2 = 0;

        _drawDataBuffer.push_back(data);
    }

    glCreateBuffers(1, &_drawDataBufferId);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _drawDataBufferId);
    glNamedBufferData(_drawDataBufferId, sizeof(drawData) * _drawCount, &_drawDataBuffer[0], GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _drawDataBufferId);*/

    glCreateBuffers(1, &_materialsBufferId);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, _materialsBufferId);
    glNamedBufferData(_materialsBufferId, sizeof(materialData) * _materialsCount, &_materialsBuffer[0], GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, _materialsBufferId);
}

void* newPersistentBuffer(GLenum bufferType, GLuint& bufferId, uint bufferSize)
{
    auto persistentMapFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;

    glCreateBuffers(1, &bufferId);
    glBindBuffer(bufferType, bufferId);
    glBufferStorage(bufferType, bufferSize, NULL, persistentMapFlags);
    return glMapBufferRange(bufferType, 0, bufferSize, persistentMapFlags);
}

void createBuffers()
{
    _positionsBufferSize = _vertices.size() * 3 * sizeof(float) * _objectCount;
    _texCoordsBufferSize = _vertices.size() * 2 * sizeof(float) * _objectCount;
    _drawIndexBufferSize = _drawCount * sizeof(GLuint);
    _indicesBufferSize = _indices.size() * sizeof(uint) * _objectCount;
    _drawDataBufferSize = _drawCount * sizeof(drawData);
    _mdiCmdBufferSize = _objectCount * sizeof(mdiCmd);

    glCreateVertexArrays(1, &_vaoId);
    glBindVertexArray(_vaoId);

    _positionsBuffer = (float*)newPersistentBuffer(GL_ARRAY_BUFFER, _positionsBufferId, _positionsBufferSize * TRIPLE_BUFFER);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    _texCoordsBuffer = (float*)newPersistentBuffer(GL_ARRAY_BUFFER, _texCoordsBufferId, _texCoordsBufferSize * TRIPLE_BUFFER);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    _drawIndexBuffer = (uint*)newPersistentBuffer(GL_ARRAY_BUFFER, _drawIndexBufferId, _drawIndexBufferSize * TRIPLE_BUFFER);
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, 0, NULL);
    glVertexAttribDivisor(2, 1);

    _indicesBuffer = (uint*)newPersistentBuffer(GL_ELEMENT_ARRAY_BUFFER, _indicesBufferId, _indicesBufferSize * TRIPLE_BUFFER);

    _mdiCmdBuffer = (mdiCmd*)newPersistentBuffer(GL_DRAW_INDIRECT_BUFFER, _mdiCmdBufferId, _mdiCmdBufferSize);

    _drawDataBuffer = (drawData*)newPersistentBuffer(GL_SHADER_STORAGE_BUFFER, _drawDataBufferId, _drawDataBufferSize * TRIPLE_BUFFER);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _drawDataBufferId);
    /*auto persistentMapFlags = GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT;
    */

    /*glCreateBuffers(1, &_drawDataBufferId);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _drawDataBufferId);
    glBufferStorage(GL_SHADER_STORAGE_BUFFER, _drawDataBufferSize * TRIPPLE_BUFFER, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);
    _drawDataBuffer = (drawData*)glMapBufferRange(GL_SHADER_STORAGE_BUFFER, 0, _drawDataBufferSize * TRIPPLE_BUFFER, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT);*/

    /*_modelMatricesBuffer = (glm::mat4*) newPersistentBuffer(GL_ARRAY_BUFFER, _modelMatricesBufferId, _modelMatricesBufferSize * TRIPPLE_BUFFER);
    for (unsigned int i = 0; i < 4; i++)
    {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (const GLvoid*)(sizeof(GLfloat) * i * 4));
        glVertexAttribDivisor(3 + i, 1);
    }*/
}

void fillBuffers()
{
    int vIndex = -1;
    int tIndex = -1;
    int nIndex = -1;

    for (uint i = 0; i < _objectCount * TRIPLE_BUFFER; i++)
    {
        for (auto vertex : _vertices)
        {
            GLfloat x = vertex.GetPosition().x;
            GLfloat y = vertex.GetPosition().y;
            GLfloat z = vertex.GetPosition().z;

            _positionsBuffer[++vIndex] = x;
            _positionsBuffer[++vIndex] = y;
            _positionsBuffer[++vIndex] = z;

            GLfloat u = vertex.GetTexCoord().x;
            GLfloat v = vertex.GetTexCoord().y;

            _texCoordsBuffer[++tIndex] = u;
            _texCoordsBuffer[++tIndex] = v;
        }
    }

    for (uint i = 0; i < _drawCount * TRIPLE_BUFFER; i++)
        _drawIndexBuffer[i] = i;

    auto indicesCount = _indices.size();
    for (uint i = 0; i < _objectCount * TRIPLE_BUFFER; i++)
    {
        for (uint j = 0; j < indicesCount; j++)
        {
            auto index = i * indicesCount + j;
            _indicesBuffer[index] = _indices[j];
        }
    }

    auto indexCount = _indices.size();
    auto vertexCount = _vertices.size();

    for (uint i = 0; i < _objectCount; i++)
    {
        _mdiCmdBuffer[i].count = indexCount;
        _mdiCmdBuffer[i].instanceCount = _instanceCount;
        _mdiCmdBuffer[i].firstIndex = 0;
        _mdiCmdBuffer[i].baseVertex = i * vertexCount;
        _mdiCmdBuffer[i].baseInstance = i * _instanceCount;
    }

    
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

        for (auto j = 0; j < TRIPLE_BUFFER; j++)
        {
            auto index = j * _drawCount + i;

            _drawDataBuffer[index].m = mat;
            _drawDataBuffer[index].materialId = matId;
            _drawDataBuffer[index].pad0 = 0;
            _drawDataBuffer[index].pad1 = 0;
            _drawDataBuffer[index].pad2 = 0;
        }
    }
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

    initShader();
    initCamera();

    createTextures();
    createMaterials();
    createCubes();
    
    createDrawDataBuffer();
    
    createBuffers();
    fillBuffers();

    for (size_t i = 0; i < TRIPLE_BUFFER; i++)
    {
        _drawFences.push_back(GLsync());
    }

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

void updateMdiCmdBuffer()
{
    for (uint i = 0; i < _objectCount; i++)
       _mdiCmdBuffer[i].firstIndex = _drawRange * _mdiCmdBuffer[i].count;
}

void updateModelMatricesBuffer()
{
    for (auto i = _drawRange * _drawCount; i < _drawCount * (_drawRange  + 1); i++)
            _drawDataBuffer[i].m[3][1] += height;

    if (isDecreasingHeight)
        height -= 0.0001f;
    else
        height += 0.0001f;

    if (height >= 0.01f)
        isDecreasingHeight = true;
    else if (height <= -0.01f)
        isDecreasingHeight = false;
}

void update()
{
    if (_camera == nullptr)
        return;

    auto x = glm::cos(t);
    auto z = glm::sin(t);

    t += i;

    //auto pos = glm::vec3(x, 0.2f, z) * 70.0f;
    //_camera->setPosition(pos);
    _camera->update();

    _viewMatrix = glm::lookAt<float>(_camera->getPosition(), _camera->getTarget(), _camera->getUp());

    updateModelMatricesBuffer();
    updateMdiCmdBuffer();
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _shader->getUniform(0).set(_projectionMatrix * _viewMatrix);

    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)0, _objectCount, 0);

    _lastDrawRange = _drawRange;
    _drawRange = ++_drawRange % TRIPLE_BUFFER;
}

void lock()
{
    _drawFences[_lastDrawRange] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
}

void waitLock()
{
    glClientWaitSync(_drawFences[_drawRange], GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
    glDeleteSync(_drawFences[_drawRange]);
}

void loop()
{
    while (_isRunning)
    {
        input();

        waitLock();

        update();

        stopwatch::MeasureInMilliseconds([&]
        {
            render();
            lock();
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

    glDeleteBuffers(1, &_positionsBufferId);
    glDeleteBuffers(1, &_texCoordsBufferId);
    glDeleteBuffers(1, &_modelMatricesBufferId);
    glDeleteBuffers(1, &_drawIndexBufferId);
    glDeleteBuffers(1, &_indicesBufferId);
    glDeleteBuffers(1, &_mdiCmdBufferId); 
    glDeleteBuffers(1, &_materialsBufferId);

    delete [] _positionsBuffer;
    delete[] _texCoordsBuffer;
    delete[] _normalsBuffer;
    delete[] _drawIndexBuffer;
    delete[] _modelMatricesBuffer;
    delete[] _indicesBuffer;
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

    if (!init())
        return -1;

    _shader->bind();

    loop();

    _shader->unbind();

    release();

    return 0;
}