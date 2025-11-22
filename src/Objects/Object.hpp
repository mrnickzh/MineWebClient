#pragma once

#define GLM_FORCE_PURE
#include "../../lib/glm/glm.hpp"
#include "../../lib/glm/gtc/matrix_transform.hpp"
#include "../../lib/glm/gtc/type_ptr.hpp"

class Object {
public:
    glm::vec3 position;
    glm::vec3 rotation;
    glm::mat4 model;
    int vboid;
    int texture;

    Object(glm::vec3 position, glm::vec3 rotation, int vboid, int texture) : position(position), rotation(rotation), vboid(vboid), texture(texture) {};

    virtual void render() {}
    virtual void setposition(glm::vec3 position) {}
    virtual void setrotation(glm::vec3 rotation) {}
};
