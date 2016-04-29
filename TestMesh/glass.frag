#version 450

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;

uniform sampler2D background;

out vec4 fragColor;

void main(void)
{
    //fragColor = texture2D(background, gl_FragCoord.xy/vec2(1600.0, 900.0)) + vec4(1.0, 1.0, 1.0, 0.5);

    fragColor = vec4(0.9) * texture2D(background, gl_FragCoord.xy/vec2(1600.0, 900.0));

    //fragColor = texture2D(background, fragTexCoord) + vec4(0.0, 0.0, 1.0, 0.0);
    //fragColor = vec4(gl_FragCoord.xy/vec2(1600.0, 900.0), 0.0, 1.0);
}