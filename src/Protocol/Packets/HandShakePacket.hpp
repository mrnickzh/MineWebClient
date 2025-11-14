#pragma once

#include "../Packet.hpp"
#include <iostream>

class HandShakePacket : public Packet {

public:
    std::string name;

    void send(ByteBuf &buffer) override {
        buffer.writeString(name);
    }

    void receive(ByteBuf &buffer) override {}
};