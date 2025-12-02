#version 300 es
precision mediump float;
precision lowp sampler2D;

in vec2 uv0;

uniform sampler2D textureSampler;
uniform vec3 color;

out vec4 fragColor;

void main()
{
    float alpha = texture(textureSampler, uv0).r;
    fragColor = vec4(color.r, color.g, color.b, alpha);
}