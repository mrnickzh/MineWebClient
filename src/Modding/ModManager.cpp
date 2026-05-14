#include "ModManager.hpp"

#include "Utils/ZipUtils.hpp"

ModManager::ModManager() {

}

void ModManager::initLoad() {
    for (const auto& entry : std::filesystem::directory_iterator("/mods")) {
        if (entry.path().extension().string() == ".zip") {
            std::string modName = entry.path().stem().string();
            ZipUtils::extract_all(entry.path().string(), "/mods/" + modName + "/");
            loadMod(modName);
        }
    }
}

void ModManager::loadMod(std::string& modName) {
    Mod* mod = new Mod(modName);
    mod->loadAssets();
    mod->loadMainLua();
    mods[modName] = mod;
    std::cout << "Mod " << modName << " loaded" << std::endl;
}

