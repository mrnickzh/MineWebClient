#include "GUIManager.hpp"

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

void GUIManager::poll(int x, int y, int stateMask) {
    if (!active) { return; }
    for (auto& element : registeredElements) {
        element->callback(x, y, stateMask);
    }
}

void GUIManager::render() {
    if (!active) { return; }
    for (auto& element : registeredElements) {
        element->render();
    }
}
