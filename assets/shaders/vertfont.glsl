#version 300 es
layout (location = 0) in vec3 pos;
layout (location = 1) in vec2 texcord;

uniform mat4 projection;

out vec2 uv0;
out vec3 clr;

void main()
{
    gl_Position = projection * vec4(pos.x, pos.y, 0.0f, 1.0f);
    uv0 = texcord;
}