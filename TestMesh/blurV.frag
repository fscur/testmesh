#version 450

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;

uniform sampler2D inputTexture;

out vec4 fragColor;

void main(void)
{
    fragColor = texture2D(inputTexture, gl_FragCoord.xy) * 0.5;
}