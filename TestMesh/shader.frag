#version 450

in vec3 fragPosition;

uniform samplerCube cubemap;

out vec4 fragColor;

void main(void)
{
    fragColor = texture(cubemap, fragPosition);
    //fragColor = vec4(fragTexCoord, 0.0, 1.0);
    //fragColor = vec4(gl_FragCoord.xy/vec2(1600.0, 900.0), 0.0, 1.0);
}