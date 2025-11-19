#pragma once

#include "../Packet.hpp"
#include "../../main.hpp"
#include "../../Objects/AirObject.hpp"
#include "../../Objects/BlockObject.hpp"

class GenerateChunk : public Packet {
    public:
        glm::vec3 chunkpos;

    void receive(ByteBuf &buffer) override {
        std::shared_ptr<ChunkMap> chunkMap = std::make_shared<ChunkMap>();
        chunkpos = glm::vec3(buffer.readFloat(), buffer.readFloat(), buffer.readFloat());
        // int ch = 0;

        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                for (int z = 0; z < 8; z++) {
                    int id = buffer.readInt();
                    glm::vec3 position;
                    glm::vec3 blockpos = glm::vec3(buffer.readFloat(), buffer.readFloat(), buffer.readFloat());
                    position.x = (chunkpos.x * 8.0f) + (blockpos.x);
                    position.y = (chunkpos.y * 8.0f) + (blockpos.y);
                    position.z = (chunkpos.z * 8.0f) + (blockpos.z);

                    // ch += 1;

                    switch (id) {
                        case 0: {
                            std::shared_ptr<AirObject> airObject = std::make_shared<AirObject>(position, glm::vec3(0.0f, 0.0f, 0.0f));
                            chunkMap->addBlock(glm::vec3(blockpos.x, blockpos.y, blockpos.z), airObject);
                            break;
                        }
                        case 1: {
                            std::shared_ptr<BlockObject> blockObject = std::make_shared<BlockObject>(position, glm::vec3(0.0f, 0.0f, 0.0f), 1);
                            chunkMap->addBlock(glm::vec3(blockpos.x, blockpos.y, blockpos.z), blockObject);
                            break;
                        }
                        case 2: {
                            std::shared_ptr<BlockObject> blockObject = std::make_shared<BlockObject>(position, glm::vec3(0.0f, 0.0f, 0.0f), 2);
                            chunkMap->addBlock(glm::vec3(blockpos.x, blockpos.y, blockpos.z), blockObject);
                            break;
                        }
                        case 3: {
                            std::shared_ptr<BlockObject> blockObject = std::make_shared<BlockObject>(position, glm::vec3(0.0f, 0.0f, 0.0f), 3);
                            chunkMap->addBlock(glm::vec3(blockpos.x, blockpos.y, blockpos.z), blockObject);
                            break;
                        }
                    }
                }
            }
        }

        // if (ch > 256) {
        //     std::cout << chunkpos.x << " " << chunkpos.y << std::endl;
        // }
        Main::chunks[chunkpos] = chunkMap;
    }
    void send(ByteBuf &buffer) override {
        buffer.writeFloat(chunkpos.x);
        buffer.writeFloat(chunkpos.y);
        buffer.writeFloat(chunkpos.z);
    }
};
