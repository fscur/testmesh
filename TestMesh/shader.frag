#version 450

in vec2 fragTexCoord;

uniform int diffuseTextureArrayIndex;
uniform float diffuseTexturePageIndex;
uniform sampler2DArray textureArrays[32];

layout (location = 0) out vec4 fragColor;

void main()
{
	fragColor = texture(textureArrays[diffuseTextureArrayIndex], vec3(fragTexCoord.xy, diffuseTexturePageIndex));
	//fragColor = texture(textureArrays[4], vec3(fragTexCoord.xy, 0));
	//fragColor = vec4(0.0, 1.0, 0.0, 1.0);
}