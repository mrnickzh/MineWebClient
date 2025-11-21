#version 300 es
precision mediump float;
precision lowp sampler2DArray;

out vec4 FragColor;

in vec3 TexCoord;

uniform sampler2DArray textureArray;

void main()
{
    FragColor = texture(textureArray, vec3(TexCoord.xy, TexCoord.z));
}