#pragma once
#include <map>
#include <string>

#include "Mod.hpp"

class ModManager {
public:
    std::map<std::string, Mod*> mods;

    ModManager();
    void initLoad();
    void loadMod(std::string& modname);
};

