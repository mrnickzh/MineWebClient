#pragma once

#include "../Event.hpp"

class TickEvent : public Event {
public:
    TickEvent() : Event() {}
};