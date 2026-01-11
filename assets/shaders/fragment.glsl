#version 300 es
precision mediump float;
precision lowp sampler2DArray;

out vec4 FragColor;

in vec3 TexCoord;
in vec2 LightLevel;

uniform sampler2DArray textureSampler;
uniform float ambientLevel;

void main()
{
    vec4 lighting = max(clamp((ambientLevel + LightLevel.y), 0.0f, 1.0f), LightLevel.x) * vec4(1.0f, 1.0f, 1.0f, 0.0f);
    FragColor = texture(textureSampler, vec3(TexCoord.xy, TexCoord.z)) * lighting;
}