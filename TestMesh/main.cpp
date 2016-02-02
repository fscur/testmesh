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

const uint TRIPPLE_BUFFER = 3;

std::vector<geometry*> _geometries;
std::vector<glm::mat4> _modelMatrices = std::vector<glm::mat4>();

std::vector<material*> _materialsLibrary;
std::vector<materialData> _materialsBuffer;
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
uint _interleavedVBOId = 0;
uint _texCoordsBufferId = 0;
uint _drawIndexBufferId = 0;
uint _modelMatricesBufferId = 0;
uint _indicesBufferId = 0;
uint _mdiCmdBufferId = 0;
uint _materialsBufferId = 0;

uint _interleavedVBOSize = 0;
uint _texCoordsBufferSize = 0;
uint _drawIndexBufferSize = 0;
uint _indicesBufferSize = 0;
uint _modelMatricesBufferSize = 0;
uint _mdiCmdBufferSize = 0;

float* _interleavedVBO;
float* _normalsBuffer;
uint* _drawIndexBuffer;
glm::mat4* _modelMatricesBuffer;
uint* _indicesBuffer;
mdiCmd* _mdiCmdBuffer;

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

float height = 0.0f;
bool isDecreasingHeight = false;
uint _drawRange = 0;
bufferLockManager _bufferLockManager(true);

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
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);

    _glContext = SDL_GL_CreateContext(_window);

    if(!_glContext)
    {
        LOG("Could not create context:" << SDL_GetError());
        return false;
    }

    glewExperimental = GL_TRUE;

    GLenum glewInitStatus = glewInit();

    if(glewInitStatus != GLEW_OK)
    {
        LOG("Error" << glewGetErrorString(glewInitStatus))
            return false;
    }

    //SDL_GL_SetSwapInterval(0);

    return true;
}

bool initGL()
{
    glClearColor(0.0f, 0.0f, 0.0f, 0.5f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    return glGetError() != GL_FALSE;
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

void createModelMatrices()
{
    for(auto v = 0; v < _drawCount; v++)
    {
        auto x = randf(-0.5f, 0.5f) * 10.0f;
        auto y = randf(-0.5f, 0.5f) * 10.0f;
        auto z = randf(-0.5f, 0.5f) * 10.0f;

        auto mat = glm::mat4(
            1.0f, 0.0f, 0.0f, 0.0f,
            0.0f, 1.0f, 0.0f, 0.0f,
            0.0f, 0.0f, 1.0f, 0.0f,
            x, y, z, 1.0f);

        _modelMatrices.push_back(mat);
    }
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
    for(auto i = 0; i < _materialsCount; i++)
    {
        auto r = rand() % _texturesCount;

        _materialsLibrary.push_back(createMaterial(_diffuseTextures[r], _normalTextures[r]));
    }

    for(auto i = 0; i < _materialsCount; i++)
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

    auto id = _shader->getId();

}

void createTextures(uint n)
{
    for(auto i = 0; i < n; i++)
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
    _camera->setPosition(glm::vec3(0.0f, 3.0f, 10.0f));
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
    auto tripleBufferObjectCount = _objectCount * TRIPPLE_BUFFER;
    auto indicesCount = _indices.size();

    _interleavedVBOSize = _vertices.size() * 5 * tripleBufferObjectCount;
    _drawIndexBufferSize = _drawCount * TRIPPLE_BUFFER;
    _indicesBufferSize = indicesCount * tripleBufferObjectCount;

    _interleavedVBO = new float[_interleavedVBOSize];
    _indicesBuffer = new uint[_indicesBufferSize];
    _drawIndexBuffer = new uint[_drawIndexBufferSize];

    int index = -1;

    for(uint i = 0; i < tripleBufferObjectCount; i++)
    {
        for(auto vertex : _vertices)
        {
            auto position = vertex.GetPosition();
            auto texCoord = vertex.GetTexCoord();

            _interleavedVBO[++index] = position.x;
            _interleavedVBO[++index] = position.y;
            _interleavedVBO[++index] = position.z;
            _interleavedVBO[++index] = texCoord.x;
            _interleavedVBO[++index] = texCoord.y;
        }
    }

    for(uint i = 0; i < _drawIndexBufferSize; i++)
        _drawIndexBuffer[i] = rand() % _materialsCount;

    for(uint i = 0; i < tripleBufferObjectCount; i++)
    {
        for(uint j = 0; j < indicesCount; j++)
        {
            auto index = i * indicesCount + j;
            _indicesBuffer[index] = _indices[j];
        }
    }
}

void createNonPersistentBuffers()
{
    glCreateVertexArrays(1, &_vaoId);
    glBindVertexArray(_vaoId);

    auto materialBufferSize = sizeof(materialData) * _materialsCount;

    auto floatSize = sizeof(float);
    newNamedBufferData(GL_ARRAY_BUFFER, _interleavedVBOId, _interleavedVBOSize * sizeof(float), _interleavedVBO);
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, floatSize * 5, 0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, floatSize * 5, (void*) (floatSize * 3));
    //floatSize * 5: is the number in bytes for the next ocurrence in the buffer of the beggining of that particular attribute
    //(void*)(floatSize * 3): is the initial offset for the beggining of that particular attribute. Before the texCoords there are 3 floats (x,y,z) in the buffer

    newNamedBufferData(GL_ARRAY_BUFFER, _drawIndexBufferId, _drawIndexBufferSize * sizeof(uint), _drawIndexBuffer);
    glEnableVertexAttribArray(2);
    glVertexAttribIPointer(2, 1, GL_UNSIGNED_INT, 0, 0);
    glVertexAttribDivisor(2, 1);

    newNamedBufferData(GL_ELEMENT_ARRAY_BUFFER, _indicesBufferId, _indicesBufferSize * sizeof(uint), _indicesBuffer);

    newNamedBufferData(GL_SHADER_STORAGE_BUFFER, _materialsBufferId, materialBufferSize, &_materialsBuffer[0]);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, _materialsBufferId);
}

void createPersistentBuffers()
{
    _modelMatricesBufferSize = sizeof(glm::mat4) * _drawCount;
    _mdiCmdBufferSize = _objectCount * sizeof(mdiCmd);

    _modelMatricesBuffer = (glm::mat4*) newPersistentBuffer(GL_ARRAY_BUFFER, _modelMatricesBufferId, _modelMatricesBufferSize * TRIPPLE_BUFFER);
    for(unsigned int i = 0; i < 4; i++)
    {
        glEnableVertexAttribArray(3 + i);
        glVertexAttribPointer(3 + i, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (const GLvoid*) (sizeof(GLfloat) * i * 4));
        glVertexAttribDivisor(3 + i, 1);
    }

    _mdiCmdBuffer = (mdiCmd*) newPersistentBuffer(GL_DRAW_INDIRECT_BUFFER, _mdiCmdBufferId, _mdiCmdBufferSize);
}

void fillPersistentBuffers()
{
    memcpy(_modelMatricesBuffer, &*_modelMatrices.begin(), _modelMatrices.size() * sizeof(glm::mat4));;

    auto indexCount = _indices.size();
    auto vertexCount = _vertices.size();

    for(uint i = 0; i < _objectCount; i++)
    {
        _mdiCmdBuffer[i].count = indexCount;
        _mdiCmdBuffer[i].instanceCount = _instanceCount;
        _mdiCmdBuffer[i].firstIndex = 0;
        _mdiCmdBuffer[i].baseVertex = i * vertexCount;
        _mdiCmdBuffer[i].baseInstance = i * _instanceCount;
    }
}

bool init()
{
    if(SDL_Init(SDL_INIT_VIDEO) < 0)
        LOG("SDL could not initialize! SDL_Error: " << SDL_GetError());

    TTF_Init();
    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF);

    auto error = FT_Init_FreeType(&freeLib);

    if(error)
    {
        LOG("SDL could not initialize! SDL_Error: " << SDL_GetError());
    }

    if(!createGLWindow())
        return false;

    if(!initGL())
        return false;

    initShader();
    initCamera();

    createTextures(_texturesCount);
    createMaterials();
    createCubes();
    createModelMatrices();

    fillNonPersistentBuffers();
    createNonPersistentBuffers();

    createPersistentBuffers();
    fillPersistentBuffers();

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);

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

void updateMdiCmdBuffer()
{
    for(uint i = 0; i < _objectCount; i++)
        _mdiCmdBuffer[i].firstIndex = _drawRange * 36;
}

void updateModelMatricesBuffer()
{
    int bufferRange = _drawRange * _drawCount;
    int endOfBufferRange = bufferRange + _drawCount;

    stopwatch::MeasureInMilliseconds([&] {
        for(auto &matrix : _modelMatrices)
        {
            matrix[3][1] += height;
        }
    }, "update matrices");

    stopwatch::MeasureInMilliseconds([&] {
        memcpy(_modelMatricesBuffer + bufferRange, &*_modelMatrices.begin(), _modelMatrices.size() * sizeof(glm::mat4));
    }, "memcpy");

    if(isDecreasingHeight)
    {
        height -= 0.001f;
    }
    else
    {
        height += 0.001f;
    }

    if(height >= 0.05f)
    {
        isDecreasingHeight = true;
    }
    else if(height <= -0.05f)
    {
        isDecreasingHeight = false;
    }
}

void update()
{
    if(_camera == nullptr)
        return;

    _camera->update();

    _viewMatrix = glm::lookAt<float>(_camera->getPosition(), _camera->getTarget(), _camera->getUp());

    updateModelMatricesBuffer();
    updateMdiCmdBuffer();
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    _shader->getUniform(0).set(_projectionMatrix * _viewMatrix);

    glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, NULL, _objectCount, 0);

    _bufferLockManager.lockRange(_drawRange, 1);
    _drawRange = ++_drawRange % TRIPPLE_BUFFER;
}

void loop()
{
    while(_isRunning)
    {
        input();

        _bufferLockManager.waitForLockedRange(_drawRange, 1);
        update();

        stopwatch::MeasureInMilliseconds([&]
        {
            render();
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

    glDeleteBuffers(1, &_interleavedVBOId);
    glDeleteBuffers(1, &_texCoordsBufferId);
    glDeleteBuffers(1, &_modelMatricesBufferId);
    glDeleteBuffers(1, &_drawIndexBufferId);
    glDeleteBuffers(1, &_indicesBufferId);
    glDeleteBuffers(1, &_mdiCmdBufferId);
    glDeleteBuffers(1, &_materialsBufferId);

    delete[] _interleavedVBO;
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

    if(!init())
        return -1;

    _shader->bind();

    loop();

    _shader->unbind();

    release();

    return 0;
}