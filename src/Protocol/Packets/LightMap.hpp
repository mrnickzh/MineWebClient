#pragma once

#include "../Packet.hpp"
#include "../../main.hpp"
#include "../../Objects/BlockObject.hpp"

class LightMap : public Packet {
    public:
        glm::vec3 chunkpos;

    void receive(ByteBuf &buffer) override {
        chunkpos = glm::vec3(buffer.readFloat(), buffer.readFloat(), buffer.readFloat());
        // printf("%f %f %f chunkpos\n", chunkpos.x, chunkpos.y, chunkpos.z);
        if (!Main::chunks.count(chunkpos)) {
            return;
        }
        std::shared_ptr<ChunkMap> chunkMap = Main::chunks[chunkpos];
        // int ch = 0;

        for (int x = 0; x < 8; x++) {
            for (int y = 0; y < 8; y++) {
                for (int z = 0; z < 8; z++) {
                    glm::vec3 blockpos = glm::vec3(buffer.readFloat(), buffer.readFloat(), buffer.readFloat());
                    int lights[12] = {buffer.readInt(), buffer.readInt(), buffer.readInt(), buffer.readInt(), buffer.readInt(), buffer.readInt(), buffer.readInt(), buffer.readInt(), buffer.readInt(), buffer.readInt(), buffer.readInt(), buffer.readInt()};
                    // ch += 1;
                    Object object = chunkMap->getBlock(blockpos);

                    for (int i = 0; i < 6; i++) {
                        object.lightLevels[i].x = (float)lights[i * 2] / 10.0f;
                        object.lightLevels[i].y = (float)lights[i * 2 + 1] / 10.0f;
                    }

                    // if ((chunkpos.x == 0.0f && chunkpos.y == 0.0f && chunkpos.z == 0.0f) && (lights[1] != -5 || lights[3] != -5 || lights[5] != -5 || lights[7] != -5 || lights[9] != -5 || lights[11] != -5)) {
                    //     printf("%f %f %f block %f %f %f %f %f %f light\n", blockpos.x, blockpos.y, blockpos.z, block->lightLevels[0].y, block->lightLevels[1].y, block->lightLevels[2].y, block->lightLevels[3].y, block->lightLevels[4].y, block->lightLevels[5].y);
                    // }

                    chunkMap->addBlock(blockpos, object);
                }
            }
        }

        chunkMap->initLights();

        // if (ch > 512) {
        //     // std::cout << chunkpos.x << " " << chunkpos.y << " " << chunkpos.z << std::endl;
        // }
    }
    void send(ByteBuf &buffer) override {}
};
