#pragma once

#include "mesh.h"

#include <vector>

class scene
{
private:
    std::vector<const mesh*> _meshes;

public:
    scene();
    ~scene();

    void render();

    void add(const mesh* mesh);
};

