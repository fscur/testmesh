#version 450

in vec3 fragPosition;
in vec2 fragTexCoord;

uniform sampler2D inputTexture;
uniform vec2 resolution; 
uniform int level;

float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308);
float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703);

out vec4 fragColor;
 
vec3 fetch(vec2 uv)
{
    return texture2D(inputTexture, uv).rgb;
}

//vec3 fetch(vec2 uv)
//{
//    vec3 color = vec3(0.0);

//    color += textureLod(inputTexture, uv, level + 0).rgb * 0.38774;
//    color += textureLod(inputTexture, uv, level + 1).rgb * 0.24477 * 2;
//    color += textureLod(inputTexture, uv, level + 2).rgb * 0.06136 * 2;

//    return color;
//}

void main() 
{
    vec2 uv = fragTexCoord.xy;
    vec3 color = vec3(0.0, 0.0, 0.0);
    color = fetch(uv) * weight[0];

    for (int i = 1; i < 3; i++) 
    {
        color += fetch(uv + vec2(offset[i]/resolution.x, 0.0)) * weight[i];
        color += fetch(uv - vec2(offset[i]/resolution.x, 0.0)) * weight[i];
    }

    fragColor = vec4(color, 1.0);
}