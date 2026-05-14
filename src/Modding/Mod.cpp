#include "Mod.hpp"

#include "../main.hpp"

Mod::Mod(std::string modName) {
    this->modName = modName;
}

void Mod::loadAssets() {
    nlohmann::json assets;
    std::ifstream manifestfile("/mods/" + modName + "/manifest.json");

    if (!manifestfile) {
        std::cout << "mod " + modName + " assets manifest not found" << std::endl;
    } else manifestfile >> assets;

    for (auto& element : assets["blocks"].items()) {
        nlohmann::json block = element.value();
        std::string id = block["id"];
        modBlocks[id] = ModBlock(block["texture"], block["cancollide"]);
        // std::string texture = block["texture"];
        // std::cout << "/mods/" + modName + "/assets/" + texture << std::endl;
        // Main::textureManager->addTexture("/mods/" + modName + "/assets/" + texture, id);
    }

    // for (auto& element : assets["entities"].items()) {
    //     nlohmann::json entity = element.value();
    //     int id = entity["id"];
    //     std::string texture = entity["texture"];
    //     Main::textureManager->addTexture("/mods/" + modName + "/assets/" + texture, id);
    // }
}

void Mod::loadBlock(std::string blockId, int id) {
    ModBlock block = modBlocks[blockId];
    Main::textureManager->addTexture("/mods/" + modName + "/assets/" + block.texture, id);
}

void Mod::loadMainLua() {
    mainLua.open_libraries(sol::lib::base);

    std::ifstream file("/mods/" + modName + "/client/main.lua");
    std::stringstream script;
    script << file.rdbuf();

    mainLua.script(script.str());
}

void Mod::doEvent(std::string event) {
    std::cout << event << std::endl;
    sol::protected_function func = mainLua[event];
    sol::protected_function_result result = func();
    if (!result.valid()) {
        sol::error err = result;
        std::cerr << "Error: " << err.what() << std::endl;
    }
}


