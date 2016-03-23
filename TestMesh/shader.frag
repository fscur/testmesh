#version 440
#extension GL_ARB_bindless_texture : require

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;

out vec4 fragColor;

void main(void)
{
	fragColor = vec4(1.0);
}