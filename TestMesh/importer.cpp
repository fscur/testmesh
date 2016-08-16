#include "importer.h"
#include "material.h"
#include "program.h"
#include "programBuilder.h"
#include "path.h"

#define TINYGLTF_LOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "tiny_gltf_loader.h"

using namespace tinygltf;

Scene* importGltf(std::string path)
{
    Scene* scene = nullptr;
    TinyGLTFLoader loader;
    std::string error = "";

    bool ret = loader.LoadASCIIFromFile(scene, &error, path);

    if (!error.empty())
    {
        printf("Error: %s\n", error.c_str());
        __debugbreak();
    }

    if (!ret)
    {
        printf("Failed to parse glTF\n");
        __debugbreak();
    }

    return scene;
}

void importer::importScene(std::string path)
{
    auto scene = importGltf(path);
}

material* importer::importDefaultMaterial(std::string path)
{
    auto directory = path::getDirectoryFullName(path);
    auto vertName = directory + "\\defaultVert.vert";
    auto fragName = directory + "\\defaultFrag.frag";

    auto program = programBuilder::buildProgram(vertName, fragName);

    std::vector<techniqueUniform> uniforms =
    {
        techniqueUniform("modelViewMatrix", "u_modelViewMatrix"),
        techniqueUniform("projectionMatrix", "u_projectionMatrix"),
        techniqueUniform("emission", "u_emission")
    };

    std::vector<techniqueAttribute> attributes =
    {
        techniqueAttribute("position", "a_position")
    };

    std::vector<long> enabledStates = { 2884, 2929 };

    auto technique = new shadingTechnique(uniforms, attributes, enabledStates, program);

    technique->addParameter("modelViewMatrix", new techniqueParameter(35676, parameterSemantic::MODELVIEW));
    technique->addParameter("projectionMatrix", new techniqueParameter(35676, parameterSemantic::PROJECTION));
    technique->addParameter("emission", new techniqueParameter(35666));
    technique->addParameter("position", new techniqueParameter(35665, parameterSemantic::POSITION));

    auto mat = new material("Effect1", technique);
    mat->addValue("emission", glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));

    return mat;
}
