#pragma once
#include <string>
#include <GLES3/gl3.h>

class TextureManager {
public:
    GLuint textureArray = 0;

    void startInit(int w, int h);
    void endInit();
    void addTexture(std::string texturepath, int textureoffset);
};
