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
layout (std140, binding = 0) uniform MaterialsLibraryBlock
{
    materialData items[100];
} materialsLibrary;

struct drawMaterialData
{
    uint drawMaterial0;
    uint drawMaterial1;
    uint drawMaterial2;
    uint drawMaterial3;
};

layout (std140, binding = 1) uniform DrawMaterialsBlock
{
    drawMaterialData items[9];
} drawMaterials;

in vec3 fragPosition;
in vec2 fragTexCoord;
flat in uint fragDrawId;
out vec4 fragColor;

void main(void)
{
    drawMaterialData drawMatData = drawMaterials.items[fragDrawId];
    materialData mat = materialsLibrary.items[drawMatData.drawMaterial1];
    vec4 diffuseTextureColor = texture2D(mat.diffuseTexture, fragTexCoord);
    fragColor = mat.diffuse;

    //float d = float(fragDrawId)/8.0;
    //float e = float(drawMatData.drawMaterial1)/3.0;
    //float i = float(fragInstanceId)/2.0;
    //fragColor = vec4(i, d, e, 1.0);
}