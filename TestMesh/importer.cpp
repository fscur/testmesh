#include "importer.h"
#include "material.h"
#include "program.h"
#include "programBuilder.h"
#include "path.h"
#include "mesh.h"
#include "vertex.h"
#include "window.h"
#include "vertexArrayObject.h"

#define TINYGLTF_LOADER_IMPLEMENTATION
#define STB_IMAGE_IMPLEMENTATION

#include "tiny_gltf_loader.h"

#include <vector>

using namespace tinygltf;

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

vertexArrayObject* vaoFromGltfPrimitive(tinygltf::Primitive gltfPrimitive, tinygltf::Scene* scene)
{
    auto verticesAcessorId = gltfPrimitive.attributes.find("POSITION")->second;
    auto normalsAcessorId = gltfPrimitive.attributes.find("NORMAL")->second;
    auto indicesAcessorId = gltfPrimitive.indices;

    //indices
    auto indicesAcessor = scene->accessors.find(indicesAcessorId)->second;
    auto indicesBufferView = scene->bufferViews.find(indicesAcessor.bufferView)->second;
    auto indicesBuffer = scene->buffers.find(indicesBufferView.buffer)->second;

    auto indicesOffset = indicesAcessor.byteOffset + indicesBufferView.byteOffset;
    auto indicesByteLength = indicesBufferView.byteLength;
    auto indicesDataPtr = (short*)&indicesBuffer.data[indicesOffset];

    //normals
    auto normalsAcessor = scene->accessors.find(normalsAcessorId)->second;
    auto normalsBufferView = scene->bufferViews.find(normalsAcessor.bufferView)->second;
    auto normalsBuffer = scene->buffers.find(normalsBufferView.buffer)->second;

    auto normalsOffset = normalsAcessor.byteOffset + normalsBufferView.byteOffset;
    auto normalsByteLength = normalsBufferView.byteLength;
    auto normalsDataPtr = (glm::vec3*)&normalsBuffer.data[normalsOffset];

    //vertices
    auto verticesAcessor = scene->accessors.find(verticesAcessorId)->second;
    auto verticesBufferView = scene->bufferViews.find(verticesAcessor.bufferView)->second;
    auto verticesBuffer = scene->buffers.find(verticesBufferView.buffer)->second;

    auto verticesOffset = verticesAcessor.byteOffset + verticesBufferView.byteOffset;
    auto verticesByteLength = verticesBufferView.byteLength;
    auto verticesDataPtr = (glm::vec3*)&verticesBuffer.data[verticesOffset];

    GLuint currentVertexAttrib = 0;
    GLuint vao;
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    GLuint verticesVbo;
    glGenBuffers(1, &verticesVbo);
    glBindBuffer(verticesBufferView.target, verticesVbo);
    glBufferData(verticesBufferView.target, verticesAcessor.byteStride * verticesAcessor.count, verticesDataPtr, GL_STATIC_DRAW);
    glEnableVertexAttribArray(currentVertexAttrib);
    glVertexAttribPointer(currentVertexAttrib, 3, verticesAcessor.componentType, GL_FALSE, verticesAcessor.byteStride, 0);

    ++currentVertexAttrib;

    GLuint normalsVbo;
    glGenBuffers(1, &normalsVbo);
    glBindBuffer(normalsBufferView.target, normalsVbo);
    glBufferData(normalsBufferView.target, normalsAcessor.byteStride * normalsAcessor.count, normalsDataPtr, GL_STATIC_DRAW);
    glEnableVertexAttribArray(currentVertexAttrib);
    glVertexAttribPointer(currentVertexAttrib, 3, normalsAcessor.componentType, GL_FALSE, normalsAcessor.byteStride, 0);

    ++currentVertexAttrib;

    GLuint indicesEbo;
    glGenBuffers(1, &indicesEbo);
    glBindBuffer(indicesBufferView.target, indicesEbo);
    glBufferData(indicesBufferView.target, indicesBufferView.byteLength, indicesDataPtr, GL_STATIC_DRAW);

    glBindVertexArray(0);

    return new vertexArrayObject(vao, indicesAcessor.count, indicesAcessor.componentType);
}

geometry* geometryFromGltfPrimitive(tinygltf::Primitive gltfPrimitive, tinygltf::Scene* scene)
{
    auto verticesAcessorId = gltfPrimitive.attributes.find("POSITION")->second;
    auto normalsAcessorId = gltfPrimitive.attributes.find("NORMAL")->second;
    auto indicesAcessorId = gltfPrimitive.indices;

    //indices
    auto indicesAcessor = scene->accessors.find(indicesAcessorId)->second;

    auto indicesBufferViewId = indicesAcessor.bufferView;
    auto indicesBufferView = scene->bufferViews.find(indicesBufferViewId)->second;

    auto indicesBufferId = indicesBufferView.buffer;
    auto indicesBuffer = scene->buffers.find(indicesBufferId)->second;

    auto indicesOffset = indicesAcessor.byteOffset + indicesBufferView.byteOffset;
    auto indicesByteLength = indicesBufferView.byteLength;
    auto indicesDataPtr = (short*)&indicesBuffer.data[indicesOffset];

    auto indices = std::vector<short>();

    for (size_t i = 0; i < indicesAcessor.count; i++)
    {
        indices.push_back(indicesDataPtr[i]);
    }

    //normals
    auto normalsAcessor = scene->accessors.find(normalsAcessorId)->second;
    auto normalsBufferView = scene->bufferViews.find(normalsAcessor.bufferView)->second;
    auto normalsBuffer = scene->buffers.find(normalsBufferView.buffer)->second;

    auto normalsOffset = normalsAcessor.byteOffset + normalsBufferView.byteOffset;
    auto normalsByteLength = normalsBufferView.byteLength;
    auto normalsDataPtr = (glm::vec3*)&normalsBuffer.data[normalsOffset];

    auto normals = std::vector<glm::vec3>();

    for (size_t i = 0; i < normalsAcessor.count; i++)
    {
        normals.push_back(normalsDataPtr[i]);
    }

    ////vertices
    auto verticesAcessor = scene->accessors.find(verticesAcessorId)->second;
    auto verticesBufferView = scene->bufferViews.find(verticesAcessor.bufferView)->second;
    auto verticesBuffer = scene->buffers.find(verticesBufferView.buffer)->second;

    auto verticesOffset = verticesAcessor.byteOffset + verticesBufferView.byteOffset;
    auto verticesByteLength = verticesBufferView.byteLength;
    auto verticesDataPtr = (glm::vec3*)&verticesBuffer.data[verticesOffset];

    auto vertices = std::vector<glm::vec3>();

    for (size_t i = 0; i < verticesAcessor.count; i++)
    {
        vertices.push_back(verticesDataPtr[i]);
    }

    auto v = std::vector<vertex>();
    for (size_t i = 0; i < vertices.size(); i++)
    {
        auto position = vertices[i];
        auto normal = normals[i];
        auto texCoord = glm::vec2();

        v.push_back(vertex(position, texCoord, normal));
    }

    return geometry::create(v, indices);
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
}

material* materialFromGltf(std::string materialName, tinygltf::Scene* scene)
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

    auto program = programFromGltf(gltfTechnique.program, scene);
    auto technique = new shadingTechnique(uniforms, attributes, enableStates, program);

    for (auto& parameter : gltfTechnique.parameters)
    {
        auto semantic = semanticFromString(parameter.second.semantic);
        technique->addParameter(parameter.first, new techniqueParameter(parameter.second.type, semantic));
    }

    auto mat = new material(gltfMaterial.name, technique);

    for (auto& value : gltfMaterial.values)
    {
        auto values = value.second.number_array;
        if (values.size() == 4)
            mat->addValue(value.first, glm::vec4(values[0], values[1], values[2], values[3]));

        if (values.size() == 3)
            mat->addValue(value.first, glm::vec3(values[0], values[1], values[2]));

        if (values.size() == 1)
            mat->addValue(value.first, values[0]);
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

        auto material = materialFromGltf(firstPrimitive.material, gltfScene);
        auto vao = vaoFromGltfPrimitive(firstPrimitive, gltfScene);
        auto m = new mesh(material, vao);

        s->add(m);
    }

    return s;
}

material* importer::importDefaultMaterial()
{
    auto vertName = "../TestMesh/defaultVert.vert";
    auto fragName = "../TestMesh/defaultFrag.frag";

    auto program = programBuilder::buildProgramFromFile(vertName, fragName);

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
