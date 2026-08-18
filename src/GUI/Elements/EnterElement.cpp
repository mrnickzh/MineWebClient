#include "EnterElement.hpp"

#include "../../main.hpp"

EnterElement::EnterElement(std::string id, std::function<bool(int, int, int, bool)> callback, float wPer, float hPer, int fontsize, FontManager* fm, int ml, std::string deftext, bool bg) : Element(id, callback, wPer, hPer, fontsize) {
    x = (wPer / 100.0f) * Main::windowWidth;
    y = (hPer / 100.0f) * Main::windowHeight;

    fontManager = fm;
    maxlen = ml;
    text = deftext;
    count = fontManager->genGlyphs(text, x, y, fontsize, vao, vbo, uv);
    background = bg;

    dummybgtext.clear();
    for (int i = 0; i < maxlen; i++) {
        dummybgtext += "A";
    }

    if (background) {
        bounds = fontManager->genBackground(dummybgtext, fontsize, x, y, 10, 10, bvao, bvbo, buv);
    }
}

void EnterElement::render() {
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

void EnterElement::addChar(const char chr) {
    if (chr > (' ' - 1) && chr < '~' && enteractive) {
        if ((text + chr).length() > maxlen) { return; }
        text += chr;
        count = fontManager->genGlyphs(text, x, y, fontsize, vao, vbo, uv);
    }
}

void EnterElement::removeChar() {
    if (!text.empty() && enteractive) {
        text.pop_back();
        count = fontManager->genGlyphs(text, x, y, fontsize, vao, vbo, uv);
    }
}

void EnterElement::setPosition(int posx, int posy) {
    x = posx;
    y = posy;
    count = fontManager->genGlyphs(text, x, y, fontsize, vao, vbo, uv);
    if (background) {
        dummybgtext.clear();
        for (int i = 0; i < maxlen; i++) {
            dummybgtext += "A";
        }

        bounds = fontManager->genBackground(dummybgtext, fontsize, x, y, 10, 10, bvao, bvbo, buv);
    }
}

bool EnterElement::checkBounds(int x, int y) {
    if ((float)x > bounds.minX && (float)x < bounds.maxX && (float)y > bounds.minY && (float)y < bounds.maxY) {
        return true;
    }
    return false;
}

void EnterElement::reposition() {
    x = (wPer / 100.0f) * Main::windowWidth;
    y = (hPer / 100.0f) * Main::windowHeight;
    setPosition(x, y);
}


