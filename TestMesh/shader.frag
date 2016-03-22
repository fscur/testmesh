#version 450

in vec2 fragTexCoord;
in vec3 fragNormal;
in int gl_PrimitiveID;

layout (location = 0) out vec4 fragColor;

void main()
{
        vec3 lightPos = vec3(0.4, 0.3, 0.5);
        float intensity = max(dot(normalize(lightPos), fragNormal), 0.2);
        fragColor = vec4(1.0, 0.0, 0.0, 1.0)  * intensity;
}