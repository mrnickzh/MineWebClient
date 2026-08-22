#pragma once
#include <map>
#include <string>

#include "Mod.hpp"

class ModManager {
public:
    std::unordered_map<std::string, Mod*> mods;

    ModManager();
    void initLoad();
    void loadMod(std::string& modname);
};

