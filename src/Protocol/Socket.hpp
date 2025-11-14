#pragma once
#include <emscripten/websocket.h>
#include "Packet.hpp"

class SocketClient {
public:
    static void sendPacket(Packet* packet);
    static void connect();

    static void on_open();
    static void on_message(std::vector<uint8_t>);
};