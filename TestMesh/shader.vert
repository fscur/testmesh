#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in uint inDrawId;

struct drawData
{
    mat4 m;
    uint materialId;
    uint pad0;
    uint pad1;
    uint pad2;
};

layout (std140, binding = 0) buffer DrawInfo
{
    drawData items[];
} drawInfo;

uniform mat4 vp;

out vec3 fragPosition;
out vec2 fragTexCoord;
flat out uint fragMaterialId;
//out vec3 fragNormal;

void main()
{
    drawData data = drawInfo.items[inDrawId]; 
    mat4 m = data.m;
	gl_Position = vp * m * vec4(inPosition, 1.0);
	fragTexCoord = inTexCoord;
	fragPosition = inPosition;
	//fragNormal = inNormal;
	fragMaterialId = data.materialId;
}