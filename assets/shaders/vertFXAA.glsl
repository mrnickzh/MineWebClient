#version 300 es

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 fragCoord;

void main(void)
{
	fragCoord = aTexCoord;

	gl_Position = vec4(aPos.x, aPos.y, 0.0f, 1.0f);
}