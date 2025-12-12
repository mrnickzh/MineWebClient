#include "FontManager.hpp"

#include <fstream>
#include <iostream>
#include <algorithm>
#include <map>

#define STB_TRUETYPE_IMPLEMENTATION

#include "../../lib/stb/stb_truetype.h"

struct FontData
{
    uint32_t size = 20;
    const uint32_t atlasWidth = 2048;
    const uint32_t atlasHeight = 2048;
    const uint32_t oversampleX = 2;
    const uint32_t oversampleY = 2;
    const uint32_t firstChar = ' ';
    const uint32_t charCount = '~' - ' ';
    std::unique_ptr<stbtt_packedchar[]> charInfo;
};

std::map<int, std::shared_ptr<FontData>> fonts;

int FontManager::offsetFromSize(int size) {
    int offset = 0;
    for (auto& font : fonts) {
        if (font.first == size) {
            return offset;
        }
        offset++;
    }
}

void FontManager::init(const std::string& path, std::vector<int>& sizes) {
    std::ifstream file(path, std::ios::binary);
    std::vector<uint8_t> fontData;

    if (file) {
        fontData = std::vector<uint8_t>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }

    fontArray = 0;
    glGenTextures(1, &fontArray);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_ARRAY, fontArray);

    glTexStorage3D(GL_TEXTURE_2D_ARRAY,
        1,                    //5 mipmaps
        GL_R8,               //Internal format
        2048, 2048, (int)sizes.size()           //width,height
    );

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    for (int i = 0; i < sizes.size(); i++) {
        std::shared_ptr<FontData> font = std::make_shared<FontData>();
        font->size = sizes[i];
        fonts.insert(std::make_pair(sizes[i], font));

        uint8_t atlasData[fonts[sizes[i]]->atlasWidth * fonts[sizes[i]]->atlasHeight];

        fonts[sizes[i]]->charInfo = std::make_unique<stbtt_packedchar[]>(fonts[sizes[i]]->charCount);

        stbtt_pack_context context;
        if (!stbtt_PackBegin(&context, &atlasData[0], (int)fonts[sizes[i]]->atlasWidth, (int)fonts[sizes[i]]->atlasHeight, 0, 1, nullptr))
            std::cout << "Failed to initialize font" << std::endl;

        stbtt_PackSetOversampling(&context, fonts[sizes[i]]->oversampleX, fonts[sizes[i]]->oversampleY);
        if (!stbtt_PackFontRange(&context, fontData.data(), 0, (float)fonts[sizes[i]]->size, (int)fonts[sizes[i]]->firstChar, (int)fonts[sizes[i]]->charCount, fonts[sizes[i]]->charInfo.get()))
            std::cout << "Failed to pack font" << std::endl;

        stbtt_PackEnd(&context);

        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D_ARRAY, fontArray);

        glTexSubImage3D(GL_TEXTURE_2D_ARRAY,
                0,                      //Mipmap number
                0, 0, i, //xoffset, yoffset, zoffset
                (int)fonts[sizes[i]]->atlasWidth, (int)fonts[sizes[i]]->atlasHeight, 1,          //width, height, depth
                GL_RED,                 //format
                GL_UNSIGNED_BYTE,       //type
                &atlasData[0]); //pointer to data
    }
}

GlyphInfo FontManager::getGlyphInfo(int size, uint32_t character, float offsetX, float offsetY)
{
    stbtt_aligned_quad quad;

    stbtt_GetPackedQuad(fonts[size]->charInfo.get(), (int)fonts[size]->atlasWidth, (int)fonts[size]->atlasHeight, (int)(character - fonts[size]->firstChar), &offsetX, &offsetY, &quad, 1);
    auto xmin = quad.x0;
    auto xmax = quad.x1;
    auto ymin = quad.y0;
    auto ymax = quad.y1;

    GlyphInfo info = GlyphInfo();
    info.offsetX = offsetX;
    info.offsetY = offsetY;
    info.positions[0] = { xmin, ymin };
    info.positions[1] = { xmax, ymin };
    info.positions[2] = { xmax, ymax };
    info.positions[3] = { xmin, ymin };
    info.positions[4] = { xmax, ymax };
    info.positions[5] = { xmin, ymax };

    info.uvs[0] = { quad.s0, quad.t0 };
    info.uvs[1] = { quad.s1, quad.t0 };
    info.uvs[2] = { quad.s1, quad.t1 };
    info.uvs[3] = { quad.s0, quad.t0 };
    info.uvs[4] = { quad.s1, quad.t1 };
    info.uvs[5] = { quad.s0, quad.t1 };

    return info;
}

int FontManager::genGlyphs(const std::string& text, int x, int y, int size, GLuint& vao, GLuint& vbo, GLuint& uv) {
    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> uvs;

    int count = 0;
    float offsetX = (float)x, offsetY = (float)y;
    for (auto c : text)
    {
        auto glyphInfo = getGlyphInfo(size, c, offsetX, offsetY);
        offsetX = glyphInfo.offsetX;
        offsetY = glyphInfo.offsetY;

        vertices.emplace_back(glyphInfo.positions[0]);
        vertices.emplace_back(glyphInfo.positions[1]);
        vertices.emplace_back(glyphInfo.positions[2]);
        vertices.emplace_back(glyphInfo.positions[3]);
        vertices.emplace_back(glyphInfo.positions[4]);
        vertices.emplace_back(glyphInfo.positions[5]);
        uvs.emplace_back(glyphInfo.uvs[0]);
        uvs.emplace_back(glyphInfo.uvs[1]);
        uvs.emplace_back(glyphInfo.uvs[2]);
        uvs.emplace_back(glyphInfo.uvs[3]);
        uvs.emplace_back(glyphInfo.uvs[4]);
        uvs.emplace_back(glyphInfo.uvs[5]);

        count += 6;
    }

    if (!vao)
        glGenVertexArrays(1, &vao);

    glDeleteBuffers(1, &vbo);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDeleteBuffers(1, &uv);
    glGenBuffers(1, &uv);
    glBindBuffer(GL_ARRAY_BUFFER, uv);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * uvs.size(), uvs.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    return count;
}

void FontManager::render(GLuint& vao, GLuint& vbo, GLuint& uv, int count) {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D_ARRAY, fontArray);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, uv);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, count);
}

void FontManager::genBackground(int x, int y, int w, int h, GLuint& vao, GLuint& vbo, GLuint& uv) {
    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> uvs = {glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f), glm::vec2(0.0f, 0.0f)};

    vertices.emplace_back((float)x, (float)y);
    vertices.emplace_back((float)(x + w), (float)(y));
    vertices.emplace_back((float)(x + w), (float)(y + h));
    vertices.emplace_back((float)x, (float)y);
    vertices.emplace_back((float)(x + w), (float)(y + h));
    vertices.emplace_back((float)(x), (float)(y + h));

    if (!vao)
        glGenVertexArrays(1, &vao);

    glDeleteBuffers(1, &vbo);
    glGenBuffers(1, &vbo);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * vertices.size(), vertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDeleteBuffers(1, &uv);
    glGenBuffers(1, &uv);
    glBindBuffer(GL_ARRAY_BUFFER, uv);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 2 * uvs.size(), uvs.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void FontManager::renderBackground(GLuint& vao, GLuint& vbo, GLuint& uv) {
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, uv);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}