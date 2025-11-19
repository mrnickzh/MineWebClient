#pragma once

#include <functional>
#include <map>
#include <vector>
#include "Packet.hpp"
#include "Utils/ByteBuf.hpp"
#include <iostream>

namespace PacketHelper {

    inline std::map<int, std::function<Packet*()>> packetCreators;

    inline int getPacketId(Packet* packet) {
        for (const auto& pair : packetCreators) {
            if (typeid(*packet) == typeid(*pair.second())) {

                return pair.first;
            }
        }
        return -1;
    }

    inline void registerPacket(int id, std::function<Packet*()> creator) {
        packetCreators[id] = creator;
    }

    inline Packet* createPacket(int id) {
        if (packetCreators.find(id) != packetCreators.end()) {
            return packetCreators[id]();
        }
        return nullptr;
    }

    inline std::vector<uint8_t> encodePacket(Packet* packet) {
        ByteBuf buffer(65536);
        int id = getPacketId(packet);
        buffer.writeInt(id);
        packet->send(buffer);
        return buffer.toByteArray();
    }

    inline Packet* decodePacket(const std::vector<uint8_t> data) {
        ByteBuf buffer(65536);

        buffer.fromByteArray(data);
        int id = buffer.readInt();
        Packet* packet = createPacket(id);
        if (packet) {
            packet->receive(buffer);
        }
        return packet;
    }
}