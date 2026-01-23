#pragma once
#include <deque>
#include <mutex>
#include <emscripten/websocket.h>
#include "Packet.hpp"
#include "Protocol/ClientSession.hpp"

class SocketClient {
public:
    std::mutex clientPacketQueueMutex;
    std::deque<std::pair<ClientSession*, std::vector<uint8_t>>> clientPacketQueue;

    static SocketClient& getInstance() {
        static SocketClient instance;
        return instance;
    }

    void sendPacket(Packet* packet);
    void connect();

    void on_open();
    void on_message(ClientSession*, std::vector<uint8_t>);
};
