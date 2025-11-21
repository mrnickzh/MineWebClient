#pragma once
#include <map>
#include <string>
#include <GLES3/gl3.h>

class VertexManager {
public:
    std::map<int, GLuint> VBOs;

    void initVBO(int vboid, float vertices[], int length);
    GLuint getVBO(int vboid);
};
