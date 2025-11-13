#pragma once

#define GLM_FORCE_PURE
#include "../../lib/glm/glm.hpp"
#include "../../lib/glm/gtc/matrix_transform.hpp"
#include "../../lib/glm/gtc/type_ptr.hpp"

class Object {
public:
    glm::vec3 position;
    glm::vec3 rotation;

    Object(glm::vec3 position, glm::vec3 rotation) : position(position), rotation(rotation) {};

    virtual void render() {}
};
