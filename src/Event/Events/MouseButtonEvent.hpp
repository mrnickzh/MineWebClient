#pragma once

#include "../Event.hpp"

class MouseButtonEvent : public Event {
public:
    int button;
    bool down;

    MouseButtonEvent(int button, bool down) : Event(), button(button), down(down) {}
};