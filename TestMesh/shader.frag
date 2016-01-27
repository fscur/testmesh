#version 330
uniform vec4 ambient;
uniform vec4 diffuse;

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;

out vec4 fragcolor;

void main(void)
{
	fragcolor = ambient * diffuse;
}