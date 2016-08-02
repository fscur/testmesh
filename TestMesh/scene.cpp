#include "scene.h"

scene::scene()
{
}

scene::~scene()
{
}

void scene::render()
{
    for (auto& mesh : _meshes)
    {
        mesh->render();
    }
}

void scene::add(const mesh* mesh)
{
    _meshes.push_back(mesh);
}
