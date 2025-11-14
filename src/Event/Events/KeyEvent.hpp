#pragma once

#include "../Event.hpp"

class KeyEvent : public Event {
public:
    int key;
    bool state;

    KeyEvent(int key, bool state) : Event(), key(key), state(state) {}
};