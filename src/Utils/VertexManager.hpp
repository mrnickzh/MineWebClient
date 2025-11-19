#pragma once
#include <map>
#include <string>
#include <GLES3/gl3.h>

struct VertexUnit {
    GLuint vao;
    GLuint vbo;
    GLuint texture;
};

class VertexManager {
public:
    std::map<int, VertexUnit> vertexunits;

    void initVAO(int vuid, float vertices[], int length, std::string texturepath);
    VertexUnit getVAO(int vuid);
};
