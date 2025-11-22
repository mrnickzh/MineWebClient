#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aTexture;

layout (location = 3) in vec4 aTranslation1;
layout (location = 4) in vec4 aTranslation2;
layout (location = 5) in vec4 aTranslation3;
layout (location = 6) in vec4 aTranslation4;

out vec3 TexCoord;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * mat4(aTranslation1, aTranslation2, aTranslation3, aTranslation4) * vec4(aPos, 1.0f);
    TexCoord = vec3(aTexCoord.x, aTexCoord.y, aTexture);
}