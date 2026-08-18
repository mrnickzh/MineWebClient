#include "GUIManager.hpp"

#include <iostream>
#include <__ostream/basic_ostream.h>

void GUIManager::addElement(std::shared_ptr<Element> element) {
    registeredElements.push_back(element);
}

void GUIManager::removeElement(std::shared_ptr<Element> element) {
    registeredElements.erase(std::remove(registeredElements.begin(), registeredElements.end(), element), registeredElements.end());
}

std::shared_ptr<Element> GUIManager::getElement(std::string elementid) {
    auto it = std::find_if(registeredElements.begin(), registeredElements.end(), [&elementid](const std::shared_ptr<Element>& elem) {return elem->id == elementid; });
    if (it != registeredElements.end()) { return *it; }
    else { return nullptr; }
}

bool GUIManager::poll(int x, int y, int stateMask, bool isProcessed) {
    if (!active) { return false; }
    bool processed = isProcessed;
    for (auto& element : registeredElements) {
        if (element->callback(x, y, stateMask, processed)) { processed = true; }
    }
    return processed;
}

void GUIManager::reposition() {
    if (!active) { return; }
    for (auto& element : registeredElements) {
        element->reposition();
    }
}

void GUIManager::render() {
    if (!active) { return; }
    for (auto& element : registeredElements) {
        element->render();
    }
}
