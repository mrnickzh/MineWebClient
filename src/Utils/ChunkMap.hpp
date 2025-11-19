#pragma once
#include <map>

#include "../Objects/Object.hpp"
#include "vec3Comparator.hpp"

class ChunkMap {
public:
    std::map<glm::vec3, std::shared_ptr<Object>, vec3Comparator> blocks;

    void addBlock(glm::vec3 blockPos, std::shared_ptr<Object> block);
    std::shared_ptr<Object> getBlock(glm::vec3 blockPos);
};
