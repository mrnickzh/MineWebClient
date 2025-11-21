#include "ChunkMap.hpp"

#include "../main.hpp"
#include "../Objects/AirObject.hpp"

ChunkMap::ChunkMap() {
    glGenVertexArrays(1, &VAO);

    memset(translations, 0, sizeof(translations));
    instanceCount = 0;
    for (auto& b : blocks) {
        if (typeid((*b.second)) != typeid(AirObject)) {
            translations[instanceCount++] = glm::vec4(b.second->position, (float)b.second->texture);
        }
    }

    glGenBuffers(1, &IVBO);
    glBindBuffer(GL_ARRAY_BUFFER, IVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * 512, &translations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void ChunkMap::addBlock(glm::vec3 blockPos, std::shared_ptr<Object> block) {
    blocks[blockPos] = block;
}

std::shared_ptr<Object> ChunkMap::getBlock(glm::vec3 blockPos) {
    return blocks[blockPos];
}

void ChunkMap::initTranslations() {
    memset(translations, 0, sizeof(translations));
    instanceCount = 0;
    for (auto& b : blocks) {
        if (typeid((*b.second)) != typeid(AirObject)) {
            translations[instanceCount] = glm::vec4(b.second->position, (float)b.second->texture);
            // std::cout << translations[instanceCount].x << " " << translations[instanceCount].y << " " << translations[instanceCount].z << " " << translations[instanceCount].w << std::endl;
            instanceCount++;
        }
    }

    glDeleteBuffers(1, &IVBO);
    glGenBuffers(1, &IVBO);
    glBindBuffer(GL_ARRAY_BUFFER, IVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec4) * 512, &translations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ChunkMap::renderChunk() {
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, Main::vertexManager->getVBO(0));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, IVBO);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glVertexAttribDivisor(2, 1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // for (int i = 0; i < instanceCount; i++) {
    //     // if (translations[i].x == 0.0f && translations[i].y == 0.0f && translations[i].z == 0.0f) {
    //         std::cout << translations[i].x << " " << translations[i].y << " " << translations[i].z << " " << translations[i].w << " " << i << std::endl;
    //     // }
    // }

    glDrawArraysInstanced(GL_TRIANGLES, 0, 36, instanceCount);

    glBindVertexArray(0);
}
