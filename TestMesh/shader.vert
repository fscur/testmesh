#version 440

in vec3 inPosition;
in vec2 inTexCoord;
in vec3 inNormal;
in mat4 inModelMatrix;

uniform mat4 vp;

out vec2 fragTexCoord;
out vec3 fragPosition;
out vec3 fragNormal;

void main()
{
	gl_Position = vp * inModelMatrix * vec4(inPosition, 1.0);
	fragTexCoord = inTexCoord;
	fragPosition = inPosition;
	fragNormal = inNormal;
}