#pragma once
#include "../Objects/Object.hpp"

class PhysicsObject {
public:
    std::shared_ptr<Object> object;
    glm::vec3 velocity;
    float mass;

    PhysicsObject(std::shared_ptr<Object>& obj, float m);
    glm::vec3 getPosition();
    void setPosition(glm::vec3 pos);
    glm::vec3 getCollider();
};

