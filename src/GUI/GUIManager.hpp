#pragma once

#include <vector>

#include "Elements/Element.hpp"

#define LEFT_CLICK 0x0001
#define RIGHT_CLICK 0x0002
#define MIDDLE_CLICK 0x0004
#define CTRL_FLAG 0x0010
#define SHIFT_FLAG 0x0020

class GUIManager {
public:
    std::vector<std::shared_ptr<Element>> registeredElements;
    bool active = true;

    void addElement(std::shared_ptr<Element> element);
    void removeElement(std::shared_ptr<Element> element);
    std::shared_ptr<Element> getElement(std::string elementid);
    void render();
    void poll(int x, int y, int stateMask);
};
