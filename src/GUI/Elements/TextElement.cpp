#include "TextElement.hpp"

TextElement::TextElement(std::string id, std::function<void(int, int, int)> callback, int x, int y, FontManager* fm, bool bg) : Element(id, callback, x, y) {
    fontManager = fm;
    background = bg;
}

void TextElement::render() {
    if (!active) { return; }

    if (background) {
        glUniform1i(Main::fontShader->uniforms["background"], 1);
        glUniform3f(Main::fontShader->uniforms["color"], bcolor.r, bcolor.g, bcolor.b);
        fontManager->renderBackground(bvao, bvbo, buv);
        glUniform1i(Main::fontShader->uniforms["background"], 0);
    }

    glUniform3f(Main::fontShader->uniforms["color"], color.r, color.g, color.b);
    fontManager->render(vao, vbo, uv, count);
}

void TextElement::setText(const std::string &t) {
    text = t;
    count = fontManager->genGlyphs(text, x, y, vao, vbo, uv);
    if (background) {
        fontManager->genBackground(x - 5, y - 25, (10 * (int)text.length()), 30, bvao, bvbo, buv);
    }
}

void TextElement::setPosition(int posx, int posy) {
    x = posx;
    y = posy;
    count = fontManager->genGlyphs(text, x, y, vao, vbo, uv);
    if (background) {
        fontManager->genBackground(x - 5, y - 25, (10 * (int)text.length()), 30, bvao, bvbo, buv);
    }
}
