#pragma once

#include <map>

#include "../Objects/Object.hpp"

class BlockRegistry {
public:
    std::map<int, std::pair<std::string, Object>> blocks;

    BlockRegistry() = default;
    void registerBlock(int id, Object block, std::string modName);
    Object getBlock(int id);
};

