#include "PhysicsEngine.hpp"

#include "../main.hpp"
#include "../Objects/AirObject.hpp"
#include "../Utils/GetAABB.hpp"

PhysicsEngine::PhysicsEngine(std::map<glm::vec3, std::shared_ptr<ChunkMap>, vec3Comparator>* worldmap) {
    chunkmap = worldmap;
}

bool PhysicsEngine::isColliding(glm::vec3 object1, glm::vec3 object2, glm::vec3 collider1, glm::vec3 collider2) {
    AABB obj1 = GetAABB::CP2AABB(collider1, object1);
    AABB obj2 = GetAABB::CP2AABB(collider2, object2);

    bool xcheck1 = obj1.AA.x < obj2.BB.x;
    bool x1check1 = obj1.BB.x > obj2.AA.x;
    bool ycheck1 = obj1.AA.y < obj2.BB.y;
    bool y1check1 = obj1.BB.y > obj2.AA.y;
    bool zcheck1 = obj1.AA.z < obj2.BB.z;
    bool z1check1 = obj1.BB.z > obj2.AA.z;
    if (xcheck1 && ycheck1 && x1check1 && y1check1 && zcheck1 && z1check1) {
        // std::cout << "====================" << std::endl;
        // std::cout << object1.x << " " << object1.y << std::endl;
        // std::cout << collider1.x << " " << collider1.y << std::endl;
        // std::cout << object2.x << " " << object2.y << std::endl;
        // std::cout << collider2.x << " " << collider2.y << std::endl;
        return true;
    }
    return false;
}

bool PhysicsEngine::possibleCollision(glm::vec3 position, glm::vec3 collider, std::shared_ptr<Object>& object2) {
    if (!object2->cancollide) { return false; }
    return isColliding(position, object2->position, collider, object2->collider);
}

std::vector<std::shared_ptr<Object>> PhysicsEngine::possibleObstacles(glm::vec3 position) {
    std::vector<std::shared_ptr<Object>> obstacles;
    glm::vec3 currentChunk = glm::vec3(floor(position.x / 8.0f), floor(position.y / 8.0f), floor(position.z / 8.0f));
    glm::vec3 currentChunkBlock = glm::vec3(floor(std::fmod(position.x, 8.0f)), floor(std::fmod(position.y, 8.0f)), floor(std::fmod(position.z, 8.0f)));
    currentChunkBlock.x += (currentChunk.x < 0.0f ? (currentChunkBlock.x == 0.0f ? 0.0f : 8.0f) : 0.0f);
    currentChunkBlock.y += (currentChunk.y < 0.0f ? (currentChunkBlock.y == 0.0f ? 0.0f : 8.0f) : 0.0f);
    currentChunkBlock.z += (currentChunk.z < 0.0f ? (currentChunkBlock.z == 0.0f ? 0.0f : 8.0f) : 0.0f);
    // std::cout << "==========" << std::endl;
    for (int i = (int)currentChunkBlock.x - 1; i <= (int)currentChunkBlock.x + 1; i++) {
        glm::vec3 collisionChunkX = currentChunk;
        glm::vec3 collisionChunkBlockX = currentChunkBlock;
        collisionChunkBlockX.x = (float)i;
        if (i < 0) { collisionChunkX.x -= 1.0f; collisionChunkBlockX.x = 7.0f; }
        if (i > 7) { collisionChunkX.x += 1.0f; collisionChunkBlockX.x = 0.0f; }
        for (int j = (int)currentChunkBlock.y - 1; j <= (int)currentChunkBlock.y + 1; j++) {
            glm::vec3 collisionChunkY = collisionChunkX;
            glm::vec3 collisionChunkBlockY = collisionChunkBlockX;
            collisionChunkBlockY.y = (float)j;
            if (j < 0) { collisionChunkY.y -= 1.0f; collisionChunkBlockY.y = 7.0f; }
            if (j > 7) { collisionChunkY.y += 1.0f; collisionChunkBlockY.y = 0.0f; }
            for (int k = (int)currentChunkBlock.z - 1; k <= (int)currentChunkBlock.z + 1; k++) {
                glm::vec3 collisionChunkZ = collisionChunkY;
                glm::vec3 collisionChunkBlockZ = collisionChunkBlockY;
                collisionChunkBlockZ.z = (float)k;
                if (k < 0) { collisionChunkZ.z -= 1.0f; collisionChunkBlockZ.z = 7.0f; }
                if (k > 7) { collisionChunkZ.z += 1.0f; collisionChunkBlockZ.z = 0.0f; }
                if ((*chunkmap).find(collisionChunkZ) == (*chunkmap).end()) { return obstacles; }
                // std::cout << i << " " << j << " " << position.x << " " << position.y << " " << collisionChunkY.x << " " << collisionChunkY.y << " " << collisionChunkBlockY.x << " " << collisionChunkBlockY.y << " " << (*chunkmap)[collisionChunkY]->getBlock(collisionChunkBlockY)->position.x << " " << (*chunkmap)[collisionChunkY]->getBlock(collisionChunkBlockY)->position.y << " " << (*chunkmap)[collisionChunkY]->getBlock(collisionChunkBlockY)->canCollide << std::endl;
                obstacles.push_back((*chunkmap)[collisionChunkZ]->getBlock(collisionChunkBlockZ));
                // std::cout << collisionChunk.x << ", " << collisionChunk.y << ", " << collisionChunkBlock.x << ", " << collisionChunkBlock.y << ", " << i << ", " << j << std::endl;
            }
        }
    }
    return obstacles;
}

void PhysicsEngine::calculateVelocity(std::shared_ptr<PhysicsObject>& obj) {
    glm::vec3 vel = obj->velocity;
    glm::vec3 pos = obj->getPosition();

    // std::cout << vel.x << " " << vel.y << " " << vel.z << std::endl;

    bool YCollision = false;
    bool XCollision = false;
    bool ZCollision = false;

    std::vector<std::shared_ptr<Object>> obstacles = possibleObstacles(pos);
    if (obstacles.size() < 27) { std::cout << obstacles.size() << std::endl; return; }

    float inertiaAdjusted = 0.002f * obj->mass;
    float gravityAdjusted = 0.002f * obj->mass;

    float rate = (std::abs(vel.x) + std::abs(vel.z)) / 2.0f;
    if (rate == 0.0f) {
        rate = 1.0f;
    }

    // std::cout << (inertiaAdjusted * (std::abs(vel.x) / rate)) << " x" << std::endl;
    // std::cout << (inertiaAdjusted * (std::abs(vel.z) / rate)) << " z" << std::endl;

    if (vel.x < 0) { vel.x += (inertiaAdjusted * (std::abs(vel.x) / rate)); }
    if (vel.x > 0) { vel.x -= (inertiaAdjusted * (std::abs(vel.x) / rate)); }
    if (vel.z < 0) { vel.z += (inertiaAdjusted * (std::abs(vel.z) / rate)); }
    if (vel.z > 0) { vel.z -= (inertiaAdjusted * (std::abs(vel.z) / rate)); }
    if (vel.x < inertiaAdjusted && vel.x > -inertiaAdjusted) { vel.x = 0.0f; }
    if (vel.z < inertiaAdjusted && vel.z > -inertiaAdjusted) { vel.z = 0.0f; }

    vel.y -= gravityAdjusted;

    for (auto obstacle : obstacles) {
        if (possibleCollision(glm::vec3(pos.x + vel.x, pos.y, pos.z), obj->getCollider(), obstacle)) { XCollision = true; }
        if (possibleCollision(glm::vec3(pos.x, pos.y, pos.z + vel.z), obj->getCollider(), obstacle)) { ZCollision = true; }
        if (possibleCollision(glm::vec3(pos.x, pos.y + vel.y, pos.z), obj->getCollider(), obstacle)) { YCollision = true; }
    }

    if (!XCollision) { pos.x += vel.x; }
    else { vel.x = 0.0f; }
    if (!ZCollision) { pos.z += vel.z; }
    else { vel.z = 0.0f; }
    if (!YCollision) { pos.y += vel.y; }
    else { vel.y = 0.0f; }

    // std::cout << XCollision << " " << YCollision << " " << ZCollision << std::endl;
    // std::cout << vel.x << " " << vel.y << " " << vel.z << std::endl;

    obj->setPosition(pos);
    obj->velocity = vel;
}

void PhysicsEngine::registerObject(std::shared_ptr<Object> object, float mass) {
    std::shared_ptr<PhysicsObject> physicsObject = std::make_shared<PhysicsObject>(object, mass);
    registeredObjects.push_back(physicsObject);
}

void PhysicsEngine::unregisterObject(std::shared_ptr<Object> object) {
    auto it = std::find_if(registeredObjects.begin(), registeredObjects.end(), [&object](const std::shared_ptr<PhysicsObject>& obj) {return obj->object == object; });
    if (it != registeredObjects.end()) { registeredObjects.erase(it); }
}

void PhysicsEngine::addVelocityRotation(std::shared_ptr<Object> object, glm::vec3 velocity) {
    auto it = std::find_if(registeredObjects.begin(), registeredObjects.end(), [&object](const std::shared_ptr<PhysicsObject>& obj) {return obj->object == object; });
    if (it != registeredObjects.end()) {
        glm::vec3 rotation = glm::radians(object->rotation);
        glm::vec3 real_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        real_velocity.x = (velocity.x * std::cos(rotation.y) * std::cos(rotation.z)) + (velocity.z * -std::sin(rotation.y) * std::cos(rotation.z));
        real_velocity.y = velocity.y * std::sin(rotation.z);
        real_velocity.z = (velocity.x * std::sin(rotation.y) * std::cos(rotation.z)) + (velocity.z * std::cos(rotation.y) * std::cos(rotation.z));
        it->get()->velocity = glm::vec3(it->get()->velocity.x + real_velocity.x, it->get()->velocity.y + real_velocity.y, it->get()->velocity.z + real_velocity.z);
    }
}

void PhysicsEngine::addVelocityClampedRotation(std::shared_ptr<Object> object, glm::vec3 velocity, glm::vec3 limit) {
    auto it = std::find_if(registeredObjects.begin(), registeredObjects.end(), [&object](const std::shared_ptr<PhysicsObject>& obj) {return obj->object == object; });
    if (it != registeredObjects.end()) {
        glm::vec3 rotation = glm::radians(object->rotation);

        // std::cout << rotation.x << " " << rotation.y << " " << rotation.z << std::endl;
        // std::cout << rotation.y << " " << velocity.x * std::sin(rotation.y) << std::endl;
        // std::cout << rotation.y << " " << velocity.x << " " << std::sin(rotation.y) << std::endl;
        // std::cout << rotation.y << " " << dvelocity.x * std::cosl(rotation.y) * std::cosl(rotation.z) << std::endl;
        // std::cout << rotation.y << " " << dvelocity.z * std::sinl(rotation.y) * std::cosl(rotation.z) << std::endl;
        // std::cout << rotation.y << " " << dvelocity.z * std::cosl(rotation.y) * std::cosl(rotation.z) << std::endl;

        // std::cout << "=====BEGIN=====" << std::endl;
        // std::cout << velocity.x * std::cos(rotation.y) * std::cos(rotation.z) << " xx" << std::endl;
        // std::cout << velocity.z * -std::sin(rotation.y) * std::cos(rotation.z) << " xz" << std::endl;
        // std::cout << "=====" << std::endl;
        // std::cout << velocity.z * std::cos(rotation.y) * std::cos(rotation.z) << " zz" << std::endl;
        // std::cout << velocity.x * std::sin(rotation.y) * std::cos(rotation.z) << " zx" << std::endl;

        glm::vec3 real_limit = glm::vec3(0.0f, 0.0f, 0.0f);
        real_limit.x = (limit.x * std::cos(rotation.y) * std::cos(rotation.z)) + (limit.z * -std::sin(rotation.y) * std::cos(rotation.z));
        real_limit.y = limit.y * std::sin(rotation.z);
        real_limit.z = (limit.x * std::sin(rotation.y) * std::cos(rotation.z)) + (limit.z * std::cos(rotation.y) * std::cos(rotation.z));

        glm::vec3 real_velocity = glm::vec3(0.0f, 0.0f, 0.0f);
        real_velocity.x = std::clamp((velocity.x * std::cos(rotation.y) * std::cos(rotation.z)) + (velocity.z * -std::sin(rotation.y) * std::cos(rotation.z)), -std::abs(real_limit.z), std::abs(real_limit.x));
        real_velocity.y = std::clamp(velocity.y * std::sin(rotation.z), real_limit.y, real_limit.y);
        real_velocity.z = std::clamp((velocity.x * std::sin(rotation.y) * std::cos(rotation.z)) + (velocity.z * std::cos(rotation.y) * std::cos(rotation.z)), -std::abs(real_limit.z), std::abs(real_limit.z));

        // std::cout << "=====" << std::endl;
        // std::cout << real_limit.x << " " << real_limit.y << " " << real_limit.z << std::endl;
        // std::cout << (velocity.x * std::cos(rotation.y) * std::cos(rotation.z)) + (velocity.z * std::sin(rotation.y) * std::cos(rotation.z)) << " " << (velocity.x * std::sin(rotation.y) * std::cos(rotation.z)) + (velocity.z * std::cos(rotation.y) * std::cos(rotation.z)) << std::endl;
        // std::cout << real_velocity.x << " " << real_velocity.z << std::endl;

        it->get()->velocity = glm::vec3(it->get()->velocity.x + real_velocity.x, it->get()->velocity.y + real_velocity.y, it->get()->velocity.z + real_velocity.z);
    }}

void PhysicsEngine::setVelocity(std::shared_ptr<Object> object, glm::vec3 velocity) {
    auto it = std::find_if(registeredObjects.begin(), registeredObjects.end(), [&object](const std::shared_ptr<PhysicsObject>& obj) {return obj->object == object; });
    if (it != registeredObjects.end()) { it->get()->velocity = velocity; }
}

void PhysicsEngine::addVelocity(std::shared_ptr<Object> object, glm::vec3 velocity) {
    auto it = std::find_if(registeredObjects.begin(), registeredObjects.end(), [&object](const std::shared_ptr<PhysicsObject>& obj) {return obj->object == object; });
    if (it != registeredObjects.end()) { it->get()->velocity = glm::vec3(it->get()->velocity.x + velocity.x, it->get()->velocity.y + velocity.y, it->get()->velocity.z + velocity.z); }
}

void PhysicsEngine::addVelocityClamped(std::shared_ptr<Object> object, glm::vec3 velocity, glm::vec3 limit) {
    auto it = std::find_if(registeredObjects.begin(), registeredObjects.end(), [&object](const std::shared_ptr<PhysicsObject>& obj) {return obj->object == object; });
    if (it != registeredObjects.end()) {
        it->get()->velocity = glm::vec3(std::clamp(it->get()->velocity.x + velocity.x, -limit.x, limit.x), std::clamp(it->get()->velocity.y + velocity.y, -limit.y, limit.y), std::clamp(it->get()->velocity.z + velocity.z, -limit.z, limit.z));
    }
}

glm::vec3 PhysicsEngine::getVelocity(std::shared_ptr<Object> object) {
    auto it = std::find_if(registeredObjects.begin(), registeredObjects.end(), [&object](const std::shared_ptr<PhysicsObject>& obj) {return obj->object == object; });
    if (it != registeredObjects.end()) { return it->get()->velocity; }
    return glm::vec3(0.0f, 0.0f, 0.0f);
}

bool PhysicsEngine::isOnFoot(std::shared_ptr<Object> object) {
    auto it = std::find_if(registeredObjects.begin(), registeredObjects.end(), [&object](const std::shared_ptr<PhysicsObject>& obj) {return obj->object == object; });
    if (it != registeredObjects.end()) {
        std::vector<std::shared_ptr<Object>> obstacles = possibleObstacles(it->get()->getPosition());
        for (auto &obstacle : obstacles) {
            if (!obstacle->cancollide) { continue; }

            AABB obj1 = GetAABB::CP2AABB(object->collider, object->position);
            AABB obj2 = GetAABB::CP2AABB(obstacle->collider, obstacle->position);

            bool footcheckd = obj1.AA.y - 0.01f <= obj2.BB.y;
            bool footchecku = obj1.AA.y + 0.01f >= obj2.BB.y;
            bool xcheck1 = obj1.AA.x < obj2.BB.x;
            bool x1check1 = obj1.BB.x > obj2.AA.x;
            bool zcheck1 = obj1.AA.z < obj2.BB.z;
            bool z1check1 = obj1.BB.z > obj2.AA.z;

            // std::cout << footcheckd << " " << footchecku << std::endl;

            if (footcheckd && footchecku && xcheck1 && x1check1 && zcheck1 && z1check1) { return true; }
        }
    }
    return false;
}

void PhysicsEngine::step() {
    for (std::shared_ptr<PhysicsObject> object : registeredObjects) {
        calculateVelocity(object);
    }
}