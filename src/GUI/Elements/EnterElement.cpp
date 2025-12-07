#include "EnterElement.hpp"

EnterElement::EnterElement(std::string id, std::function<void(int)> callback, int x, int y, FontManager* fm, int ml, std::string deftext) : Element(id, callback, x, y) {
    fontManager = fm;
    maxlen = ml;
    text = deftext;
    count = fontManager->genGlyphs(text, x, y, vao, vbo, uv);
    fontManager->genBackground(x - 5, y - 25, (10 * maxlen), 30, bvao, bvbo, buv);
}

void EnterElement::render() {
    if (!active) { return; }

    glUniform1i(Main::fontShader->uniforms["background"], 1);
    glUniform3f(Main::fontShader->uniforms["color"], 0.5f, 0.5f, 0.5f);
    fontManager->renderBackground(bvao, bvbo, buv);
    glUniform1i(Main::fontShader->uniforms["background"], 0);

    glUniform3f(Main::fontShader->uniforms["color"], color.r, color.g, color.b);
    fontManager->render(vao, vbo, uv, count);
}

void EnterElement::addChar(const char chr) {
    if (chr > ' ' && chr < '~' && enteractive) {
        if ((text + chr).length() > maxlen) { return; }
        text += chr;
        count = fontManager->genGlyphs(text, x, y, vao, vbo, uv);
    }
}

void EnterElement::removeChar() {
    if (!text.empty()) {
        text.pop_back();
        count = fontManager->genGlyphs(text, x, y, vao, vbo, uv);
    }
}

void EnterElement::setPosition(int posx, int posy) {
    x = posx;
    y = posy;
    count = fontManager->genGlyphs(text, x, y, vao, vbo, uv);
}
