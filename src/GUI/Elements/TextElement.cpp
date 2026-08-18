#include "TextElement.hpp"

#include "../../main.hpp"

TextElement::TextElement(std::string id, std::function<bool(int, int, int, bool)> callback, float wPer, float hPer, int fontsize, FontManager* fm, bool bg) : Element(id, callback, wPer, hPer, fontsize) {
    fontManager = fm;
    background = bg;

    x = (wPer / 100.0f) * Main::windowWidth;
    y = (hPer / 100.0f) * Main::windowHeight;
}

void TextElement::render() {
    if (!active) { return; }

    glUniform1f(Main::fontShader->uniforms["texindex"], (float)fontManager->offsetFromSize(fontsize));

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
    count = fontManager->genGlyphs(text, x, y, fontsize, vao, vbo, uv);
    if (background) {
        bounds = fontManager->genBackground(text, fontsize, x, y, 10, 10, bvao, bvbo, buv);
    }
}

void TextElement::setPosition(int posx, int posy) {
    x = posx;
    y = posy;
    count = fontManager->genGlyphs(text, x, y, fontsize, vao, vbo, uv);
    if (background) {
        bounds = fontManager->genBackground(text, fontsize, x, y, 10, 10, bvao, bvbo, buv);
    }
}

bool TextElement::checkBounds(int x, int y) {
    if ((float)x > bounds.minX && (float)x < bounds.maxX && (float)y > bounds.minY && (float)y < bounds.maxY) {
        return true;
    }
    return false;
}

void TextElement::reposition() {
    x = (wPer / 100.0f) * Main::windowWidth;
    y = (hPer / 100.0f) * Main::windowHeight;
    setPosition(x, y);
}
