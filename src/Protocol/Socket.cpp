#include <emscripten/websocket.h>
#include <cstdio>
#include "Socket.hpp"

#include "PacketHelper.hpp"
#include "../main.hpp"
#include "Packets/HandShakePacket.hpp"

EMSCRIPTEN_WEBSOCKET_T ws = 0;
static ClientSession* localSession = new ClientSession("localhost");

void SocketClient::on_open() {
    Main::serverConnected = true;

    if (Main::isSingleplayer) {
        Main::serverInstance.clients[localSession] = nullptr;
    }

    HandShakePacket pkt;
    pkt.name = "player";
    sendPacket(&pkt);
}

void SocketClient::on_message(ClientSession* session, std::vector<uint8_t> data) {
    if (Main::isSingleplayer) {
        std::lock_guard<std::mutex> guard(clientPacketQueueMutex);
        clientPacketQueue.push_back(std::pair<ClientSession*, std::vector<uint8_t>>(session, data));
    }
    else {
        PacketHelper::decodePacket(data);
    }
}

EM_BOOL remote_open(int type, const EmscriptenWebSocketOpenEvent *e, void *userData) {
    SocketClient::getInstance().on_open();
    return EM_TRUE;
}

EM_BOOL remote_message(int type, const EmscriptenWebSocketMessageEvent *e, void *userData) {
    std::vector<uint8_t> vec(e->data, e->data + e->numBytes);
    SocketClient::getInstance().on_message(localSession, vec);
    return EM_TRUE;
}

void SocketClient::connect() {
    if (Main::serverAddress == "localhost") {
        Main::isSingleplayer = true;
        Main::serverInstance.setCallback([&](ClientSession* session, std::vector<uint8_t> data){on_message(session, data);});
        on_open();
        return;
    }

    EmscriptenWebSocketCreateAttributes attr = {
            .url = Main::serverAddress.c_str(),
            .protocols = NULL,
            .createOnMainThread = EM_FALSE
    };

    ws = emscripten_websocket_new(&attr);

    emscripten_websocket_set_onopen_callback(ws, NULL, remote_open);
    emscripten_websocket_set_onmessage_callback(ws, NULL, remote_message);
}

void SocketClient::sendPacket(Packet* packet) {
    std::vector<uint8_t> data = PacketHelper::encodePacket(packet);
    if (Main::isSingleplayer) {
        Main::serverInstance.processPacket(localSession, data);
    }
    else {
        emscripten_websocket_send_binary(ws, data.data(), static_cast<uint32_t>(data.size()));
    }
}