#pragma once

#include "../Event.hpp"

class MouseMoveEvent : public Event {
public:
    float posx;
    float posy;
    float deltax;
    float deltay;

    MouseMoveEvent(float posx, float posy, float deltax, float deltay) : Event(), posx(posx), posy(posy), deltax(deltax), deltay(deltay) {}
};