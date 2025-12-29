#pragma once

#include "../Packet.hpp"
#include "../../main.hpp"
#include "../../Objects/BlockObject.hpp"
#include "../../Objects/AirObject.hpp"

#define GLM_FORCE_PURE
#include "../../../lib/glm/glm.hpp"
#include "../../../lib/glm/gtc/matrix_transform.hpp"
#include "../../../lib/glm/gtc/type_ptr.hpp"
#include "../../Objects/LightObject.hpp"


class EditChunk : public Packet {
public:
    int id;
    glm::vec3 chunkpos;
    glm::vec3 blockpos;

    void receive(ByteBuf &buffer) override {
        id = buffer.readInt();
        chunkpos.x = buffer.readFloat();
        chunkpos.y = buffer.readFloat();
        chunkpos.z = buffer.readFloat();
        blockpos.x = buffer.readFloat();
        blockpos.y = buffer.readFloat();
        blockpos.z = buffer.readFloat();

        glm::vec3 position;
        position.x = (chunkpos.x * 8.0f) + (blockpos.x);
        position.y = (chunkpos.y * 8.0f) + (blockpos.y);
        position.z = (chunkpos.z * 8.0f) + (blockpos.z);

        if (Main::chunks.find(chunkpos) == Main::chunks.end()) {
            return;
        }

        switch (id) {
            case 0: {
                std::shared_ptr<AirObject> block = std::make_shared<AirObject>(position, glm::vec3(0.0f, 0.0f, 0.0f));
                Main::chunks[chunkpos]->addBlock(blockpos, block);
                break;
            }
            case 1: {
                std::shared_ptr<BlockObject> block = std::make_shared<BlockObject>(position, glm::vec3(0.0f, 0.0f, 0.0f), 0, 1, true, glm::vec3(0.5f, 0.5f, 0.5f));
                Main::chunks[chunkpos]->addBlock(blockpos, block);
                break;
            }
            case 2: {
                std::shared_ptr<BlockObject> block = std::make_shared<BlockObject>(position, glm::vec3(0.0f, 0.0f, 0.0f), 0, 2, true, glm::vec3(0.5f, 0.5f, 0.5f));
                Main::chunks[chunkpos]->addBlock(blockpos, block);
                break;
            }
            case 3: {
                std::shared_ptr<BlockObject> block = std::make_shared<BlockObject>(position, glm::vec3(0.0f, 0.0f, 0.0f), 0, 3, true, glm::vec3(0.5f, 0.5f, 0.5f));
                Main::chunks[chunkpos]->addBlock(blockpos, block);
                break;
            }
            case 4: {
                std::shared_ptr<BlockObject> block = std::make_shared<BlockObject>(position, glm::vec3(0.0f, 0.0f, 0.0f), 0, 4, true, glm::vec3(0.5f, 0.5f, 0.5f));
                Main::chunks[chunkpos]->addBlock(blockpos, block);
                break;
            }
            case 5: {
                std::shared_ptr<LightObject> block = std::make_shared<LightObject>(position, glm::vec3(0.0f, 0.0f, 0.0f), 0, 5, true, glm::vec3(0.5f, 0.5f, 0.5f), 10);
                Main::chunks[chunkpos]->addBlock(blockpos, block);
                break;
            }
        }

        Main::chunks[chunkpos]->initTranslations();
    }

    void send(ByteBuf &buffer) override {
        buffer.writeInt(id);
        buffer.writeFloat(chunkpos.x);
        buffer.writeFloat(chunkpos.y);
        buffer.writeFloat(chunkpos.z);
        buffer.writeFloat(blockpos.x);
        buffer.writeFloat(blockpos.y);
        buffer.writeFloat(blockpos.z);
    }
};
