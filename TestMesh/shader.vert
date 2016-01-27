#version 440

in vec3 inPosition;
in vec2 inTexCoord;
in vec3 inNormal;

layout(std140, binding = 0) uniform TransformsBlock
{
	mat4 items[1024];
} transforms;

uniform mat4 v;
uniform mat4 p;
//uniform uint mIndex;

out vec2 fragTexCoord;
out vec3 fragPosition;
out vec3 fragNormal;

void main()
{
	gl_Position = p * v * transforms.items[gl_InstanceID] * vec4(inPosition, 1.0);
	//gl_Position = p * v * mat4(1.0) * vec4(inPosition, 1.0);
	fragTexCoord = inTexCoord;
	fragPosition = inPosition;
	fragNormal = inNormal;
}