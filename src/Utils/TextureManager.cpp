#include "TextureManager.hpp"

#include <iostream>

#define STB_IMAGE_IMPLEMENTATION
#include "../../lib/stb/stb_image.hpp"

void TextureManager::startInit(int w, int h, int size) {
    textureArray = 0;
    glGenTextures(1, &textureArray);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);

    glTexStorage3D(GL_TEXTURE_2D_ARRAY,
        4,                    //5 mipmaps
        GL_RGB8,               //Internal format
        w, h, size           //width,height
    );

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
}

void TextureManager::addTexture(std::string texturepath, int textureoffset) {
    int width, height, nrChannels;

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(texturepath.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
        0,                      //Mipmap number
        0, 0, textureoffset, //xoffset, yoffset, zoffset
        width, height, 1,          //width, height, depth
        GL_RGB,                 //format
        GL_UNSIGNED_BYTE,       //type
        data); //pointer to data
        glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
};

void TextureManager::endInit() {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D_ARRAY, textureArray);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
    glGenerateMipmap(GL_TEXTURE_2D_ARRAY);
}