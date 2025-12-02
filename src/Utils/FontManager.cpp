#include "FontManager.hpp"

#include <fstream>
#include <iostream>

#define STB_TRUETYPE_IMPLEMENTATION
#include <algorithm>

#include "../../lib/stb/stb_truetype.h"

struct
{
    const uint32_t size = 20;
    const uint32_t atlasWidth = 1024;
    const uint32_t atlasHeight = 1024;
    const uint32_t oversampleX = 1;
    const uint32_t oversampleY = 1;
    const uint32_t firstChar = ' ';
    const uint32_t charCount = '~' - ' ';
    std::unique_ptr<stbtt_packedchar[]> charInfo;
    GLuint texture = 0;
} font;

void FontManager::init(const std::string& path) {
    std::ifstream file(path, std::ios::binary);
    std::vector<uint8_t> fontData;

    if (file) {
        fontData = std::vector<uint8_t>((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
    }

    uint8_t atlasData[font.atlasWidth * font.atlasHeight];

    font.charInfo = std::make_unique<stbtt_packedchar[]>(font.charCount);

    stbtt_pack_context context;
    if (!stbtt_PackBegin(&context, &atlasData[0], (int)font.atlasWidth, (int)font.atlasHeight, 0, 1, nullptr))
        std::cout << "Failed to initialize font" << std::endl;

    stbtt_PackSetOversampling(&context, font.oversampleX, font.oversampleY);
    if (!stbtt_PackFontRange(&context, fontData.data(), 0, (float)font.size, (int)font.firstChar, (int)font.charCount, font.charInfo.get()))
        std::cout << "Failed to pack font" << std::endl;

    stbtt_PackEnd(&context);

    glGenTextures(1, &font.texture);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, font.texture);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_R8, (int)font.atlasWidth, (int)font.atlasHeight, 0, GL_RED, GL_UNSIGNED_BYTE, &atlasData[0]);
    glHint(GL_GENERATE_MIPMAP_HINT, GL_NICEST);
    glGenerateMipmap(GL_TEXTURE_2D);
}

GlyphInfo FontManager::getGlyphInfo(uint32_t character, float offsetX, float offsetY)
{
    stbtt_aligned_quad quad;

    stbtt_GetPackedQuad(font.charInfo.get(), (int)font.atlasWidth, (int)font.atlasHeight, (int)(character - font.firstChar), &offsetX, &offsetY, &quad, 1);
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

int FontManager::genGlyphs(const std::string& text, int x, int y, GLuint& vao, GLuint& vbo, GLuint& uv) {
    std::vector<glm::vec2> vertices;
    std::vector<glm::vec2> uvs;

    int count = 0;
    float offsetX = x, offsetY = y;
    for (auto c : text)
    {
        auto glyphInfo = getGlyphInfo(c, offsetX, offsetY);
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
    glBindTexture(GL_TEXTURE_2D, font.texture);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ARRAY_BUFFER, uv);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(1);

    glDrawArrays(GL_TRIANGLES, 0, count);
}