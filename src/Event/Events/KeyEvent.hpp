#pragma once

#include <string>

#include "../Event.hpp"

class KeyEvent : public Event {
public:
    std::string key;
    EM_UTF8 code[32];
    bool state;

    KeyEvent(std::string key, bool state) : Event(), key(key),  state(state) {}
};