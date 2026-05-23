#version 300 es
layout (location = 0) in vec2 aPos;
// layout (location = 1) in vec3 wPos;
//layout (location = 2) in float aTexture;

layout (location = 3) in vec4 aTranslation1;
layout (location = 4) in vec4 aTranslation2;
layout (location = 5) in vec4 aTranslation3;
layout (location = 6) in vec4 aTranslation4;

// out vec3 TexCoord;

uniform mat4 projection;
uniform mat4 view;

void main()
{
    mat4 model = mat4(aTranslation1, aTranslation2, aTranslation3, aTranslation4);
    vec3 worldPos = vec3(model[3][0], model[3][1], model[3][2]);

    vec3 camRight = vec3(view[0][0], view[1][0], view[2][0]);
    vec3 camUp    = vec3(view[0][1], view[1][1], view[2][1]);

    vec3 pos = worldPos
    + camRight * aPos.x * length(vec3(model[0][0], model[0][1], model[0][2]))
    + camUp    * aPos.y * length(vec3(model[1][0], model[1][1], model[1][2]));

    gl_Position = projection * view * vec4(pos, 1.0);
    // TexCoord = vec3(aTexCoord.x, aTexCoord.y, aTexture);
}