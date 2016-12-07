#include "importer.h"
#include "material.h"
#include "program.h"
#include "programBuilder.h"
#include "path.h"
#include "mesh.h"
#include "vertex.h"
#include "window.h"
#include "vertexArrayObject.h"
#include "gltfUtils.h"
#include "vertexBufferObject.h"
#include "elementBufferObject.h"
#include "primitive.h"
#include "shadingTechnique.h"

#define TINYGLTF_LOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "tiny_gltf_loader.h"
#include "../inc/glm/vec3.hpp"

#include <vector>

using namespace tinygltf;

parameterSemantic semanticFromString(std::string semantic)
{
    if (semantic == "MODELVIEW")
        return parameterSemantic::MODELVIEW;
    if (semantic == "NORMAL")
        return parameterSemantic::NORMAL;
    if (semantic == "MODELVIEWINVERSETRANSPOSE")
        return parameterSemantic::MODELVIEWINVERSETRANSPOSE;
    if (semantic == "POSITION")
        return parameterSemantic::POSITION;
    if (semantic == "PROJECTION")
        return parameterSemantic::PROJECTION;
    if (semantic == "TEXCOORD_0")
        return parameterSemantic::TEXCOORD;
}

Scene* importGltf(std::string path)
{
    Scene* scene = new Scene();
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

primitive* primitiveFromGltfPrimitive(tinygltf::Primitive gltfPrimitive, tinygltf::Scene* scene)
{
    auto indicesAcessorId = gltfPrimitive.indices;

    //indices
    auto indicesAcessor = scene->accessors.find(indicesAcessorId)->second;
    auto indicesBufferView = scene->bufferViews.find(indicesAcessor.bufferView)->second;
    auto indicesBuffer = scene->buffers.find(indicesBufferView.buffer)->second;

    auto indicesOffset = indicesAcessor.byteOffset + indicesBufferView.byteOffset;
    auto indicesByteLength = indicesBufferView.byteLength;
    auto indicesDataPtr = &indicesBuffer.data[indicesOffset];

    std::unordered_map<parameterSemantic, vertexBufferObject*> vbos;

    for (auto& attribute : gltfPrimitive.attributes)
    {
        auto semantic = semanticFromString(attribute.first);

        auto attributeAcessor = scene->accessors.find(attribute.second)->second;
        auto attributeBufferView = scene->bufferViews.find(attributeAcessor.bufferView)->second;
        auto attributeBuffer = scene->buffers.find(attributeBufferView.buffer)->second;

        auto attributeOffset = attributeAcessor.byteOffset + attributeBufferView.byteOffset;
        auto attributeByteLength = attributeBufferView.byteLength;
        auto attributeDataPtr = &attributeBuffer.data[attributeOffset];

        auto componentCount = attributeAcessor.type;
        auto componentSize = gltfUtils::getComponentSizeFromComponentType(attributeAcessor.componentType);
        auto attributeSize = componentCount * componentSize;
        auto bufferSize = attributeSize * attributeAcessor.count;

        vbos[semantic] = new vertexBufferObject(
            attributeBufferView.target,
            bufferSize,
            attributeDataPtr,
            attributeAcessor.componentType,
            attributeAcessor.byteStride);
    }

    auto ebo = new elementBufferObject(
        indicesBufferView.target,
        indicesBufferView.byteLength,
        indicesDataPtr,
        indicesAcessor.count,
        indicesAcessor.componentType);

    return new primitive(gltfPrimitive.mode, vbos, ebo);
}

program* programFromGltf(std::string programName, tinygltf::Scene* scene)
{
    auto gltfProgram = scene->programs.find(programName)->second;

    auto gltfVertexShader = scene->shaders.find(gltfProgram.vertexShader)->second;
    auto gltfFragmentShader = scene->shaders.find(gltfProgram.fragmentShader)->second;

    auto vertexSource = gltfVertexShader.source;
    auto fragmentSource = gltfFragmentShader.source;

    vertexSource.push_back('\0');
    fragmentSource.push_back('\0');

    return programBuilder::buildProgramFromSource((char*)(&vertexSource[0]), (char*)(&fragmentSource[0]));
}

material* materialFromGltf(std::string materialName, primitive* primitive, tinygltf::Scene* scene)
{
    auto gltfMaterial = scene->materials.find(materialName)->second;
    auto gltfTechnique = scene->techniques.find(gltfMaterial.technique)->second;

    std::vector<techniqueUniform> uniforms;
    std::vector<techniqueAttribute> attributes;
    std::vector<long> enableStates = { 2929, 2884 };

    for (auto& uniform : gltfTechnique.uniforms)
        uniforms.push_back(techniqueUniform(uniform.second, uniform.first));

    for (auto& attribute : gltfTechnique.attributes)
        attributes.push_back(techniqueAttribute(attribute.second, attribute.first));

    //for(auto& enableState : gltfTechnique.states) WHY GOD WHYYYYYYYYYYYYYY

    auto parameters = std::map<std::string, techniqueParameter*>();
    for (auto& parameter : gltfTechnique.parameters)
    {
        auto semantic = semanticFromString(parameter.second.semantic);
        parameters[parameter.first] = new techniqueParameter(parameter.second.type, semantic);
    }

    auto program = programFromGltf(gltfTechnique.program, scene);
    auto technique = new shadingTechnique(uniforms, attributes, parameters, enableStates, primitive, program);

    auto mat = new material(gltfMaterial.name, technique);

    for (auto& value : gltfMaterial.values)
    {
        auto stringValue = value.second.string_value;
        if (!stringValue.empty())
        {
            auto gltfTexture = scene->textures.find(stringValue)->second;
            auto sampler = scene->samplers.find(gltfTexture.sampler)->second;
            auto image = scene->images.find(gltfTexture.source)->second;

            //auto data = std::vector<glm::vec3>();

            //for (size_t i = 0; i < 211; i++)
            //{
            //    for (size_t j = 0; j < 211; j++)
            //    {
            //        data.push_back(glm::vec3(1.0, 1.0, 0.0));
            //    }
            //}

            GLuint texId;
            glGenTextures(1, &texId);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, texId);

            auto data = new glm::vec3(1.0f, 1.0f, 0.0f);

            glTexImage2D(GL_TEXTURE_2D, 0, gltfTexture.internalFormat, image.width, image.height, 0, gltfTexture.format, gltfTexture.type, &image.image[0]);

            auto tex = new texture(
                image.width,
                image.height,
                gltfTexture.target,
                gltfTexture.internalFormat,
                gltfTexture.format,
                gltfTexture.type,
                (byte*)(&image.image[0]),
                sampler.wrapR,
                sampler.minFilter,
                sampler.magFilter);

            mat->addValue(value.first, tex);
        }
        else
        {
            auto numberArray = value.second.number_array;

            if (numberArray.size() == 4)
            {
                mat->addValue(value.first, glm::vec4(numberArray[0], numberArray[1], numberArray[2], numberArray[3]));
            }
            else if (numberArray.size() == 3)
            {
                mat->addValue(value.first, glm::vec3(numberArray[0], numberArray[1], numberArray[2]));
            }
            else if (numberArray.size() == 1)
            {
                mat->addValue(value.first, numberArray[0]);
            }
        }
    }

    return mat;
}

scene* importer::importScene(std::string path)
{
    auto gltfScene = importGltf(path);

    auto materials = gltfScene->materials;
    auto techniques = gltfScene->techniques;

    auto s = new scene(window::_width, window::_height);

    for (auto& gltfMesh : gltfScene->meshes)
    {
        auto firstPrimitive = gltfMesh.second.primitives[0];

        auto primitive = primitiveFromGltfPrimitive(firstPrimitive, gltfScene);
        auto material = materialFromGltf(firstPrimitive.material, primitive, gltfScene);

        s->add(material);
    }

    return s;
}

material* importer::importDefaultMaterial()
{
    //auto vertName = "../TestMesh/defaultVert.vert";
    //auto fragName = "../TestMesh/defaultFrag.frag";

    //auto program = programBuilder::buildProgramFromFile(vertName, fragName);

    //std::vector<techniqueUniform> uniforms =
    //{
    //    techniqueUniform("modelViewMatrix", "u_modelViewMatrix"),
    //    techniqueUniform("projectionMatrix", "u_projectionMatrix"),
    //    techniqueUniform("emission", "u_emission")
    //};

    //std::vector<techniqueAttribute> attributes =
    //{
    //    techniqueAttribute("position", "a_position")
    //};

    //std::vector<long> enabledStates = { 2884, 2929 };

    //auto parameters = std::map<std::string, techniqueParameter*>();
    //parameters["modelViewMatrix"] = new techniqueParameter(35676, parameterSemantic::MODELVIEW);
    //parameters["projectionMatrix"] = new techniqueParameter(35676, parameterSemantic::PROJECTION);
    //parameters["emission"] = new techniqueParameter(35666);
    //parameters["position"] = new techniqueParameter(35665, parameterSemantic::POSITION);

    //auto technique = new shadingTechnique(uniforms, attributes, parameters, enabledStates, program);

    //auto mat = new material("Effect1", technique);
    //mat->addValue("emission", glm::vec4(0.8f, 0.8f, 0.8f, 1.0f));

    return nullptr;
}
