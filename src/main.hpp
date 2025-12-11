#pragma once

#include "Shader.hpp"
#include "Objects/Object.hpp"
#include "../MineWebServer/src/Server.hpp"
#include "Entities/Entity.hpp"
#include "GUI/GUIManager.hpp"
#include "Physics/PhysicsEngine.hpp"
#include "Utils/ChunkMap.hpp"
#include "Utils/FontManager.hpp"
#include "Utils/TextureManager.hpp"
#include "Utils/VertexManager.hpp"

namespace Main {
    inline Shader* ourShader;
    inline Shader* fontShader;

    inline VertexManager* vertexManager;
    inline TextureManager* textureManager;
    inline FontManager* fontManager;

    inline GUIManager* menuManager;
    inline GUIManager* gameUIManager;

    inline std::map<glm::vec3, std::shared_ptr<ChunkMap>, vec3Comparator> chunks;
    inline std::vector<glm::vec3> requestedChunks;

    inline std::unique_ptr<PhysicsEngine> physicsEngine;

    inline bool isSingleplayer = false;
    inline bool serverConnected = false;
    inline Server& serverInstance = Server::getInstance();
    inline std::string serverAddress = "localhost";

    inline std::shared_ptr<Entity> localPlayer;
    inline std::vector<std::shared_ptr<Entity>> entities;

    inline int mouseX = 0;
    inline int mouseY = 0;
}
