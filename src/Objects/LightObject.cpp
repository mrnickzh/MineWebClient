#include "LightObject.hpp"

#include "../main.hpp"

#include "../Shader.hpp"
#include "../Utils/VertexManager.hpp"

LightObject::LightObject(glm::vec3 position, glm::vec3 rotation, int vboid, int tid, bool cancollide, glm::vec3 collider, int light) : Object(position, rotation, vboid, tid, cancollide, collider) {
    model = glm::translate(glm::mat4(1.0f), position);
    model *= glm::mat4_cast(glm::quat(glm::radians(-rotation)));
    lightIntensity = light;
}

void LightObject::render() { }

void LightObject::setposition(glm::vec3 pos) {
    position = pos;

    model = glm::translate(glm::mat4(1.0f), position);
    model *= glm::mat4_cast(glm::quat(glm::radians(-rotation)));
}

void LightObject::setrotation(glm::vec3 rot) {
    rotation = rot;

    model = glm::translate(glm::mat4(1.0f), position);
    model *= glm::mat4_cast(glm::quat(glm::radians(-rotation)));
}

void LightObject::setlight(int level, int side) {
    lightLevels[side] = (float)level / 10.0f;
}

