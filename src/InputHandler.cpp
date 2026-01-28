#include "InputHandler.hpp"

#include "main.hpp"
#include "Event/EventBus.hpp"
#include "Event/Events/KeyEvent.hpp"
#include "Event/Events/MouseButtonEvent.hpp"
#include "Event/Events/MouseMoveEvent.hpp"

std::set<std::string> chars;
bool buttons[3] = {false, false, false};

EmscriptenTouchPoint touch0;

EM_BOOL InputHandler::touchMoved(int eventType, const EmscriptenTouchEvent* e, void*) {
    float cx = (float)e->touches[0].clientX;
    float cy = (float)e->touches[0].clientY;
    MouseMoveEvent event(cx, cy, cx - (float)touch0.clientX, cy - (float)touch0.clientY);
    EventBus::getInstance().publish(&event);
    if (event.canceled) return EM_TRUE;
    return EM_FALSE;
}

EM_BOOL InputHandler::touchStart(int eventType, const EmscriptenTouchEvent* e, void*) {
    if (!Main::isMobile) { Main::isMobile = true; }
    emscripten_request_fullscreen("canvas", EM_TRUE);
    touch0 = e->touches[0];
    float cx = (float)e->touches[0].clientX;
    float cy = (float)e->touches[0].clientY;
    MouseMoveEvent moveevent(cx, cy, 0.0f, 0.0f);
    MouseButtonEvent buttonevent(MOUSE_LEFT, true);
    buttons[MOUSE_LEFT] = true;
    EventBus::getInstance().publish(&moveevent);
    EventBus::getInstance().publish(&buttonevent);
    if (moveevent.canceled || buttonevent.canceled)  return EM_TRUE;
    return EM_FALSE;
}

EM_BOOL InputHandler::touchEnd(int eventType, const EmscriptenTouchEvent* e, void*) {
    MouseButtonEvent buttonevent(MOUSE_LEFT, false);
    buttons[MOUSE_LEFT] = false;
    EventBus::getInstance().publish(&buttonevent);
    if (buttonevent.canceled)  return EM_TRUE;
    return EM_FALSE;
}

EM_BOOL InputHandler::mouseMoved(int eventType, const EmscriptenMouseEvent* e, void*) {
    MouseMoveEvent event((float)e->clientX, (float)e->clientY, (float)e->movementX, (float)e->movementY);
    EventBus::getInstance().publish(&event);
    if (event.canceled) return EM_TRUE;
    return EM_FALSE;
}

EM_BOOL InputHandler::mouseButton(int eventType, const EmscriptenMouseEvent* e, void*) {
    MouseButtonEvent event(e->button, (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN));
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