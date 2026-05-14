#pragma once

#include "../Packet.hpp"
#include "../../main.hpp"
#include "../../Objects/AirObject.hpp"
#include "../../Objects/BlockObject.hpp"
#include "Utils/ZipUtils.hpp"

class TransferMod : public Packet {
    public:
        std::string modName;

    void receive(ByteBuf &buffer) override {
        modName = buffer.readString();
        int size = buffer.readInt();

        std::string modPath = "/mods/" + modName + ".zip";

        std::vector<uint8_t> data = buffer.toByteArray(size);

        std::cout << "mod size: " << data.size() << "\n";

        std::ofstream file(modPath, std::ios::binary);
        file.write(reinterpret_cast<const char*>(data.data()), data.size());
        file.close();

        ZipUtils::extract_all(modPath, "/mods/" + modName + "/");

        Main::modManager->loadMod(modName);
    }
    void send(ByteBuf &buffer) override {

    }
};
