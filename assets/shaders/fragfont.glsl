#version 300 es
precision mediump float;
precision lowp sampler2DArray;

uniform sampler2DArray textureSampler;
uniform vec3 color;
uniform int background;
uniform float texindex;

in vec2 uv0;

out vec4 fragColor;

void main()
{
    float alpha = texture(textureSampler, vec3(uv0.xy, texindex)).r;
    if (background == 1) {
        alpha = 1.0f;
    }
    fragColor = vec4(color.r, color.g, color.b, alpha);
}