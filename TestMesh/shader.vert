#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in mat4 inModelMatrix;

uniform mat4 vp;

out vec2 fragTexCoord;
out vec3 fragNormal;

void main()
{
    gl_Position = vp * inModelMatrix * vec4(inPosition, 1.0);
    fragTexCoord = inTexCoord;
    fragNormal = inNormal;
}