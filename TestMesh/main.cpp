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
#include "lineMesh.h"
#include "parametricData.h"
#include "parasolid.h"
#include "ray.h"

#include <SDL\SDL.h>
#include <SDL\SDL_TTF.h>
#include <SDL\SDL_image.h>
#include <glm\gtc\type_ptr.hpp>
#include <rapidjson\rapidjson.h>

#include <freetype\ft2build.h>
#include FT_FREETYPE_H

#include <algorithm>
#include <memory>
#include <functional>
#include <limits>
extern "C" {
    _declspec(dllexport) DWORD NvOptimusEnablement = 0x00000001;
}

SDL_Window* _window;
SDL_GLContext _glContext;

float _width;
float _height;
float _aspect;
float _fov = glm::half_pi<float>() * 0.5f;
float _zNear = 0.1f;
float _zFar = 100.0f;

std::vector<geometry*> _geometries;
std::vector<glm::mat4> _modelMatrices;
shader* _shader;
shader* _lineShader;

glm::mat4 _projectionMatrix;
glm::mat4 _viewMatrix;

camera* _activeCamera;

ray _ray;

bool _isRunning = true;
bool _isLeftButtonPressed = false;
bool _isRightButtonPressed = false;
bool _isMiddleButtonPressed = false;
bool _rotating;

glm::vec2 _mouseDownPos;
glm::vec2 _lastMousePos;
glm::vec3 _cameraPos;

std::vector<GLuint> _selectedTriangles;

float _rotationSpeed = 0.01f;
float zoomOffset = 0.0f;
float zoomTarget = 0.0f;
float t = 0.0f;
float i = 0.01f;
parametricData* _data;
parasolid* _parasolid;
PK_BODY_t _body;

lineMesh* _rayMesh;

float randf(float fMin, float fMax)
{
    float f = (double)rand() / RAND_MAX;
    return fMin + f * (fMax - fMin);
}

bool createGLWindow()
{
    _width = 1024 * 2;
    _height = 768 * 2;
    _aspect = _width / _height;

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

    //SDL_GL_SetSwapInterval(0);

    return true;
}

bool initGL()
{
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);

    return glGetError() != GL_FALSE;
}

void initShader()
{
    _shader = new shader("SHADER", "shader.vert", "shader.frag");

    _shader->addAttribute("inPosition");
    _shader->addAttribute("inTexCoord");
    _shader->addAttribute("inNormal");
    _shader->addAttribute("inModelMatrix");

    _shader->init();
    _shader->addUniform("vp", 0);
    _shader->addUniform("diffuseTextureArrayIndex", 1);
    _shader->addUniform("diffuseTexturePageIndex", 2);
    _shader->addUniform("albedoTexture", 3);
    //_shader->addUniform("selectedTriangles", 4);
    _shader->addUniform("circlePos", 4);
    _shader->addUniform("circleRad", 5);
    _shader->addUniform("circleColorA", 6);
    _shader->addUniform("circleColorB", 7);

    _lineShader = new shader("LINE SHADER", "line_shader.vert", "line_shader.frag");
    _lineShader->addAttribute("inPosition");

    _lineShader->init();
    _lineShader->addUniform("vp", 0);
}

void initCamera()
{
    _activeCamera = new camera();
    _activeCamera->setPosition(glm::vec3(0.0f, 0.0f, 10.0f));
    _activeCamera->setTarget(glm::vec3(0.0f));

    _projectionMatrix = glm::perspective<float>(_fov, _aspect, _zNear, _zFar);
    _viewMatrix = glm::lookAt<float>(_activeCamera->getPosition(), _activeCamera->getTarget(), _activeCamera->getUp());
}

void initParametricData()
{
    _data = parametricData::load("D:\\Part1.json");
}

void initParasolid()
{
    _parasolid = new parasolid();
    _body = _parasolid->loadX_TFile("D:\\Part1.x_t");

    auto modelMatrix = glm::mat4(1.0);
    _modelMatrices.push_back(modelMatrix);

    std::vector<vertex> vertices;
    std::vector<uint> indices;
    _parasolid->createGeometry(_body, vertices, indices);

    _geometries.push_back(geometry::create(vertices, indices, _modelMatrices));
}

bool init()
{
    if (SDL_Init(SDL_INIT_VIDEO) < 0)
        LOG("SDL could not initialize! SDL_Error: " << SDL_GetError());

    IMG_Init(IMG_INIT_JPG | IMG_INIT_PNG | IMG_INIT_TIF);

    if (!createGLWindow())
        return false;

    if (!initGL())
        return false;

    initShader();
    initCamera();
    initParasolid();
    initParametricData();
    SDL_GL_SetSwapInterval(0);

    return true;
}

ray castRay(glm::vec2 mousePos)
{
    auto x = (2.0f * mousePos.x) / _width - 1.0f;
    auto y = 1.0f - (2.0f * mousePos.y) / _height;

    auto invPersp = glm::inverse(_projectionMatrix);
    auto invView = glm::inverse(_viewMatrix);

    auto ray_clip = glm::vec4(x, y, -1.0f, 1.0f);
    auto ray_eye = invPersp * ray_clip;
    ray_eye = glm::vec4(ray_eye.x, ray_eye.y, -1.0f, 0.0f);
    auto ray_world = glm::dvec3(invView * ray_eye);
    ray_world = glm::normalize(ray_world);

    _ray = ray(_activeCamera->getPosition(), ray_world);

    _rayMesh = lineMesh::create("ray", std::vector<glm::vec3> { _ray.origin, _ray.origin + _ray.direction * 100.0}, std::vector<GLuint>{0, 1});

    return _ray;
}

collision _offsetCollision;
bool _doingOffset = false;

int lineLineIntersect(glm::vec3 p1, glm::vec3 p2, glm::vec3 p3, glm::vec3 p4, glm::vec3 *pa, glm::vec3 *pb, float *mua, float *mub)
{
    glm::vec3 p13, p43, p21;
    float d1343, d4321, d1321, d4343, d2121;
    float numer, denom;
    float epsilon = glm::epsilon<float>();

    p13.x = p1.x - p3.x;
    p13.y = p1.y - p3.y;
    p13.z = p1.z - p3.z;
    p43.x = p4.x - p3.x;
    p43.y = p4.y - p3.y;
    p43.z = p4.z - p3.z;
    if (glm::abs(p43.x) < epsilon && glm::abs(p43.y) < epsilon && glm::abs(p43.z) < epsilon)
        return false;
    p21.x = p2.x - p1.x;
    p21.y = p2.y - p1.y;
    p21.z = p2.z - p1.z;
    if (glm::abs(p21.x) < epsilon && glm::abs(p21.y) < epsilon && glm::abs(p21.z) < epsilon)
        return false;

    d1343 = p13.x * p43.x + p13.y * p43.y + p13.z * p43.z;
    d4321 = p43.x * p21.x + p43.y * p21.y + p43.z * p21.z;
    d1321 = p13.x * p21.x + p13.y * p21.y + p13.z * p21.z;
    d4343 = p43.x * p43.x + p43.y * p43.y + p43.z * p43.z;
    d2121 = p21.x * p21.x + p21.y * p21.y + p21.z * p21.z;

    denom = d2121 * d4343 - d4321 * d4321;
    if (glm::abs(denom) < epsilon)
        return false;

    numer = d1343 * d4321 - d1321 * d4343;

    *mua = numer / denom;
    *mub = (d1343 + d4321 * (*mua)) / d4343;

    pa->x = p1.x + *mua * p21.x;
    pa->y = p1.y + *mua * p21.y;
    pa->z = p1.z + *mua * p21.z;
    pb->x = p3.x + *mub * p43.x;
    pb->y = p3.y + *mub * p43.y;
    pb->z = p3.z + *mub * p43.z;

    return true;
}

float getOffsetDistance(glm::vec2 mousePos, glm::vec3 startPos, glm::vec3 normal)
{
    auto worldToScreen = [&](glm::vec3 worldPos)
    {
        auto vp = _projectionMatrix * _viewMatrix;
        auto vw = _width * 0.5f;
        auto vh = _height * 0.5f;
        auto v = glm::vec2(vw, vh);
        auto pos = glm::vec4(worldPos, 1.0);
        pos = vp * pos;
        pos = pos / pos.w;
        return glm::vec2(pos.x, -pos.y) * v + v;
    };

    auto screenToViewZNear = [&](glm::vec2 mousePos)
    {
        auto zNear = _zNear;
        auto aspect = _aspect;
        auto fov = _fov;

        auto tg = glm::tan(fov * 0.5f) * zNear;

        auto h = _height;
        auto w = _width;

        auto hh = h * 0.5f;
        auto hw = w * 0.5f;

        auto ys = mousePos.y - hh;
        auto yp = ys / hh;
        auto y = -(yp * tg);

        auto xs = mousePos.x - hw;
        auto xp = xs / hw;
        auto x = xp * tg * aspect;

        return glm::vec3(x, y, -zNear);
    };

    auto screenObjectPos = worldToScreen(startPos);
    auto screenObjectDir = glm::normalize(worldToScreen(startPos + normal) - screenObjectPos);
    auto mouseStartToEnd = glm::vec2(mousePos.x, mousePos.y) - screenObjectPos;
    auto projMouseIntoScreenObjectDir = glm::dot(mouseStartToEnd, screenObjectDir);
    auto screenObjectEndPos = screenObjectPos + screenObjectDir * projMouseIntoScreenObjectDir;// -glm::dot(_mouseDownPos - screenObjectPos, screenObjectDir));
    auto viewObjectEndPos = screenToViewZNear(screenObjectEndPos);

    auto worldToView = [&](const glm::vec3 vec)
    {
        auto vec4 = glm::vec4(vec.x, vec.y, vec.z, 1.0f);
        vec4 = _viewMatrix * vec4;
        return glm::vec3(vec4.x, vec4.y, vec4.z);
    };

    auto viewObjectStartPos = worldToView(startPos);
    auto viewObjectStartDirPos = worldToView(startPos + normal);

    glm::vec3 p1, p2;
    float t1, t2;

    if (lineLineIntersect(glm::vec3(), viewObjectEndPos, viewObjectStartPos, viewObjectStartDirPos, &p1, &p2, &t1, &t2))
        return t2;

    return 0.0f;
}

void mouseDown(SDL_Event e)
{
    if (e.button.button == SDL_BUTTON_LEFT)
        _isLeftButtonPressed = true;
    if (e.button.button == SDL_BUTTON_RIGHT)
        _isRightButtonPressed = true;
    if (e.button.button == SDL_BUTTON_MIDDLE)
        _isMiddleButtonPressed = true;

    _mouseDownPos = glm::vec2(e.motion.x, e.motion.y);

    if (_isLeftButtonPressed)
    {
        auto ray = castRay(_mouseDownPos);

        _doingOffset = _parasolid->getFace(_body, ray, _offsetCollision);
    }

    /*if (_isLeftButtonPressed || _isMiddleButtonPressed)
    initOffset();

    if (_isRightButtonPressed)
    addPolygonPoint();*/
}

void mouseMove(SDL_Event e)
{
    glm::vec2 mousePos = glm::vec2(e.motion.x, e.motion.y);

    if (_isLeftButtonPressed & _doingOffset)
    {
        auto offsetDist = getOffsetDistance(mousePos, _offsetCollision.worldPos, _offsetCollision.worldNormal);

        //auto currentDim = 

        //std::cout << std::to_string(offsetDist) << std::endl;

        auto entity = _data->entities[_offsetCollision.parasolidId];
        auto dimensions = _data->entityDimensions[entity];
        auto dim = dimensions[0];
        auto currentDim = dim->value;
        auto discreteValues = dim->discreteValues;
        auto discreteValuesCount = discreteValues.size();
        auto closestValue = currentDim;
        offsetDist += currentDim;

        if (discreteValuesCount > 0)
        {
            auto minDist = std::abs(discreteValues[0] - offsetDist);

            for (size_t i = 1; i < discreteValuesCount; i++)
            {
                auto dist = std::abs(discreteValues[i] - offsetDist);

                if (dist < minDist)
                {
                    closestValue = discreteValues[i];
                    minDist = dist;
                }
            }
        }

        std::cout << std::to_string(closestValue) << std::endl;
    }

    if (!_isMiddleButtonPressed)
    {
        _lastMousePos = mousePos;
        return;
    }

    if (_isMiddleButtonPressed && length(_mouseDownPos - _lastMousePos) > 5)
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

    _activeCamera->orbit(_activeCamera->getTarget(), _activeCamera->getUp(), dx);
    _activeCamera->orbit(_activeCamera->getTarget(), _activeCamera->getRight(), dy);

    _lastMousePos = mousePos;
}

void mouseUp(SDL_Event e)
{
    _rotating = false;
    _doingOffset = false;
    _isLeftButtonPressed = false;
    _isMiddleButtonPressed = false;
    _isRightButtonPressed = false;
}

void mouseWheel(SDL_Event e)
{
    zoomTarget += (e.wheel.y);
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
                else if (e.key.keysym.sym == SDLK_c)
                {

                    //std::vector<PK_CURVE_t> lines;
                    //std::vector<PK_INTERVAL_t> intervals;

                    //for (auto i = 0; i < _polygonPoints.size(); i++)
                    //{
                    //    PK_VECTOR_t current = _polygonPoints[i];
                    //    PK_VECTOR_t next = _polygonPoints[(i + 1) % _polygonPoints.size()];

                    //    auto currentVec = glm::dvec3(current.coord[0], current.coord[1], current.coord[2]);
                    //    auto nextVec = glm::dvec3(next.coord[0], next.coord[1], next.coord[2]);
                    //    auto diffVec = nextVec - currentVec;
                    //    auto distance = (double)glm::length(diffVec);

                    //    PK_LINE_sf_t line_sf;
                    //    PK_VECTOR_t diff = { diffVec.x, diffVec.y, diffVec.z };
                    //    PK_VECTOR1_t axis;
                    //    PK_VECTOR_normalise(diff, &axis);

                    //    line_sf.basis_set.location = current;
                    //    line_sf.basis_set.axis = axis;

                    //    PK_LINE_t line;
                    //    PK_LINE_create(&line_sf, &line);
                    //    lines.push_back(line);
                    //    intervals.push_back({ 0.0, distance });
                    //}

                    //_polygonPoints.clear();
                    //_polygonMesh = nullptr;
                    //int er;

                    //PK_BODY_t body;
                    //PK_FACE_t face;
                    //if (_polygonFace == -1)
                    //{
                    //    PK_CURVE_make_wire_body_o_t make_wire_body_options;
                    //    PK_CURVE_make_wire_body_o_m(make_wire_body_options);
                    //    make_wire_body_options.want_edges = PK_LOGICAL_true;
                    //    int n_edges;
                    //    PK_EDGE_t* wire_edges;
                    //    int* edge_index;
                    //    er = PK_CURVE_make_wire_body_2(lines.size(), lines.data(), intervals.data(), &make_wire_body_options, &body, &n_edges, &wire_edges, &edge_index);
                    //    std::cout << "make_wire_body" << std::endl;
                    //    checkError(er);

                    //    const PK_LOGICAL_t senses[2] = { PK_LOGICAL_true, PK_LOGICAL_true };
                    //    const int shared_loop[2] = { -1, -1 };
                    //    er = PK_EDGE_make_faces_from_wire(1, &wire_edges[0], senses, shared_loop, &face);
                    //    std::cout << "make_faces_from_wire" << std::endl;
                    //    checkError(er);

                    //    PK_LOGICAL_t local_check = PK_LOGICAL_true;
                    //    PK_local_check_t local_check_result;
                    //    er = PK_FACE_attach_surf_fitting(face, local_check, &local_check_result);
                    //    std::cout << "attach_surf_fitting" << std::endl;
                    //    checkError(er);
                    //}
                    //else
                    //{
                    //    PK_FACE_imprint_curves_o_t face_imprint_curves_opt;
                    //    PK_FACE_imprint_curves_o_m(face_imprint_curves_opt);
                    //    PK_ENTITY_track_r_t track;
                    //    er = PK_FACE_imprint_curves_2(_polygonFace, lines.size(), lines.data(), intervals.data(), &face_imprint_curves_opt, &track);
                    //    std::cout << "imprint_curves" << std::endl;
                    //    checkError(er);

                    //    auto edges = std::vector<PK_EDGE_t>();
                    //    for (auto i = 0; i < track.n_track_records; i++)
                    //    {
                    //        auto record = track.track_records[i];
                    //        if (record.track == PK_ENTITY_track_split_c)
                    //        {
                    //            if (record.n_product_entities > 0)
                    //                face = record.product_entities[0];
                    //        }
                    //    }

                    //    PK_FACE_ask_body(face, &body);
                    //}

                    //if (_polygonFace == -1)
                    //{
                    //    PK_VECTOR1_t normal = getFaceNormal(face);
                    //    PK_VECTOR_t sweep = { normal.coord[0] * 0.000001, normal.coord[1] * 0.000001, normal.coord[2] * 0.000001 };
                    //    int n_laterals;
                    //    PK_TOPOL_t *laterals, *bases;
                    //    PK_local_check_t check_result;
                    //    //if (_polygonFace == -1)
                    //    er = PK_BODY_sweep(body, sweep, PK_LOGICAL_true, &n_laterals, &laterals, &bases, &check_result);
                    //    //else
                    //    //    er = PK_FACE_sweep(1, &face, sweep, PK_LOGICAL_true, &n_laterals, &laterals, &bases, &check_result);
                    //    std::cout << "sweep" << std::endl;
                    //    checkError(er);

                    //    _bodies.push_back(body);
                    //    auto geometry = createGeometry(body);
                    //    _geometries.push_back(geometry);
                    //    _bodyGeometries[body] = geometry;
                    //}
                    //else
                    //    updateGeometry(body, _bodyGeometries[body]);
                }
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
    if (_activeCamera == nullptr)
        return;
    
    auto pos = _activeCamera->getPosition();
    auto dir = _activeCamera->getDirection();

    zoomOffset = zoomTarget * 0.005;
    zoomTarget -= zoomOffset;

    if (std::abs(zoomTarget) < 0.001)
    {
        zoomTarget = 0.0f;
        zoomOffset = 0.0f;
    }

    pos += dir * zoomOffset;
    _activeCamera->setPosition(pos);

    auto target = pos + dir;

    _projectionMatrix = glm::perspective<float>(_fov, _aspect, _zNear, _zFar);
    _viewMatrix = glm::lookAt<float>(pos, target, _activeCamera->getUp());
}

void render()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    for (auto geom : _geometries)
    {
        _shader->bind();
        _shader->getUniform(0).set(_projectionMatrix * _viewMatrix);
        geom->bind();
        geom->render(1, _modelMatrices);
        geom->unbind();
        _shader->unbind();
    }
    if (_rayMesh != nullptr)
    {
        _lineShader->bind();
        _lineShader->getUniform(0).set(_projectionMatrix * _viewMatrix);
        _rayMesh->bind();
        _rayMesh->render();
        _rayMesh->unbind();
        _lineShader->unbind();
    }
    /*if (_polygonMesh != nullptr)
    {
        _lineShader->bind();
        _polygonMesh->bind();
        _lineShader->getUniform(0).set(_projectionMatrix * _viewMatrix);
        _polygonMesh->render();
        _polygonMesh->unbind();
        _lineShader->unbind();
    }*/
}

void loop()
{
    while (_isRunning)
    {
        input();
        update();

        render();
        SDL_GL_SwapWindow(_window);
    }
}

void release()
{
    SDL_GL_DeleteContext(_glContext);
    _glContext = NULL;

    SDL_DestroyWindow(_window);
    _window = NULL;

    IMG_Quit();
    SDL_Quit();

    delete _shader;
    delete _lineShader;
    delete _activeCamera;
    delete _parasolid;
}

int main(int argc, char* args[])
{
    if (!init())
        return -1;

    loop();

    release();

    return 0;
}