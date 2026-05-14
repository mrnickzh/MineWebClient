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

    std::map<std::string, ModBlock> modBlocks;

    Mod(std::string modName);
    void loadAssets();
    void loadBlock(std::string blockId, int id);
    void loadMainLua();
    void doEvent(std::string event);
};
