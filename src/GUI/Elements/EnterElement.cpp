#include "EnterElement.hpp"

EnterElement::EnterElement(std::string id, std::function<void(int)> callback, int x, int y, FontManager* fm, int ml, std::string deftext) : Element(id, callback, x, y) {
    fontManager = fm;
    maxlen = ml;
    text = deftext;
    count = fontManager->genGlyphs(text, x, y, vao, vbo, uv);
}

void EnterElement::render() {
    if (!active) { return; }
    if (text.empty()) { return; }
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
