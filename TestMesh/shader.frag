#version 450
#extension GL_ARB_bindless_texture : require

struct materialData
{
    vec4 diffuse;
    //16 bytes
    vec4 specular;
    //16 bytes
    sampler2D diffuseTexture;
    //8 bytes
    sampler2D normalTexture;
    //8 bytes
};

//uniform block
layout (std140, binding = 0) buffer Materials
{
    materialData items[];
} materials;

in vec3 fragPosition;
in vec2 fragTexCoord;
flat in uint fragDrawId;
out vec4 fragColor;

void main(void)
{
    materialData mat = materials.items[fragDrawId];
    vec4 diffuseTextureColor = texture2D(mat.diffuseTexture, fragTexCoord);
    fragColor = mat.diffuse * diffuseTextureColor;

    //float d = float(fragDrawId)/8.0;
    //float e = float(drawMatData.drawMaterial1)/3.0;
    //float i = float(fragInstanceId)/2.0;
    //fragColor = vec4(i, d, e, 1.0);
}