#version 450

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;
uniform sampler2D inputTexture;
out vec4 fragColor;

void main(void)
{
    fragColor = texture2D(inputTexture, fragTexCoord);
    //fragColor = vec4(fragTexCoord, 0.0, 1.0);
    //fragColor = vec4(gl_FragCoord.xy/vec2(1600.0, 900.0), 0.0, 1.0);
}