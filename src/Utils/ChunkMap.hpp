#pragma once
#include <map>
#include <deque>
#include <GLES3/gl3.h>

#include "../Objects/Object.hpp"
#include "vec3Comparator.hpp"
#include "../Camera.hpp"

class ChunkMap {
public:
    std::map<glm::vec3, std::shared_ptr<Object>, vec3Comparator> blocks;
    glm::mat4 translations[6*6*512];
    float sides[30*6*512];
    float textures[6*6*512];
    int instanceCount;

    GLuint VBO = 0;
    GLuint VAO = 0;
    GLuint IVBO = 0;
    GLuint TBO = 0;

    GLuint chunkTexture = 0;

    ChunkMap();

    void addBlock(glm::vec3 blockPos, std::shared_ptr<Object> block);
    std::shared_ptr<Object> getBlock(glm::vec3 blockPos);
    void initTranslations();
    void initBlocks();
    void renderChunk();
    bool checkCull(Frustum& frustum, glm::vec3 chunkpos);
};
