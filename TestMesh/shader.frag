#version 450

in vec3 fragPosition;
in vec2 fragTexCoord;
in vec3 fragNormal;

//uniform sampler2DArray textureArrays[32];
uniform sampler2D glyphTexture;
uniform vec2 texelSize;
uniform float shift;

float gamma = 1.75;

//layout (std140, binding = 0) uniform GlyphBufferBlock
//{
//    int textureUnit;
//    float texturePage;
//} glyphData;

out vec4 fragColor;

//vec4 fetch()
//{
//    int array = glyphData.textureUnit;
//    float page = glyphData.texturePage;
//    return texture(textureArrays[array], vec3(fragTexCoord, page));
//}

void main(void)
{
    //fragColor = vec4(1.0) * texture2D(glyphTexture, vec2(r,s) + (fragTexCoord) * vec2(w, h)).r;
    //fragColor = vec4(1.0) * texture2D(glyphTexture, fragTexCoord);

    vec4 current = texture2D(glyphTexture, fragTexCoord);
    vec4 previous= texture2D(glyphTexture, fragTexCoord + vec2(-1.0, 0.0) * texelSize.xy);

    current = pow(current, vec4(1.0/gamma));
    previous= pow(previous, vec4(1.0/gamma));

    float r = current.r;
    float g = current.g;
    float b = current.b;

    if( shift <= 0.333 )
    {
        float z = shift/0.333;
        r = mix(current.r, previous.b, z);
        g = mix(current.g, current.r,  z);
        b = mix(current.b, current.g,  z);
    }
    else if( shift <= 0.666 )
    {
        float z = (shift-0.33)/0.333;
        r = mix(previous.b, previous.g, z);
        g = mix(current.r,  previous.b, z);
        b = mix(current.g,  current.r,  z);
    }
   else if( shift < 1.0 )
    {
        float z = (shift-0.66)/0.334;
        r = mix(previous.g, previous.r, z);
        g = mix(previous.b, previous.g, z);
        b = mix(current.r,  previous.b, z);
    }

   float t = max(max(r,g),b);
   vec4 vcolor = vec4(1.0, 0.78, 0.0, 1.0);
   vec4 color = vec4(vcolor.rgb, (r+g+b)/3.0);
   color = t*color + (1.0-t)*vec4(r,g,b, min(min(r,g),b));
   fragColor = vec4( color.rgb, vcolor.a*color.a);
   //fragColor = vec4(1.0);
}