#include "ChunkMap.hpp"


void ChunkMap::addBlock(glm::vec3 blockPos, std::shared_ptr<Object> block) {
    blocks[blockPos] = block;
}

std::shared_ptr<Object> ChunkMap::getBlock(glm::vec3 blockPos) {
    return blocks[blockPos];
}

