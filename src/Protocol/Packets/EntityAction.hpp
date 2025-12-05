#pragma once

#include "../Packet.hpp"
#include "../../Entities/Entity.hpp"
#include <iostream>

#include "../../main.hpp"

class EntityAction : public Packet {

public:
    std::string uuidStr;
    int action;

    void send(ByteBuf &buffer) override {}

    void receive(ByteBuf &buffer) override {
        std::string uuidStr = buffer.readString();
        int action = buffer.readInt();

        switch (action) {
            case 0: {
                std::shared_ptr<Entity> e = std::make_shared<Entity>();
                e->uuid = uuidStr;
                e->object = std::make_shared<EntityObject>(glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), 1, 49, false, glm::vec3(0.25f, 0.75f, 0.25f));
                if (std::find_if(Main::entities.begin(), Main::entities.end(), [uuidStr](std::shared_ptr<Entity> &f)->bool{ return (*f).uuid == uuidStr; }) == Main::entities.end()) {
                    Main::entities.push_back(e);
                    Main::physicsEngine->registerObject(e->object, 1.0f);
                    // std::cout << e->uuid << " uuid" << std::endl;
                    // std::cout << Main::entities.size() << std::endl;
                }
                break;
            }
            case 1: {
                for (std::shared_ptr<Entity>& e : Main::entities) {
                    if (e->uuid == uuidStr) {
                        Main::physicsEngine->unregisterObject(e->object);
                        Main::entities.erase(std::remove(Main::entities.begin(), Main::entities.end(), e), Main::entities.end());
                        break;
                    }
                }
                break;
            }
        }
    }
};
