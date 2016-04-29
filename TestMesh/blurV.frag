#version 450

in vec3 fragPosition;
in vec2 fragTexCoord;

uniform sampler2D inputTexture;
 
float rt_w = 1600.0;
float rt_h = 900.0;
float vx_offset = 16.0;
 
float offset[3] = float[]( 0.0, 1.3846153846, 3.2307692308 );
float weight[3] = float[]( 0.2270270270, 0.3162162162, 0.0702702703 );
 
out vec4 fragColor;

void main() 
{ 
    vec3 tc = vec3(1.0, 0.0, 0.0);

    if (fragTexCoord.x < (vx_offset - 0.01))
    {
        vec2 uv = fragTexCoord.xy;
        
        tc = texture2D(inputTexture, uv).rgb * weight[0];

        for (int i = 1; i < 3; i++) 
        {
            tc += texture2D(inputTexture, uv + vec2(0.0, offset[i]) / rt_h).rgb \
                    * weight[i];
            tc += texture2D(inputTexture, uv - vec2(0.0, offset[i]) / rt_h).rgb \
                    * weight[i];
        }
    }
    else if (fragTexCoord.x >= (vx_offset + 0.01))
    {
        tc = texture2D(inputTexture, fragTexCoord).rgb;
    }

    fragColor = vec4(tc, 1.0);
}