#pragma once

#include "../Packet.hpp"
#include "../../main.hpp"
#include "../../Objects/AirObject.hpp"
#include "../../Objects/BlockObject.hpp"
#include "Utils/ZipUtils.hpp"

class RegisterMod : public Packet {
    public:
        std::string modName;

    void receive(ByteBuf &buffer) override {
        modName = buffer.readString();
        int count = buffer.readInt();

        auto it = Main::modManager->mods.find(modName);
        if (it == Main::modManager->mods.end()) {
            printf("No mod named %s\n", modName.c_str());
        }
        for (int i = 0; i < count; i++) {
            std::string blockId = buffer.readString();
            int id = buffer.readInt();
            it->second->loadBlock(blockId, id);
        }
    }
    void send(ByteBuf &buffer) override {

    }
};
