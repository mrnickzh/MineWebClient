#pragma once

#include "lib/json/json.hpp"
#include "lib/sol/sol.hpp"

#include <fstream>
#include <iostream>

struct ModBlock {
    std::string texture;
    bool cancollide;
};

class Mod {
public:
    std::string modName;
    sol::state mainLua;

    std::unordered_map<std::string, ModBlock> modBlocks;
    std::unordered_map<std::string, sol::function> modEvents;

    Mod(std::string modName);
    void loadAssets();
    void loadBlock(std::string blockId, int id);
    void loadMainLua();
    void registerEvent(std::string event, sol::protected_function func);
    void doEvent(std::string event);
};
