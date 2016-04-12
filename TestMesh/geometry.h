#pragma once
#include "types.h"
#include "vertex.h"
#include <vector>

struct geometry
{
private:
    static geometry* _quad;

public:
    uint verticesCount;
    uint indicesCount;
    vertex* vboData;
    uint* eboData;
    uint vboSize;
    uint eboSize;

public:
    static geometry* quad();

public:
    geometry();
    ~geometry();

    static geometry* create(std::vector<vertex> vertices, std::vector<uint> indices);
    static geometry* create(
        uint verticesCount,
        float* positionsBuffer,
        float* texCoordsBuffer,
        float* normalsBuffer,
        uint indicesCount,
        uint* indicesBuffer);

    static void calcNormals(std::vector<vertex>& vertices, std::vector<uint>& indices);
    static void calcTangents(std::vector<vertex>& vertices, std::vector<uint>& indices);
};