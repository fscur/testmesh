#version 450

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;

//uniform sampler2DArray textureArrays[32];
uniform sampler2D glyphTexture;

//layout (std140, binding = 0) uniform GlyphBufferBlock
//{
//    int textureUnit;
//    float texturePage;
//} glyphData;

out vec4 fragColor;

//vec4 fetch()
//{
//    int array = glyphData.textureUnit;
//    float page = glyphData.texturePage;
//    return texture(textureArrays[array], vec3(fragTexCoord, page));
//}

void main(void)
{
    //fragColor = vec4(1.0) * texture2D(glyphTexture, vec2(r,s) + (fragTexCoord) * vec2(w, h)).r;
    fragColor = vec4(1.0) * texture2D(glyphTexture, fragTexCoord).r;
}