#version 450

in vec3 fragPosition;
in vec2 fragTexCoord;

uniform sampler2D inputTexture;
uniform vec2 resolution; 

float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308);
float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703);

out vec4 fragColor;
 
void main() 
{
    vec3 color = vec3(0.0, 0.0, 0.0);

    vec2 uv = fragTexCoord.xy;

    color = texture2D(inputTexture, uv).rgb * weight[0];

    for (int i = 1; i < 3; i++) 
    {
        color += texture2D(inputTexture, uv + vec2(offset[i]/resolution.x, 0.0)).rgb * weight[i];
        color += texture2D(inputTexture, uv - vec2(offset[i]/resolution.x, 0.0)).rgb * weight[i];
    }

    fragColor = vec4(color, 1.0);
}