//
// Created by onlym on 3/31/2026.
//

#pragma once

#include "../Packet.hpp"
#include "../Socket.hpp"
#include "../../main.hpp"

class NetworkSettingsPacket : public Packet {
public:
    void receive(ByteBuf &buffer) override {
        SocketClient::getInstance().networkSettings.compressionType = static_cast<CompressionType>(buffer.readByte());
        SocketClient::getInstance().networkSettings.compressionThreshold = buffer.readInt();
        SocketClient::getInstance().connectionState = ConnectionState::PLAY; // shoud b changed to KEY_EXCHANGE in futuree


    }
    void send(ByteBuf &buffer) override {}
};
