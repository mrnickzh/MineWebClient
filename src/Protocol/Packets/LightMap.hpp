#pragma once

#include "../Packet.hpp"
#include "../../main.hpp"
#include "../../Objects/AirObject.hpp"
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
                    chunkMap->getBlock(blockpos)->setlight(lights[0], 0);
                    chunkMap->getBlock(blockpos)->setlight(lights[2], 1);
                    chunkMap->getBlock(blockpos)->setlight(lights[4], 2);
                    chunkMap->getBlock(blockpos)->setlight(lights[6], 3);
                    chunkMap->getBlock(blockpos)->setlight(lights[8], 4);
                    chunkMap->getBlock(blockpos)->setlight(lights[10], 5);

                    chunkMap->getBlock(blockpos)->setdarkness(lights[1], 0);
                    chunkMap->getBlock(blockpos)->setdarkness(lights[3], 1);
                    chunkMap->getBlock(blockpos)->setdarkness(lights[5], 2);
                    chunkMap->getBlock(blockpos)->setdarkness(lights[7], 3);
                    chunkMap->getBlock(blockpos)->setdarkness(lights[9], 4);
                    chunkMap->getBlock(blockpos)->setdarkness(lights[11], 5);

                    // if ((chunkpos.x == 0.0f && chunkpos.z == -1.0f) && (lights[0] != 0 || lights[2] != 0 || lights[4] != 0 || lights[6] != 0 || lights[8] != 0 || lights[10] != 0)) {
                    //     printf("%f %f %f block %d %d %d %d %d %d light\n", blockpos.x, blockpos.y, blockpos.z, lights[0], lights[2], lights[4], lights[6], lights[8], lights[10]);
                    // }
                }
            }
        }

        // if (ch > 512) {
        //     // std::cout << chunkpos.x << " " << chunkpos.y << " " << chunkpos.z << std::endl;
        // }

        chunkMap->initBlocks();
    }
    void send(ByteBuf &buffer) override {}
};
