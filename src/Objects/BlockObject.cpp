#include "BlockObject.hpp"

#include "../main.hpp"

#include "../Shader.hpp"
#include "../Utils/VertexManager.hpp"

BlockObject::BlockObject(glm::vec3 position, glm::vec3 rotation, int vuid) : Object(position, rotation, vuid) {
    model = glm::mat4(1.0f);
    model = glm::translate(model, position);
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

    VertexUnit vunit = Main::vertexManager->getVAO(vuid);
    VAO = vunit.vao;
    VBO = vunit.vbo;
    texture = vunit.texture;
}

void BlockObject::render() {
    // std::cout << model[0].x << " " << model[0].y << " " << model[0].z << " " << model[0].w << std::endl;
    // std::cout << model[1].x << " " << model[1].y << " " << model[1].z << " " << model[1].w << std::endl;
    // std::cout << model[2].x << " " << model[2].y << " " << model[2].z << " " << model[2].w << std::endl;
    // std::cout << model[3].x << " " << model[3].y << " " << model[3].z << " " << model[3].w << std::endl;
    // std::cout << Main::ourShader->uniforms["model"] << std::endl;
    glUniformMatrix4fv(Main::ourShader->uniforms["model"], 1, GL_FALSE, &model[0][0]);
    glBindTexture(GL_TEXTURE_2D, texture);
    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 36);
}

void BlockObject::setposition(glm::vec3 pos) {
    position = pos;
    model = glm::translate(model, position);
}

void BlockObject::setrotation(glm::vec3 rot) {
    rotation = rot;
    model = glm::rotate(model, glm::radians(rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, glm::radians(rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));
}

