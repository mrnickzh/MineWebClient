#pragma once
#include <map>
#include <vector>

#include "../Utils/ChunkMap.hpp"

#include "PhysicsObject.hpp"

struct RaycastResult {
    bool hit;
    float distance;
    glm::vec3 blockpos;
    glm::vec3 chunkpos;
    std::shared_ptr<Object> object;
    glm::vec3 prevblockpos;
    glm::vec3 prevchunkpos;
    std::shared_ptr<Object> prevobject;
};

class PhysicsEngine {
public:
    std::vector<std::shared_ptr<PhysicsObject>> registeredObjects;
    std::map<glm::vec3, std::shared_ptr<ChunkMap>, vec3Comparator>* chunkmap;

    PhysicsEngine(std::map<glm::vec3, std::shared_ptr<ChunkMap>, vec3Comparator>* worldmap);
    void registerObject(std::shared_ptr<Object> object, float mass);
    void unregisterObject(std::shared_ptr<Object> object);
    void addVelocityRotation(std::shared_ptr<Object> object, glm::vec3 velocity);
    void addVelocityClampedRotation(std::shared_ptr<Object> object, glm::vec3 velocity, glm::vec3 limit);
    void addVelocity(std::shared_ptr<Object> object, glm::vec3 velocity);
    void addVelocityClamped(std::shared_ptr<Object> object, glm::vec3 velocity, glm::vec3 limit);
    void setVelocity(std::shared_ptr<Object> object, glm::vec3 velocity);
    bool isOnFoot(std::shared_ptr<Object> object);
    RaycastResult raycast(float length, glm::vec3 startpos, glm::vec3 rotation);
    glm::vec3 getVelocity(std::shared_ptr<Object> object);
    void step();

    bool isColliding(glm::vec3 object1, glm::vec3 object2, glm::vec3 collider1, glm::vec3 collider2);
    bool possibleCollision(glm::vec3 position, glm::vec3 collider, const std::shared_ptr<Object>& object2);
    std::vector<std::shared_ptr<Object>> possibleObstacles(glm::vec3 position);
    void calculateVelocity(std::shared_ptr<PhysicsObject>& obj);
};
