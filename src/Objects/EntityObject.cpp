#include "EntityObject.hpp"

#include "../main.hpp"

#include "../Shader.hpp"
#include "../Utils/VertexManager.hpp"

EntityObject::EntityObject(glm::vec3 position, glm::vec3 rotation, int vboid, int tid, bool cancollide, glm::vec3 collider) : Object(position, rotation, vboid, tid, cancollide, collider) {
    model = glm::translate(glm::mat4(1.0f), position);
    model *= glm::mat4_cast(glm::quat(glm::radians(-rotation)));

    ftexture = (float)texture;
    for (int i = 0; i < 6; i++) {
        lightLevels[i].y = 0.0f;
    }

    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &IVBO);
    glBindBuffer(GL_ARRAY_BUFFER, IVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), glm::value_ptr(model), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &TBO);
    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float), &ftexture, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &LBO);
    glBindBuffer(GL_ARRAY_BUFFER, LBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 2, &lightLevels[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void EntityObject::render() {
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, Main::vertexManager->getVBO(vboid));
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glVertexAttribDivisor(2, 1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, IVBO);
    glEnableVertexAttribArray(3);
    glEnableVertexAttribArray(4);
    glEnableVertexAttribArray(5);
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(0));
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(4 * sizeof(float)));
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(8 * sizeof(float)));
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * 4 * sizeof(float), (void*)(12 * sizeof(float)));
    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, LBO);
    glEnableVertexAttribArray(7);
    glVertexAttribPointer(7, 2, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    glVertexAttribDivisor(7, 1);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // for (int i = 0; i < 4; i++) {
    //     for (int j = 0; j < 4; j++) {
    //         std::cout << model[i][j] << std::endl;
    //     }
    // }

    glDrawArrays(GL_TRIANGLES, 0, 36);

    glBindVertexArray(0);
}

void EntityObject::setposition(glm::vec3 pos) {
    position = pos;

    model = glm::translate(glm::mat4(1.0f), position);
    model *= glm::mat4_cast(glm::quat(glm::radians(-rotation)));

    glDeleteBuffers(1, &IVBO);
    glGenBuffers(1, &IVBO);
    glBindBuffer(GL_ARRAY_BUFFER, IVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), glm::value_ptr(model), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDeleteBuffers(1, &TBO);
    glGenBuffers(1, &TBO);
    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float), &ftexture, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &LBO);
    glBindBuffer(GL_ARRAY_BUFFER, LBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36 * 2, &lightLevels[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void EntityObject::setrotation(glm::vec3 rot) {
    rotation = rot;

    model = glm::translate(glm::mat4(1.0f), position);
    model *= glm::mat4_cast(glm::quat(glm::radians(-rotation)));

    glDeleteBuffers(1, &IVBO);
    glGenBuffers(1, &IVBO);
    glBindBuffer(GL_ARRAY_BUFFER, IVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), glm::value_ptr(model), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glDeleteBuffers(1, &TBO);
    glGenBuffers(1, &TBO);
    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float), &ftexture, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &LBO);
    glBindBuffer(GL_ARRAY_BUFFER, LBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 36, &lightLevels[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void EntityObject::setlight(int level, int side) {
    lightLevels[side].x = (float)level / 5.0f;
}

void EntityObject::setdarkness(int level, int side) {
    lightLevels[side].y = (float)level / 5.0f;
}


