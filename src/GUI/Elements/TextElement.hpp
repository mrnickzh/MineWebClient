#pragma once

#include <GLES3/gl3.h>

#include "Element.hpp"
#include "../../Utils/FontManager.hpp"
#include "../../main.hpp"

class TextElement : public Element {
public:
    std::string text;
    GLuint vao, vbo, uv = 0;
    GLuint bvao, bvbo, buv = 0;
    FontManager* fontManager;
    int count = 0;
    glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);
    glm::vec3 bcolor = glm::vec3(0.0f, 0.0f, 0.0f);
    bool background = false;

    TextElement(std::string id, std::function<void(int, int, int)> callback, int x, int y, int fontsize, FontManager* fm, bool bg);

    void render() override;
    void setText(const std::string& t);
    void setPosition(int x, int y) override;
};
