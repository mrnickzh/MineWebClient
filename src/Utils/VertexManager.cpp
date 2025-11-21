#include "VertexManager.hpp"

void VertexManager::initVBO(int vboid, float vertices[], int length) {
    GLuint vbo = 0;

    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, length, vertices, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    VBOs[vboid] = vbo;
};

GLuint VertexManager::getVBO(int vboid) {
    return VBOs[vboid];
};