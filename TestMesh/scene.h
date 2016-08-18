#pragma once
#include "mesh.h"
#include "camera.h"

#include <vector>

class scene
{
private:
    std::vector<const mesh*> _meshes;
    camera* _camera;

    float _width;
    float _height;

public:
    scene(float width, float height);
    ~scene();

    void render();

    void add(const mesh* mesh);

    camera* getCamera() { return _camera; }
};

