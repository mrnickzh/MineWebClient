#pragma once

#define GLM_FORCE_PURE
#include <GLES3/gl3.h>

#include "../../lib/glm/glm.hpp"
#include "../../lib/glm/gtc/matrix_transform.hpp"
#include "../../lib/glm/gtc/type_ptr.hpp"

struct GlyphInfo
{
    glm::vec2 positions[6];
    glm::vec2 uvs[6];
    float offsetX, offsetY;
};

class FontManager {
public:
    GLuint fontArray = 0;

    int offsetFromSize(int size);
    void init(const std::string& path, std::vector<int>& sizes);
    void render(GLuint& vao, GLuint& vbo, GLuint& uv, int count);
    GlyphInfo getGlyphInfo(int size, uint32_t character, float offsetX, float offsetY);
    int genGlyphs(const std::string& text, int x, int y, int size, GLuint& vao, GLuint& vbo, GLuint& uv);
    void genBackground(int x, int y, int w, int h, GLuint& vao, GLuint& vbo, GLuint& uv);
    void renderBackground(GLuint& vao, GLuint& vbo, GLuint& uv);
};
