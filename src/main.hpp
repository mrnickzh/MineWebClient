#pragma once

#include "Shader.hpp"
#include "Objects/Object.hpp"
#include "../MineWebServer/src/Server.hpp"

namespace Main {
    inline Shader* ourShader;

    inline std::vector<std::shared_ptr<Object>> objects;

    inline bool isSingleplayer = false;
    inline Server& serverInstance = Server::getInstance();
}
