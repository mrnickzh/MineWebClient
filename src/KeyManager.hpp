#pragma once
#include <unordered_map>

#include "KeyBind.hpp"

class KeyManager {
public:
    std::unordered_map<std::string, std::shared_ptr<KeyBind>> keys;

    KeyManager() {};
    void setKeyBind(std::string keyName, std::shared_ptr<KeyBind> keyBind);
    void processKeyBinds(std::string keyName, bool down);
};

