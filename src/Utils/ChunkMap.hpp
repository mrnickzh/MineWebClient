#pragma once
#include <map>
#include <GLES3/gl3.h>

#include "../Objects/Object.hpp"
#include "vec3Comparator.hpp"
#include "../Camera.hpp"

class ChunkMap {
public:
    std::map<glm::vec3, std::shared_ptr<Object>, vec3Comparator> blocks;
    glm::mat4 translations[512];
    float textures[512];
    int instanceCount;

    GLuint VAO = 0;
    GLuint IVBO = 0;
    GLuint TBO = 0;

    ChunkMap();

    void addBlock(glm::vec3 blockPos, std::shared_ptr<Object> block);
    std::shared_ptr<Object> getBlock(glm::vec3 blockPos);
    void initTranslations();
    void renderChunk();
    bool checkCull(Frustum& frustum, glm::vec3 chunkpos);
};
