#pragma once

#define GLM_FORCE_PURE
#include "../../lib/glm/glm.hpp"
#include "../../lib/glm/gtc/matrix_transform.hpp"
#include "../../lib/glm/gtc/type_ptr.hpp"

struct AABB {
    glm::vec3 AA;
    glm::vec3 BB;
};

class GetAABB {
public:
    static AABB CP2AABB(glm::vec3 collider, glm::vec3 position) {
        glm::vec3 aa = position - collider;
        glm::vec3 bb = position + collider;
        return AABB(aa, bb);
    }
};