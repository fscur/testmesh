#version 330

in vec3 inPosition;
in vec2 inTexCoord;
in vec3 inNormal;

uniform mat4 mvp;
uniform mat4 mv;

out vec2 fragTexCoord;
out vec3 fragPosition;
out vec3 fragNormal;

void main()
{
	gl_Position = mvp * vec4(inPosition, 1.0);
	fragTexCoord = inTexCoord;
	fragPosition = inPosition;
	fragNormal = inNormal;
	gl_PointSize = 10.0;
}