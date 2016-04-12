#version 450
struct glyphInfo
{
    vec2 pos;
    float pad0;
    float pad1;
    vec2 size;
    float pad2;
    float pad3;
    float shift;
    float pad4;
    float pad5;
    float pad6;
};

layout (location = 0) in vec3 inPosition;
layout (location = 1) in vec2 inTexCoord;
layout (location = 2) in vec3 inNormal;
layout (location = 3) in vec3 inTangent;
layout (location = 4) in uint inGlyphId;
layout (location = 5) in mat4 inModelMatrix;

uniform mat4 v;
uniform mat4 p;

layout (std140, binding = 0) buffer GlyphInfos
{
    glyphInfo items[];
} glyphs;

out vec2 fragTexCoord;
out vec3 fragPosition;
flat out float fragShift;

void main()
{
	gl_Position = p * v * inModelMatrix * vec4(inPosition, 1.0);
    glyphInfo info = glyphs.items[inGlyphId];
	fragTexCoord = info.pos + inTexCoord * info.size;
	fragPosition = inPosition;
    fragShift = info.shift;
}