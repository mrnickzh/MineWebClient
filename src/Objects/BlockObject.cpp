#include "BlockObject.hpp"

#include "../main.hpp"

#include "../Shader.hpp"
#include "../Utils/VertexManager.hpp"

BlockObject::BlockObject(glm::vec3 position, glm::vec3 rotation, int vboid, int tid, bool cancollide, glm::vec3 collider) : Object(position, rotation, vboid, tid, cancollide, collider) {
    model = glm::translate(glm::mat4(1.0f), position);
    model *= glm::mat4_cast(glm::quat(glm::radians(-rotation)));
}

void BlockObject::render() { }

void BlockObject::setposition(glm::vec3 pos) {
    position = pos;

    model = glm::translate(glm::mat4(1.0f), position);
    model *= glm::mat4_cast(glm::quat(glm::radians(-rotation)));
}

void BlockObject::setrotation(glm::vec3 rot) {
    rotation = rot;

    model = glm::translate(glm::mat4(1.0f), position);
    model *= glm::mat4_cast(glm::quat(glm::radians(-rotation)));
}

