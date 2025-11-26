#include "PhysicsObject.hpp"

PhysicsObject::PhysicsObject(std::shared_ptr<Object>& obj, float m) {
    object = obj;
    mass = m;
    velocity = glm::vec3(0.0f, 0.0f, 0.0f);
}

glm::vec3 PhysicsObject::getPosition() { return object->position; }
void PhysicsObject::setPosition(glm::vec3 pos) { object->position = pos; }
glm::vec3 PhysicsObject::getCollider() { return object->collider; }
