#version 300 es
precision highp float;
precision highp sampler2DArray;
precision highp usampler3D;

out vec4 FragColor;

in vec2 TexCoord;
flat in int chunkSide;
in vec2 LightLevel;
in highp vec3 fragChunkPos;

uniform sampler2DArray textureSampler;
uniform usampler3D chunkSampler;
uniform float ambientLevel;

void main()
{
    ivec2 uvs;
    ivec3 coords = ivec3(int(floor(fragChunkPos.z + 3.99999f)), int(floor(fragChunkPos.x + 3.99999f)), int(floor(fragChunkPos.y + 3.99999f)));

    if (chunkSide == 1) {
        uvs.x = int(clamp(fract(fragChunkPos.x), 0.0f, 0.99f) * 16.0f);
        uvs.y = int(clamp(fract(fragChunkPos.y), 0.0f, 0.99f) * 16.0f);
    }
    if (chunkSide == 3) {
        uvs.x = int(clamp(fract(fragChunkPos.z), 0.0f, 0.99f) * 16.0f);
        uvs.y = int(clamp(fract(fragChunkPos.y), 0.0f, 0.99f) * 16.0f);
    }
    if (chunkSide == 5) {
        uvs.x = int(clamp(fract(fragChunkPos.x), 0.0f, 0.99f) * 16.0f);
        uvs.y = int(clamp(fract(fragChunkPos.z), 0.0f, 0.99f) * 16.0f);
    }
    if (chunkSide == 0) {
        uvs.x = int(clamp(fract(fragChunkPos.x), 0.0f, 0.99f) * 16.0f);
        uvs.y = int(clamp(fract(fragChunkPos.y), 0.0f, 0.99f) * 16.0f);
        coords.x = int(ceil(fragChunkPos.z + 3.99999f));
    }
    if (chunkSide == 2) {
        uvs.x = int(clamp(fract(fragChunkPos.z), 0.0f, 0.99f) * 16.0f);
        uvs.y = int(clamp(fract(fragChunkPos.y), 0.0f, 0.99f) * 16.0f);
        coords.y = int(ceil(fragChunkPos.x + 3.99999f));
    }
    if (chunkSide == 4) {
        uvs.x = int(clamp(fract(fragChunkPos.x), 0.0f, 0.99f) * 16.0f);
        uvs.y = int(clamp(fract(fragChunkPos.z), 0.0f, 0.99f) * 16.0f);
        coords.z = int(ceil(fragChunkPos.y + 3.99999f));
    }

    uvec4 pack = texelFetch(chunkSampler, coords, 0);

    vec2 lightvalues;
    float blockid = float(int(pack[0] & 0xffu));

    int shift = int(mod(float(chunkSide), 2.0f));

    lightvalues.x = float(int((pack[chunkSide / 2 + 1] >> (6 * shift)) & 0x7u)) / 5.0f;
    lightvalues.y = float(int((pack[chunkSide / 2 + 1] >> (6 * shift + 3)) & 0x7u)) / -5.0f;

    vec3 pointlight = vec3(1.0f, 1.0f, 0.75f) * lightvalues.x;
    vec3 ambientlight = vec3(0.75f, 0.75f, 1.0f) * clamp(ambientLevel + lightvalues.y, 0.1f, 1.0f);

    FragColor = texelFetch(textureSampler, ivec3(uvs.xy, int(blockid)), 0) * vec4(max(pointlight.x, ambientlight.x), max(pointlight.y, ambientlight.y), max(pointlight.z, ambientlight.z), 1.0f);
}