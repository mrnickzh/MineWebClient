#pragma once
#include "Object.hpp"

#include <GLES3/gl3.h>

#define GLM_FORCE_PURE
#include "lib/glm/glm.hpp"
#include "lib/glm/gtc/matrix_transform.hpp"
#include "lib/glm/gtc/type_ptr.hpp"

class BillboardObject : public Object {
public:
    GLuint VAO = 0;
    GLuint TBO = 0;
    GLuint IVBO = 0;
    GLuint VBO = 0;
    float ftexture;

    BillboardObject(glm::vec3 position);

    void render() override;
    void setposition(glm::vec3 position) override;
    void setrotation(glm::vec3 rotation) override;
};
