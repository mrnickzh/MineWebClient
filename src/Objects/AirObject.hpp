#pragma once
#include "Object.hpp"

#define GLM_FORCE_PURE
#include "../../lib/glm/glm.hpp"
#include "../../lib/glm/gtc/matrix_transform.hpp"
#include "../../lib/glm/gtc/type_ptr.hpp"

class AirObject : public Object {
public:
    AirObject(glm::vec3 position, glm::vec3 rotation) : Object(position, rotation, 0, 0) {};

    void render() override {};
    void setposition(glm::vec3 position) override {};
    void setrotation(glm::vec3 rotation) override {};
};
