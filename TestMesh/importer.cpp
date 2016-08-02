#include "importer.h"

#define TINYGLTF_LOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "tiny_gltf_loader.h"

using namespace tinygltf;

void importer::import(std::string path)
{
    Scene scene;
    TinyGLTFLoader loader;
    std::string error = "";

    bool ret = loader.LoadASCIIFromFile(&scene, &error, path);

    if (!error.empty()) 
    {
        printf("Error: %s\n", error.c_str());
    }

    if (!ret)
    {
        printf("Failed to parse glTF\n");
    }
}
