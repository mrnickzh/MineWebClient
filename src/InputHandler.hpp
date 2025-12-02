#pragma once

#include <set>
#include <string>
#include <emscripten/em_types.h>
#include <emscripten/html5.h>

class InputHandler {
public:
    static EM_BOOL mouseMoved(int, const EmscriptenMouseEvent* e, void*);
    static EM_BOOL mouseButton(int eventType, const EmscriptenMouseEvent* e, void*);
    static EM_BOOL keyPressed(int, const EmscriptenKeyboardEvent* e, void*);
    static EM_BOOL keyReleased(int, const EmscriptenKeyboardEvent* e, void*);
    static bool isKeyPressed(const std::string& key);
    static bool isKeyReleased(const std::string& key);
};