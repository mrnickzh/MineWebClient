#include "KeyManager.hpp"

#include <iostream>
#include <__ostream/basic_ostream.h>

void KeyManager::setKeyBind(std::string keyName, std::shared_ptr<KeyBind> keyBind) {
    keys[keyName] = keyBind;
}

void KeyManager::processKeyBinds(std::string keyName, bool down) {
    std::shared_ptr<KeyBind> key = keys[keyName];
    if (key == nullptr) { return; }
    if (down && !key->pressed) {
        key->pressed = true;
        key->callback(true);
    }
    else if (!down && key->pressed) {
        key->pressed = false;
        key->callback(false);
    }
}


