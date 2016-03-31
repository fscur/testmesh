#version 450

in vec3 inPosition;
in vec2 inTexCoord;
in vec3 inNormal;

uniform mat4 m;
uniform mat4 v;
uniform mat4 p;
uniform vec2 texPos;
uniform vec2 texSize;

out vec2 fragTexCoord;
out vec3 fragPosition;
out vec3 fragNormal;

void main()
{
	//gl_Position = vec4(inPosition, 1.0);
	gl_Position = p * v * m * vec4(inPosition, 1.0);
	fragTexCoord = texPos + inTexCoord * texSize;
	fragPosition = inPosition;
	fragNormal = inNormal;
}