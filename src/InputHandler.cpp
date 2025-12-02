#include "InputHandler.hpp"

#include "main.hpp"
#include "Event/EventBus.hpp"
#include "Event/Events/KeyEvent.hpp"
#include "Event/Events/MouseButtonEvent.hpp"
#include "Event/Events/MouseMoveEvent.hpp"

std::set<std::string> chars;

EM_BOOL InputHandler::mouseMoved(int, const EmscriptenMouseEvent* e, void*) {
    MouseMoveEvent event((float)e->clientX, (float)e->clientY, (float)e->movementX, (float)e->movementY);
    EventBus::getInstance().publish(&event);
    if (event.canceled) return EM_TRUE;
    return EM_FALSE;
}

EM_BOOL InputHandler::mouseButton(int eventType, const EmscriptenMouseEvent* e, void*) {
    MouseButtonEvent event(e->button, (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN));
    EventBus::getInstance().publish(&event);
    if (event.canceled) return EM_TRUE;
    return EM_FALSE;
}

EM_BOOL InputHandler::keyPressed(int, const EmscriptenKeyboardEvent* e, void*) {
    chars.insert(std::string(e->code));
    KeyEvent event(std::string(e->code), true);
    EventBus::getInstance().publish(&event);
    if (event.canceled) return true;
    return EM_FALSE;
}

EM_BOOL InputHandler::keyReleased(int, const EmscriptenKeyboardEvent* e, void*) {
    chars.erase(std::string(e->code));
    KeyEvent event(std::string(e->code), false);
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