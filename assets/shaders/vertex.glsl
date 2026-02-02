#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aTexture;

layout (location = 3) in vec4 aTranslation1;
layout (location = 4) in vec4 aTranslation2;
layout (location = 5) in vec4 aTranslation3;
layout (location = 6) in vec4 aTranslation4;

out vec2 TexCoord;
flat out int chunkSide;
out vec2 LightLevel;
out vec3 fragChunkPos;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    mat4 model = mat4(aTranslation1, aTranslation2, aTranslation3, aTranslation4);
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    // fragChunkPos = vec3((aPos.x + 4.0f) / 8.0f, (aPos.y + 4.0f) / 8.0f, (aPos.z + 4.0f) / 8.0f);
    fragChunkPos = aPos;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
    chunkSide = int(aTexture);
}