#version 440
#extension GL_ARB_bindless_texture : require

struct materialData
{
	vec4 diffuse;
	vec4 specular;
	sampler2D diffuseTexture;
	sampler2D normalTexture;
};

//uniform block
layout (std140, binding = 1) uniform MaterialsBlock
{
	materialData items[100];
} materials;

uniform uint matIndex;

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;

out vec4 fragcolor;

void main(void)
{
	vec4 diffuseTextureColor = texture(materials.items[matIndex].diffuseTexture, fragTexCoord);
	//vec4 normalTextureColor = texture(normalTexture, fragTexCoord);
	fragcolor = diffuseTextureColor * materials.items[matIndex].diffuse;
	//fragcolor = vec4(1.0);
}