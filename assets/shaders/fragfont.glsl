#version 300 es
precision mediump float;
precision lowp sampler2D;

in vec2 uv0;

uniform sampler2D textureSampler;
uniform vec3 color;
uniform int background;

out vec4 fragColor;

void main()
{
    float alpha = texture(textureSampler, uv0).r;
    if (background == 1) {
        alpha = 1.0f;
    }
    fragColor = vec4(color.r, color.g, color.b, alpha);
}