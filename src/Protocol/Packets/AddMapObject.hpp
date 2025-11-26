#pragma once

#include "../Packet.hpp"
#include "../../main.hpp"
#include "../../Objects/BlockObject.hpp"
#include "../../Objects/AirObject.hpp"

#define GLM_FORCE_PURE
#include "../../../lib/glm/glm.hpp"
#include "../../../lib/glm/gtc/matrix_transform.hpp"
#include "../../../lib/glm/gtc/type_ptr.hpp"


class AddMapObject : public Packet {
    void receive(ByteBuf &buffer) override {
        int id = buffer.readInt();
        glm::vec3 position;
        glm::vec3 rotation;
        position.x = buffer.readFloat();
        position.y = buffer.readFloat();
        position.z = buffer.readFloat();
        rotation.x = buffer.readFloat();
        rotation.y = buffer.readFloat();
        rotation.z = buffer.readFloat();

        switch (id) {
            case 0:
                Main::objects.push_back(std::make_shared<AirObject>(position, rotation));
                break;
            case 1:
                Main::objects.push_back(std::make_shared<BlockObject>(position, rotation, 0, 1, true, glm::vec3(0.5f, 0.5f, 0.5f)));
                break;
            case 2:
                Main::objects.push_back(std::make_shared<BlockObject>(position, rotation, 0, 2, true, glm::vec3(0.5f, 0.5f, 0.5f)));
                break;
            case 3:
                Main::objects.push_back(std::make_shared<BlockObject>(position, rotation, 0, 3, true, glm::vec3(0.5f, 0.5f, 0.5f)));
                break;
        }
    }

    void send(ByteBuf &buffer) override {}
};