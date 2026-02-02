#pragma once
#include "Object.hpp"

#include <GLES3/gl3.h>

#define GLM_FORCE_PURE
#include "../../lib/glm/glm.hpp"
#include "../../lib/glm/gtc/matrix_transform.hpp"
#include "../../lib/glm/gtc/type_ptr.hpp"

class EntityObject : public Object {
public:
    GLuint VAO = 0;
    GLuint TBO = 0;
    GLuint IVBO = 0;
    float ftexture;

    EntityObject(glm::vec3 position, glm::vec3 rotation, int vboid, int tid, bool cancollide, glm::vec3 collider);

    void render() override;
    void setposition(glm::vec3 position) override;
    void setrotation(glm::vec3 rotation) override;
    void setlight(int level, int side) override;
    void setdarkness(int level, int side) override;
};
