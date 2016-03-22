#version 450

layout (location = 0) in vec3 inPosition;

uniform mat4 vp;

void main()
{
    gl_Position = vp * vec4(inPosition, 1.0);
}