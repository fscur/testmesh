#version 440

in vec3 inPosition;
in vec2 inTexCoord;
in vec3 inNormal;

uniform mat4 v;
uniform mat4 p;

out vec2 fragTexCoord;
out vec3 fragPosition;
out vec3 fragNormal;

void main()
{
	//gl_Position = vec4(inPosition, 1.0);
	gl_Position = p * v * mat4(1.0) * vec4(inPosition, 1.0);
	fragTexCoord = inTexCoord;
	fragPosition = inPosition;
	fragNormal = inNormal;
}