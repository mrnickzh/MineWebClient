#pragma once
#include "Object.hpp"

#include <GLES3/gl3.h>

#define GLM_FORCE_PURE
#include "../../lib/glm/glm.hpp"
#include "../../lib/glm/gtc/matrix_transform.hpp"
#include "../../lib/glm/gtc/type_ptr.hpp"

class BlockObject : public Object {
public:
    GLuint VAO = 0, VBO = 0;
    GLuint texture;
    glm::mat4 model;

    BlockObject(glm::vec3 position, glm::vec3 rotation, int vuid);

    void render() override;
    void setposition(glm::vec3 position) override;
    void setrotation(glm::vec3 rotation) override;
};
