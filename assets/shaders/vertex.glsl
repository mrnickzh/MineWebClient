#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;
layout (location = 2) in float aTexture;

layout (location = 3) in vec4 aTranslation1;
layout (location = 4) in vec4 aTranslation2;
layout (location = 5) in vec4 aTranslation3;
layout (location = 6) in vec4 aTranslation4;

layout (location = 7) in vec2 aLightLevel1;
layout (location = 8) in vec2 aLightLevel2;
layout (location = 9) in vec2 aLightLevel3;
layout (location = 10) in vec2 aLightLevel4;
layout (location = 11) in vec2 aLightLevel5;
layout (location = 12) in vec2 aLightLevel6;

out vec3 TexCoord;
out vec2 LightLevel;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    gl_Position = projection * view * mat4(aTranslation1, aTranslation2, aTranslation3, aTranslation4) * vec4(aPos, 1.0f);
    TexCoord = vec3(aTexCoord.x, aTexCoord.y, aTexture);
    int side = gl_VertexID / 6;
    if (side == 0) {
        LightLevel = aLightLevel1;
    }
    else if (side == 1) {
        LightLevel = aLightLevel2;
    }
    else if (side == 2) {
            LightLevel = aLightLevel3;
    }
    else if (side == 3) {
            LightLevel = aLightLevel4;
    }
    else if (side == 4) {
            LightLevel = aLightLevel5;
    }
    else if (side == 5) {
            LightLevel = aLightLevel6;
    }
}