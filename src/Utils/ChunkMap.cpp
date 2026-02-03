#include "ChunkMap.hpp"

#include "../main.hpp"
#include "../Objects/AirObject.hpp"
#include "../Objects/LightObject.hpp"

float sidevertices[] = {
    // Back face
    -4.0f, -4.0f, -4.0f,  0.0f, 0.0f, // Bottom-left
     4.0f,  4.0f, -4.0f,  0.25f, 0.25f, // top-right
     4.0f, -4.0f, -4.0f,  0.25f, 0.0f, // bottom-right
     4.0f,  4.0f, -4.0f,  0.25f, 0.25f, // top-right
    -4.0f, -4.0f, -4.0f,  0.0f, 0.0f, // bottom-left
    -4.0f,  4.0f, -4.0f,  0.0f, 0.25f, // top-left
    // Front face
    -4.0f, -4.0f,  4.0f,  0.0f, 0.0f, // bottom-left
     4.0f, -4.0f,  4.0f,  0.25f, 0.0f, // bottom-right
     4.0f,  4.0f,  4.0f,  0.25f, 0.25f, // top-right
     4.0f,  4.0f,  4.0f,  0.25f, 0.25f, // top-right
    -4.0f,  4.0f,  4.0f,  0.0f, 0.25f, // top-left
    -4.0f, -4.0f,  4.0f,  0.0f, 0.0f, // bottom-left
    // Left face
    -4.0f,  4.0f,  4.0f,  0.25f, 0.0f, // top-right
    -4.0f,  4.0f, -4.0f,  0.25f, 0.25f, // top-left
    -4.0f, -4.0f, -4.0f,  0.0f, 0.25f, // bottom-left
    -4.0f, -4.0f, -4.0f,  0.0f, 0.25f, // bottom-left
    -4.0f, -4.0f,  4.0f,  0.0f, 0.0f, // bottom-right
    -4.0f,  4.0f,  4.0f,  0.25f, 0.0f, // top-right
    // Right face
     4.0f,  4.0f,  4.0f,  0.25f, 0.0f, // top-left
     4.0f, -4.0f, -4.0f,  0.0f, 0.25f, // bottom-right
     4.0f,  4.0f, -4.0f,  0.25f, 0.25f, // top-right
     4.0f, -4.0f, -4.0f,  0.0f, 0.25f, // bottom-right
     4.0f,  4.0f,  4.0f,  0.25f, 0.0f, // top-left
     4.0f, -4.0f,  4.0f,  0.0f, 0.0f, // bottom-left
    // Bottom face
    -4.0f, -4.0f, -4.0f,  0.0f, 0.25f, // top-right
     4.0f, -4.0f, -4.0f,  0.25f, 0.25f, // top-left
     4.0f, -4.0f,  4.0f,  0.25f, 0.0f, // bottom-left
     4.0f, -4.0f,  4.0f,  0.25f, 0.0f, // bottom-left
    -4.0f, -4.0f,  4.0f,  0.0f, 0.0f, // bottom-right
    -4.0f, -4.0f, -4.0f,  0.0f, 0.25f, // top-right
    // Top face
    -4.0f,  4.0f, -4.0f,  0.0f, 0.25f, // top-left
     4.0f,  4.0f,  4.0f,  0.25f, 0.0f, // bottom-right
     4.0f,  4.0f, -4.0f,  0.25f, 0.25f, // top-right
     4.0f,  4.0f,  4.0f,  0.25f, 0.0f, // bottom-right
    -4.0f,  4.0f, -4.0f,  0.0f, 0.25f, // top-left
    -4.0f,  4.0f,  4.0f,  0.0f, 0.0f  // bottom-left
};

bool checkValidPos(glm::vec3 pos) {
    if (pos.x >= 0.0f && pos.y >= 0.0f && pos.z >= 0.0f && pos.x <= 7.0f && pos.y <= 7.0f && pos.z <= 7.0f) {
        return true;
    }
    return false;
}

ChunkMap::ChunkMap() {
    glGenVertexArrays(1, &VAO);

    glGenTextures(1, &chunkTexture);
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, chunkTexture);

    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    memset(sides, 0, sizeof(sides));
    memset(translations, 0, sizeof(translations));
    memset(textures, 0, sizeof(textures));
    instanceCount = 0;

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 30 * 6 * 512, &sides[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &IVBO);
    glBindBuffer(GL_ARRAY_BUFFER, IVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * 512 * 6 * 6, &translations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &TBO);
    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 512 * 6 * 6, &textures[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}


void ChunkMap::addBlock(glm::vec3 blockPos, std::shared_ptr<Object> block) {
    blocks[(int)(blockPos.x * 64.0f + blockPos.y * 8.0f + blockPos.z)] = block;
}

std::shared_ptr<Object> ChunkMap::getBlock(glm::vec3 blockPos) {
    return blocks[(int)(blockPos.x * 64.0f + blockPos.y * 8.0f + blockPos.z)];
}

void ChunkMap::initTranslations() {
    memset(sides, 0, sizeof(sides));
    memset(translations, 0, sizeof(translations));
    memset(textures, 0, sizeof(textures));
    instanceCount = 0;

    initBlocks();

    // glTexSubImage3D(GL_TEXTURE_3D,
    //     0,
    //     0, 0, 0,
    //     8, 8, 8,
    //     GL_RGBA,
    //     GL_UNSIGNED_SHORT,
    //     blocksdata);

    for (int side = 0; side < 6; side++) {
        // if (side != 0) { continue; }
        int direction[6] = {0, 0, 0, 0, 0, 0};
        direction[side] = 1;

        bool mask[64];
        memset(mask, 0, sizeof(mask));

        int nside = (int)floor((float)side / 2);

        float x;
        float y;
        float z;
        float cx;
        float cy;
        float cz;

        for (int s = 0; s < 8; s++) {
            int n = 0;
            memset(mask, 0, sizeof(mask));
            for (int a = 0; a < 8; a++) {
                for (int b = 0; b < 8; b++) {
                    switch (nside) {
                        case 0:
                            x = (float)(a + direction[3] + (direction[2] * -1));
                            y = (float)(b + direction[5] + (direction[4] * -1));
                            z = (float)(s + direction[1] + (direction[0] * -1));
                            cx = a;
                            cy = b;
                            cz = s;
                            break;
                        case 1:
                            x = (float)(s + direction[3] + (direction[2] * -1));
                            y = (float)(a + direction[5] + (direction[4] * -1));
                            z = (float)(b + direction[1] + (direction[0] * -1));
                            cx = s;
                            cy = a;
                            cz = b;
                            break;
                        case 2:
                            x = (float)(a + direction[3] + (direction[2] * -1));
                            y = (float)(s + direction[5] + (direction[4] * -1));
                            z = (float)(b + direction[1] + (direction[0] * -1));
                            cx = a;
                            cy = s;
                            cz = b;
                            break;
                    }
                    glm::vec3 pos = glm::vec3(x, y, z);
                    bool bcurrent = getBlock(glm::vec3(cx, cy, cz))->cancollide;
                    bool bcompare = (checkValidPos(pos) && getBlock(pos)->cancollide);
                    // std::cout << pos.x << ", " << pos.y << ", " << pos.z << ", " << (bcurrent && !bcompare) << std::endl;
                    mask[n++] = bcurrent && !bcompare;
                }
            }

            // std::string kek;
            //
            // for (int iii = 0; iii < 8; iii++) {
            //     for (int jjj = 0; jjj < 8; jjj++) {
            //         kek += std::to_string(mask[iii * 8 + jjj]) + " ";
            //     }
            //     kek += "\n";
            // }
            //
            // std::cout << kek << std::endl;

            int w, h, l, k;
            n = 0;

            // std::cout << "start" << std::endl;

            for (int c = 0; c < 8; c++) {
                for (int d = 0; d < 8; ) {
                    if (mask[n]) {
                        for (w = 1; d + w < 8 && mask[n + w]; w++) { }

                        bool done = false;
                        for (h = 1; c + h < 8; h++)
                        {
                            for (k = 0; k < w; ++k)
                            {
                                if (!mask[n + k + h * 8])
                                {
                                    done = true;
                                    break;
                                }
                            }

                            if (done)
                                break;
                        }

                        // std::cout << d << ", " << w << ", " << c << ", " << h << std::endl;

                        for (int jj = 0; jj < 6; jj++) {

                            std::vector<int> nsides = {2, 0, 1};
                            int realnside = nsides[nside];
                            nsides.erase(nsides.begin() + nside);

                            if (sidevertices[30*side + (5 * jj) + realnside] < 0) {
                                sides[(30 * instanceCount) + (5 * jj) + realnside] = float(s) - 4.0f;
                                // std::cout << sides[(30 * instanceCount) + (5 * jj) + realnside] << " -n" << realnside << std::endl;
                            }
                            if (sidevertices[30*side + (5 * jj) + realnside] > 0) {
                                sides[(30 * instanceCount) + (5 * jj) + realnside] = float(s) - 4.0f + 1.0f;
                                // std::cout << sides[(30 * instanceCount) + (5 * jj) + realnside] << " +n" << realnside << std::endl;
                            }

                            bool flip = false;
                            if (nside == 0) {
                                flip = true;
                            }

                            for (int ns : nsides) {
                                if (sidevertices[30*side + (5 * jj) + ns] < 0) {
                                    sides[(30 * instanceCount) + (5 * jj) + ns] = sidevertices[30*side + (5 * jj) + ns] + ((float)c * (float)flip) + ((float)d * (float)!flip);
                                    // std::cout << sides[(30 * instanceCount) + (5 * jj) + ns] << " -" << ns << std::endl;
                                }
                                if (sidevertices[30*side + (5 * jj) + ns] > 0) {
                                    sides[(30 * instanceCount) + (5 * jj) + ns] = sidevertices[30*side + (5 * jj) + ns] - ((8.0f - ((float)c + (float)h)) * (float)flip) - ((8.0f - ((float)d + (float)w)) * (float)!flip);
                                    // std::cout << sides[(30 * instanceCount) + (5 * jj) + ns] << " +" << ns << std::endl;
                                }
                                flip = !flip;
                            }

                            sides[(30 * instanceCount) + (5 * jj) + 3] = sidevertices[30*side + (5 * jj) + 3];
                            sides[(30 * instanceCount) + (5 * jj) + 4] = sidevertices[30*side + (5 * jj) + 4];
                        }
                        for (int ii = 0; ii < 6; ii++) {
                            translations[6 * instanceCount + ii] =  glm::translate(glm::mat4(1.0f), glm::vec3(getBlock(glm::vec3(0.0f, 0.0f, 0.0f))->position.x + 3.5f, getBlock(glm::vec3(0.0f, 0.0f, 0.0f))->position.y + 3.5f, getBlock(glm::vec3(0.0f, 0.0f, 0.0f))->position.z + 3.5f));
                            textures[6 * instanceCount + ii] = (float)side;
                        }
                        instanceCount++;

                        for (l = 0; l < h; ++l) {
                            for (k = 0; k < w; ++k) {
                                mask[n + k + l * 8] = false;
                            }
                        }

                        d += w;
                        n += w;
                    }
                    else
                    {
                        d++;
                        n++;
                    }
                }

            }
        }
    }

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 30 * 6 * 512, &sides[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &IVBO);
    glBindBuffer(GL_ARRAY_BUFFER, IVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4) * 512 * 6 * 6, &translations[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &TBO);
    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 512 * 6 * 6, &textures[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void ChunkMap::initBlocks() {
    uint16_t blocksdata[512*4];

    int ch = 0;
    for (int by = 0; by < 8; by++) {
        for (int bx = 0; bx < 8; bx++) {
            for (int bz = 0; bz < 8; bz++) {
                glm::vec3 pos = glm::vec3((float)bx, (float)by, (float)bz);

                uint16_t result[4] = {0, 0, 0, 0};
                uint64_t* pack = (uint64_t*)result;

                *pack |= ((uint64_t)getBlock(pos)->texture & 0xff);

                int shift = 16;
                for (int i = 0; i < 3; i++) {
                    for (int j = 0; j < 2; j++) {
                        *pack |= ((uint64_t)((int)(getBlock(pos)->lightLevels[i * 2 + j].x * 5)) & 0x7) << (shift + (6 * j));
                        *pack |= ((uint64_t)(abs((int)(getBlock(pos)->lightLevels[i * 2 + j].y * 5))) & 0x7) << (shift + (6 * j) + 3);
                    }
                    shift += 16;
                }

                blocksdata[4 * ch + 0] = result[0];
                blocksdata[4 * ch + 1] = result[1];
                blocksdata[4 * ch + 2] = result[2];
                blocksdata[4 * ch + 3] = result[3];

                ch++;
            }
        }
    }

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, chunkTexture);
    glTexImage3D(GL_TEXTURE_3D, 0, GL_RGBA16UI, 8, 8, 8, 0, GL_RGBA_INTEGER, GL_UNSIGNED_SHORT, blocksdata);
}

void ChunkMap::renderChunk() {
    glBindVertexArray(VAO);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_3D, chunkTexture);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, IVBO);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(0));
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(4 * sizeof(float)));
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(8 * sizeof(float)));
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(12 * sizeof(float)));
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // for (int i = 0; i < instanceCount; i++) {
    //     // if (translations[i].x == 0.0f && translations[i].y == 0.0f && translations[i].z == 0.0f) {
    //         std::cout << translations[i].x << " " << translations[i].y << " " << translations[i].z << " " << translations[i].w << " " << i << std::endl;
    //     // }
    // }

    // glDrawArraysInstanced(GL_TRIANGLES, 0, 36, instanceCount);
    // for (int i = 0; i < instanceCount; i++) {
    glDrawArrays(GL_TRIANGLES, 0, 6 * instanceCount);
    // }

    glBindVertexArray(0);
}

bool ChunkMap::checkCull(Frustum& frustum, glm::vec3 chunkpos) {
    glm::vec3 AA = glm::vec3(chunkpos.x * 8.0f, chunkpos.y * 8.0f, chunkpos.z * 8.0f);
    glm::vec3 BB = glm::vec3(chunkpos.x * 8.0f + 8.0f, chunkpos.y * 8.0f + 8.0f, chunkpos.z * 8.0f + 8.0f);

    glm::vec3 corners[8] = {glm::vec3(AA.x, AA.y, AA.z), glm::vec3(BB.x, BB.y, BB.z), glm::vec3(BB.x, AA.y, AA.z), glm::vec3(AA.x, BB.y, AA.z), glm::vec3(AA.x, AA.y, BB.z), glm::vec3(BB.x, BB.y, AA.z), glm::vec3(BB.x, AA.y, BB.z), glm::vec3(AA.x, BB.y, BB.z)};
    Plane planes[6] = {frustum.farFace, frustum.nearFace, frustum.bottomFace, frustum.topFace, frustum.leftFace, frustum.rightFace};

    for (int i = 0; i < 6; i++) {
        int out = 0;
        Plane frustumPlane = planes[i];
        for (int j = 0; j < 8; j++) {
            glm::vec3 point = corners[j];
            float result = glm::dot(frustumPlane.normal, point) - frustumPlane.distance;
            out += (result < 0.0f ? 1 : 0);
        }
        if (out == 8) { return false; }
    }

    return true;
}

