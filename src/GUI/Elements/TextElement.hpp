#pragma once

#include <GLES3/gl3.h>

#include "Element.hpp"
#include "../../Utils/FontManager.hpp"
#include "../../main.hpp"

class TextElement : public Element {
public:
    std::string text;
    GLuint vao, vbo, uv = 0;
    FontManager* fontManager;
    int count = 0;
    glm::vec3 color = glm::vec3(0.0f, 0.0f, 0.0f);

    TextElement(std::string id, std::function<void(int)> callback, int x, int y, FontManager* fm);

    void render() override;
    void setText(const std::string& t, glm::vec3 c);
    void setPosition(int x, int y) override;
};
