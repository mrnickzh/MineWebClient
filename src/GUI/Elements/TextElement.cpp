#include "TextElement.hpp"

TextElement::TextElement(std::string id, std::function<void(int)> callback, int x, int y, FontManager* fm) : Element(id, callback, x, y) {
    fontManager = fm;
}

void TextElement::render() {
    if (!active) { return; }
    glUniform3f(Main::fontShader->uniforms["color"], color.r, color.g, color.b);
    fontManager->render(vao, vbo, uv, count);
}

void TextElement::setText(const std::string &t, glm::vec3 c) {
    text = t;
    color = c;
    count = fontManager->genGlyphs(t, x, y, vao, vbo, uv);
}

void TextElement::setPosition(int posx, int posy) {
    x = posx;
    y = posy;
    count = fontManager->genGlyphs(text, x, y, vao, vbo, uv);
}
