#pragma once
#include "Object.hpp"

#include <GLES3/gl3.h>

#define GLM_FORCE_PURE
#include "../../lib/glm/glm.hpp"
#include "../../lib/glm/gtc/matrix_transform.hpp"
#include "../../lib/glm/gtc/type_ptr.hpp"

class LightObject : public Object {
public:
    int lightIntensity;

    LightObject(glm::vec3 position, glm::vec3 rotation, int vboid, int tid, bool cancollide, glm::vec3 collider, int light);

    void render() override;
    void setposition(glm::vec3 position) override;
    void setrotation(glm::vec3 rotation) override;
    void setlight(int level, int side) override;
};
