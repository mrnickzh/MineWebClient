#include "ModBridge.hpp"

// void ModBridge::setBlock(int x, int y, int z, int id) {
//     glm::vec3 chunkPos = glm::vec3();
//     glm::vec3 blockPos = glm::vec3();
//     chunkPos.x = (float)(x / 8);
//     chunkPos.y = (float)(y / 8);
//     chunkPos.z = (float)(z / 8);
//     blockPos.x = (float)(x % 8);
//     blockPos.y = (float)(y % 8);
//     blockPos.z = (float)(z % 8);
//
//     std::lock_guard<std::mutex> chunkmutex(Server::getInstance().chunksMutex);
//     Server::getInstance().chunks[chunkPos]->addBlock(blockPos, Block(id, blockPos, glm::vec3(0.0f, 0.0f, 0.0f), (id == 0 ? false : true), glm::vec3(0.5f, 0.5f, 0.5f)));
// }
