#pragma once

#include <memory>
#include <string>
#include "../Objects/EntityObject.hpp"

class Entity {
public:
    std::string uuid;
    std::shared_ptr<EntityObject> object;
    Entity() {}

    bool operator==(const Entity& other) const {
        return this->uuid == other.uuid;
    }
};
