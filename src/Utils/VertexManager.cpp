#include "VertexManager.hpp"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../../lib/stb/stb_image.hpp"

void VertexManager::initVAO(int vuid, float vertices[], int length, std::string texturepath) {
    VertexUnit vunit(0, 0, 0);
    int width, height, nrChannels;

    glGenVertexArrays(1, &vunit.vao);
    glBindVertexArray(vunit.vao);

    glGenBuffers(1, &vunit.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vunit.vbo);
    glBufferData(GL_ARRAY_BUFFER, length, vertices, GL_STATIC_DRAW);

    glGenTextures(1, &vunit.texture);
    glBindTexture(GL_TEXTURE_2D, vunit.texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(texturepath.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    vertexunits[vuid] = vunit;
};

VertexUnit VertexManager::getVAO(int vuid) {
    return vertexunits[vuid];
};