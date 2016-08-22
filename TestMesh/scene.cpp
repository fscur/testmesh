#include "scene.h"

#include <glm\gtc\type_ptr.hpp>
#include <glm\gtc\matrix_transform.hpp>

scene::scene(float width, float height) :
    _width(width),
    _height(height)
{
    _camera = new camera(width, height);
    _camera->setPosition(glm::vec3(0.0f, 3.0f, 5.0f));
    _camera->setTarget(glm::vec3(0.0f));
}

scene::~scene()
{
}

void scene::render()
{
    auto modelMatrix = glm::mat4(
        1.0f, 0.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 0.0f, 0.0f, 1.0f);

    auto view = _camera->getView();
    auto projection = _camera->getProjection();

    for (auto& material : _materials)
    {
        auto modelView = view *  modelMatrix;
        auto modelViewInverseTranspose = glm::mat3(glm::transpose(glm::inverse(modelView)));

        material->bindSemantic(parameterSemantic::MODELVIEW, modelView);
        material->bindSemantic(parameterSemantic::PROJECTION, projection);
        material->bindSemantic(parameterSemantic::MODELVIEWINVERSETRANSPOSE, modelViewInverseTranspose);

        material->render();
    }
}

void scene::add(material* material)
{
    _materials.push_back(material);
}
