#include "BillboardObject.hpp"

#include "../main.hpp"

#include "../Shader.hpp"

float billboard_vertex[] = {
    // Left face
    0.5f,  0.5f,
    -0.5f,  0.5f,
    -0.5f, -0.5f,
    -0.5f, -0.5f,
    0.5f, -0.5f,
    0.5f,  0.5f
};

BillboardObject::BillboardObject(glm::vec3 position) : Object(position, glm::vec3(0.0f, 0.0f, 0.0f), 0, 0, false, glm::vec3(0.5f, 0.5f, 0.5f)) {
    model = glm::translate(glm::mat4(1.0f), position);
    // model *= glm::mat4_cast(glm::quat(glm::radians(-rotation)));

    ftexture = (float)texture;
    for (int i = 0; i < 6; i++) {
        lightLevels[i].y = 0.0f;
    }

    glGenVertexArrays(1, &VAO);

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(billboard_vertex), billboard_vertex, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &IVBO);
    glBindBuffer(GL_ARRAY_BUFFER, IVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), glm::value_ptr(model), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glGenBuffers(1, &TBO);
    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float), &ftexture, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BillboardObject::render() {
    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
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

    // glBindBuffer(GL_ARRAY_BUFFER, TBO);
    // glEnableVertexAttribArray(2);
    // glVertexAttribPointer(2, 1, GL_FLOAT, GL_FALSE, sizeof(float), (void*)0);
    // glVertexAttribDivisor(2, 1);
    // glBindBuffer(GL_ARRAY_BUFFER, 0);

    // for (int i = 0; i < 4; i++) {
    //     for (int j = 0; j < 4; j++) {
    //         std::cout << model[i][j] << std::endl;
    //     }
    // }

    glDrawArrays(GL_TRIANGLES, 0, 6);

    glBindVertexArray(0);
}

void BillboardObject::setposition(glm::vec3 pos) {
    position = pos;

    model = glm::translate(glm::mat4(1.0f), position);
    // model *= glm::mat4_cast(glm::quat(glm::radians(-rotation)));

    glBindBuffer(GL_ARRAY_BUFFER, IVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), glm::value_ptr(model), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float), &ftexture, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BillboardObject::setrotation(glm::vec3 rot) {
    rotation = rot;

    model = glm::translate(glm::mat4(1.0f), position);
    // model *= glm::mat4_cast(glm::quat(glm::radians(-rotation)));

    glBindBuffer(GL_ARRAY_BUFFER, IVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), glm::value_ptr(model), GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glBindBuffer(GL_ARRAY_BUFFER, TBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float), &ftexture, GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void BillboardObject::setlight(int level, int side) {
    lightLevels[side].x = (float)level / 5.0f;
}

void BillboardObject::setdarkness(int level, int side) {
    lightLevels[side].y = (float)level / 5.0f;
}


