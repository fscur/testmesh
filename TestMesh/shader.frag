#version 440
#extension GL_ARB_bindless_texture : require

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;

uniform sampler2DArray textureArrays[32];

layout (std140, binding = 0) uniform GlyphBufferBlock
{
    int textureUnit;
    float texturePage;
} glyphData;

out vec4 fragColor;

vec4 fetch()
{
    int array = glyphData.textureUnit;
    float page = glyphData.texturePage;
    return texture(textureArrays[array], vec3(fragTexCoord, page));
}

void main(void)
{
	fragColor = fetch();
}