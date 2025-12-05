#pragma once

#include "../Packet.hpp"
#include "../../main.hpp"

class PlayerAuthInput : public Packet {
public:
    void receive(ByteBuf &buffer) override {
       std::string uuid = buffer.readString();
        std::cout << uuid << " uuid" << std::endl;
        std::cout << Main::entities.size() << std::endl;
       for (auto& e : Main::entities) {
           if (e->uuid == uuid) {
               float px = buffer.readFloat();
               float py = buffer.readFloat();
               float pz = buffer.readFloat();
               e->object->setposition(glm::vec3(px, py, pz));
               float rx = buffer.readFloat();
               float ry = buffer.readFloat();
               float rz = buffer.readFloat();
               e->object->setrotation(glm::vec3(rx, ry, rz));
               float vx = buffer.readFloat();
               float vy = buffer.readFloat();
               float vz = buffer.readFloat();
               glm::vec3 velocity = glm::vec3(vx, vy, vz);
               Main::physicsEngine->setVelocity(e->object, velocity);
           }
       }
    }

    void send(ByteBuf &buffer) override {
        buffer.writeFloat(Main::localPlayer->object->position.x);
        buffer.writeFloat(Main::localPlayer->object->position.y);
        buffer.writeFloat(Main::localPlayer->object->position.z);

        buffer.writeFloat(Main::localPlayer->object->rotation.x);
        buffer.writeFloat(Main::localPlayer->object->rotation.y);
        buffer.writeFloat(Main::localPlayer->object->rotation.z);

        glm::vec3 velocity = Main::physicsEngine->getVelocity(Main::localPlayer->object);
        buffer.writeFloat(velocity.x);
        buffer.writeFloat(velocity.y);
        buffer.writeFloat(velocity.z);
    }
};