#version 450

in vec2 fragTexCoord;

uniform sampler2D inputTexture;

out vec4 fragColor;

void main(void)
{
    //0.06136	0.24477	0.38774	0.24477	0.06136
    
    int level = 1;
    vec3 color = vec3(0.0);
    color += textureLod(inputTexture, fragTexCoord, level + 0).rgb * 0.38774;
    color += textureLod(inputTexture, fragTexCoord, level + 1).rgb * 0.24477 * 2;
    color += textureLod(inputTexture, fragTexCoord, level + 2).rgb * 0.06136 * 2;

    fragColor = vec4(color, 1.0);// * 0.25 + color2 * 0.125;

    //fragColor = textureLod(inputTexture, fragTexCoord, 5);
}