#pragma once

#include "../Event.hpp"

class MouseEvent : public Event {
public:
    float posx;
    float posy;
    float deltax;
    float deltay;
    int button;
    bool down;

    MouseEvent(float posx, float posy, float deltax, float deltay, int button, bool down) : Event(), posx(posx), posy(posy), deltax(deltax), deltay(deltay), button(button), down(down) {}
};