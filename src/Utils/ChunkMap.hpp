#pragma once
#include <map>
#include <deque>
#include <GLES3/gl3.h>

#include "../Objects/Object.hpp"
#include "../../../MineWebServer/src/Utils/vec3Comparator.hpp"
#include "../Camera.hpp"

class ChunkMap {
public:
    glm::vec3 chunkpos;
    // std::map<glm::vec3, std::shared_ptr<Object>, vec3Comparator> blocks;
    uint64_t blocksdata[512];
    // glm::mat4 translations[6*6*512];
    // float sides[30*6*512];
    // float textures[6*6*512];
    glm::mat4 trans;
    float* sides;
    float* textures;
    int instanceCount;

    GLuint VBO = 0;
    GLuint VAO = 0;
    GLuint TBO = 0;

    GLuint chunkTexture = 0;

    ChunkMap(glm::vec3 pos);
    ~ChunkMap();

    bool checkValidPos(glm::vec3 pos);
    void addBlock(glm::vec3 blockPos, Object block);
    Object getBlock(glm::vec3 blockPos);
    void initTranslations();
    void initLights();
    void renderChunk();
    bool checkCull(Frustum& frustum, glm::vec3 chunkpos);
};
