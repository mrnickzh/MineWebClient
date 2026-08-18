#pragma once
#include <functional>

class KeyBind {
public:
    KeyBind(std::function<void(bool)> cb) {callback = cb;}

    std::function<void(bool)> callback;
    bool pressed = false;
};
