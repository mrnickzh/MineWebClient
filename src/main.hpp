#pragma once

#include "Shader.hpp"
#include "Objects/Object.hpp"
#include "../MineWebServer/src/Server.hpp"
#include "Entities/Entity.hpp"
#include "Physics/PhysicsEngine.hpp"
#include "Utils/ChunkMap.hpp"
#include "Utils/TextureManager.hpp"
#include "Utils/VertexManager.hpp"

namespace Main {
    inline Shader* ourShader;

    inline VertexManager* vertexManager;
    inline TextureManager* textureManager;

    inline std::vector<std::shared_ptr<Object>> objects;
    inline std::map<glm::vec3, std::shared_ptr<ChunkMap>, vec3Comparator> chunks;
    inline std::vector<glm::vec3> requestedChunks;

    inline std::unique_ptr<PhysicsEngine> physicsEngine;

    inline bool isSingleplayer = false;
    inline Server& serverInstance = Server::getInstance();

    inline std::shared_ptr<Entity> localPlayer;
}
