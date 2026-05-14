#include "BlockRegistry.hpp"

void BlockRegistry::registerBlock(int id, Object block, std::string modName) {
    blocks.insert({id, std::pair{modName, block}});
}

Object BlockRegistry::getBlock(int id) {
    auto it = blocks.find(id);
    if (it == blocks.end()) {
        printf("Block id %d not found\n", id);
    }
    return it->second.second;
}

