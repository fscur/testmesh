#pragma once
#include "mesh.h"
#include "camera.h"

#include <vector>

class scene
{
private:
    std::vector<material*> _materials;
    camera* _camera;

    float _width;
    float _height;

public:
    scene(float width, float height);
    ~scene();

    void render();

    void add(material* material);

    camera* getCamera() { return _camera; }
};

