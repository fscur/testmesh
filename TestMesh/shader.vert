#version 450

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in uint inDrawId;
layout (location = 3) in mat4 inModelMatrix;

uniform mat4 vp;

out vec3 fragPosition;
out vec2 fragTexCoord;
flat out uint fragDrawId;
//out vec3 fragNormal;

void main()
{
	gl_Position = vp * inModelMatrix * vec4(inPosition, 1.0);
	fragTexCoord = inTexCoord;
	fragPosition = inPosition;
	fragDrawId = inDrawId;
	//fragNormal = inNormal;
}