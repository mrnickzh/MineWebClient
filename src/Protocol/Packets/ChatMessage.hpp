#pragma once

#include "../Packet.hpp"
#include "../../main.hpp"
#include "../../GUI/Elements/TextElement.hpp"

class ChatMessage : public Packet {
public:
    std::string message;

    void receive(ByteBuf &buffer) override {
        message = buffer.readString();
        if (Main::chatMessages.size() >= 5) { Main::chatMessages.pop_back(); }
        Main::chatMessages.push_front(message);
        for (int i = 0; i < 5; i++) {
            TextElement* chatrow = static_cast<TextElement*>(Main::chatUIManager->getElement("chatrow" + std::to_string(i)).get());
            chatrow->setText(Main::chatMessages[i]);
        }
    }

    void send(ByteBuf &buffer) override {
        buffer.writeString(message);
    }
};
