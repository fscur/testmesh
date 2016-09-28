#version 450

in vec3 inPosition;

uniform mat4 mvp;

out vec3 fragPosition;

void main()
{
	gl_Position = mvp * vec4(inPosition, 1.0);
	fragPosition = inPosition;
}