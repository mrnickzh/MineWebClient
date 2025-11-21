#include "BlockObject.hpp"

#include "../main.hpp"

#include "../Shader.hpp"
#include "../Utils/VertexManager.hpp"

BlockObject::BlockObject(glm::vec3 position, glm::vec3 rotation, int vboid, int tid) : Object(position, rotation, vboid, tid) {

}

void BlockObject::render() { }

void BlockObject::setposition(glm::vec3 pos) {
    position = pos;
}

void BlockObject::setrotation(glm::vec3 rot) {
    rotation = rot;
}

