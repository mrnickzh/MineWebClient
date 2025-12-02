#pragma once

#include <string>

#include "../Event.hpp"

class KeyEvent : public Event {
public:
    std::string key;
    bool state;

    KeyEvent(std::string key, bool state) : Event(), key(key), state(state) {}
};