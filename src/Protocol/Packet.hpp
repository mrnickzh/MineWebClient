#pragma once

#include "Utils/ByteBuf.hpp"

class Packet {
public:
    virtual ~Packet() = default;
    virtual void send(ByteBuf& buffer) = 0;
    virtual void receive(ByteBuf& buffer) = 0;
};
