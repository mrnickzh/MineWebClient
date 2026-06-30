#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aTexture;

out vec2 TexCoord;
flat out int chunkSide;
out vec2 LightLevel;
out vec3 fragChunkPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * model * vec4(aPos, 1.0f);
    // fragChunkPos = vec3((aPos.x + 4.0f) / 8.0f, (aPos.y + 4.0f) / 8.0f, (aPos.z + 4.0f) / 8.0f);
    fragChunkPos = aPos;
    TexCoord = vec2(aTexCoord.x, aTexCoord.y);
    chunkSide = int(aTexture);
}