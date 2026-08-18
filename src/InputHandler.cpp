#include "InputHandler.hpp"

#include "main.hpp"
#include "Event/EventBus.hpp"
#include "Event/Events/KeyEvent.hpp"
#include "Event/Events/MouseEvent.hpp"

std::set<std::string> chars;
bool buttons[3] = {false, false, false};

EmscriptenTouchPoint touches[32];

EM_BOOL InputHandler::touchMoved(int eventType, const EmscriptenTouchEvent* e, void*) {
    for (int i = 0; i < e->numTouches; i++) {
        float cx = (float)e->touches[i].clientX;
        float cy = (float)e->touches[i].clientY;
        float px = (float)touches[e->touches[i].identifier].clientX;
        float py = (float)touches[e->touches[i].identifier].clientY;
        MouseEvent event(cx * Main::DPR, cy * Main::DPR, (cx - px)  * Main::DPR, (cy - py)  * Main::DPR, MOUSE_LEFT, true);
        EventBus::getInstance().publish(&event);
    }
    return EM_FALSE;
}

EM_BOOL InputHandler::touchStart(int eventType, const EmscriptenTouchEvent* e, void*) {
    if (!Main::isMobile) { Main::isMobile = true; }
    for (int i = 0; i < e->numTouches; i++) {
        //printf("%d x, %d y, %d ischanged, %d ontarget, %d id, %d i\n", e->touches[i].clientX, e->touches[i].clientY, e->touches[i].isChanged,  e->touches[i].onTarget, e->touches[i].identifier, i);
        touches[e->touches[i].identifier] = e->touches[i];
    }
    float cx = (float)e->touches[e->numTouches - 1].clientX;
    float cy = (float)e->touches[e->numTouches - 1].clientY;
    MouseEvent event(cx * Main::DPR, cy * Main::DPR, 0.0f, 0.0f, MOUSE_LEFT, true);
    buttons[MOUSE_LEFT] = true;
    EventBus::getInstance().publish(&event);
    if (event.canceled)  return EM_TRUE;
    return EM_FALSE;
}

EM_BOOL InputHandler::touchEnd(int eventType, const EmscriptenTouchEvent* e, void*) {
    for (int i = 0; i < e->numTouches; i++) {
        //printf("%d x, %d y, %d ischanged, %d ontarget, %d id, %d i\n", e->touches[i].clientX, e->touches[i].clientY, e->touches[i].isChanged,  e->touches[i].onTarget, e->touches[i].identifier, i);

        if (e->touches[i].isChanged) {
            float cx = (float)e->touches[i].clientX;
            float cy = (float)e->touches[i].clientY;
            MouseEvent event(cx * Main::DPR, cy * Main::DPR, 0.0f, 0.0f, MOUSE_LEFT, false);
            buttons[MOUSE_LEFT] = false;
            EventBus::getInstance().publish(&event);
            if (event.canceled)  return EM_TRUE;
            return EM_FALSE;
        }
    }
    return EM_FALSE;
}

EM_BOOL InputHandler::mouseMoved(int eventType, const EmscriptenMouseEvent* e, void*) {
    MouseEvent event((float)e->clientX * Main::DPR, (float)e->clientY * Main::DPR, (float)e->movementX * Main::DPR, (float)e->movementY * Main::DPR, e->button, (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN));
    EventBus::getInstance().publish(&event);
    if (event.canceled) return EM_TRUE;
    return EM_FALSE;
}

EM_BOOL InputHandler::mouseButton(int eventType, const EmscriptenMouseEvent* e, void*) {
    MouseEvent event((float)e->clientX * Main::DPR, (float)e->clientY * Main::DPR, (float)e->movementX * Main::DPR, (float)e->movementY * Main::DPR, e->button, (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN));
    buttons[e->button] = (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN);
    EventBus::getInstance().publish(&event);
    if (event.canceled) return EM_TRUE;
    return EM_FALSE;
}

EM_BOOL InputHandler::keyPressed(int, const EmscriptenKeyboardEvent* e, void*) {
    chars.insert(std::string(e->code));
    KeyEvent event(std::string(e->code), true);
    strcpy(event.code, e->key);
    std::cout << e->code << std::endl;
    EventBus::getInstance().publish(&event);
    if (event.canceled) return true;
    return EM_FALSE;
}

EM_BOOL InputHandler::keyReleased(int, const EmscriptenKeyboardEvent* e, void*) {
    chars.erase(std::string(e->code));
    KeyEvent event(std::string(e->code), false);
    strcpy(event.code, e->key);
    EventBus::getInstance().publish(&event);
    if (event.canceled) return true;
    return EM_FALSE;
}

bool InputHandler::isKeyPressed(const std::string& key) {
    return chars.find(key) != chars.end();
}

bool InputHandler::isKeyReleased(const std::string& key) {
    return chars.find(key) == chars.end();
}

bool InputHandler::isMousePressed(int key) {
    return buttons[key];
}

bool InputHandler::isMouseReleased(int key) {
    return !buttons[key];
}

void InputHandler::addKey(std::string key) {
    chars.insert(key);
}

void InputHandler::removeKey(std::string key) {
    chars.erase(key);
}