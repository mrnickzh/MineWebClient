#include <emscripten.h>
#include <iostream>
#include <emscripten/html5.h>
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>

#include "main.hpp"

#define GLM_FORCE_PURE
#include "../lib/glm/glm.hpp"
#include "../lib/glm/gtc/matrix_transform.hpp"
#include "../lib/glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Camera.hpp"
#include "Objects/BlockObject.hpp"

#include "../MineWebServer/src/Server.hpp"
#include "Protocol/PacketHelper.hpp"
#include "Protocol/Socket.hpp"
#include "Protocol/Packets/AddMapObject.hpp"
#include "Protocol/Packets/GenerateChunk.hpp"
#include "Protocol/Packets/HandShakePacket.hpp"
#include "Utils/VertexManager.hpp"

float vertices[] = {
    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f, // bottom-right
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, // top-right
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f, // top-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    // Left face
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-left
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-right
    // Right face
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // bottom-right
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // top-left
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f, // top-left
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, // bottom-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f, // top-right
    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f, // top-right
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f, // bottom-right
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f, // top-left
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left
};

Camera* ourCamera;

GLFWwindow* window = nullptr;

int windowWidth = 800;
int windowHeight = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = 400, lastY = 300;

GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;

bool firstMouse = true;

EM_BOOL onResize(int, const EmscriptenUiEvent* e, void*) {
    windowWidth = e->windowInnerWidth;
    windowHeight = e->windowInnerHeight;
    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetWindowSize(window, windowWidth, windowHeight);

    return EM_TRUE;
}

void processInput(GLFWwindow *window)
{
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        ourCamera->ProcessKeyboard(FORWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        ourCamera->ProcessKeyboard(BACKWARD, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        ourCamera->ProcessKeyboard(LEFT, deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        ourCamera->ProcessKeyboard(RIGHT, deltaTime);
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        emscripten_request_pointerlock("canvas", EM_TRUE);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    EmscriptenPointerlockChangeEvent lastChangeEvent;
    auto haveLockInfo = emscripten_get_pointerlock_status(&lastChangeEvent) == EMSCRIPTEN_RESULT_SUCCESS;
    if (!haveLockInfo) {
        std::cout << "Can't get pointer lock info" << std::endl;
        return;
    }

    if (!lastChangeEvent.isActive) {
        return;
    }

    if (firstMouse)
    {
        lastX = (float)xpos;
        lastY = (float)ypos;
        firstMouse = false;
    }

    float xoffset = (float)xpos - lastX;
    float yoffset = lastY - (float)ypos;
    lastX = (float)xpos;
    lastY = (float)ypos;

    ourCamera->ProcessMouseMovement(xoffset, yoffset);
}

void preRender() {
    glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Main::ourShader->use();

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

    glUniformMatrix4fv(Main::ourShader->uniforms["projection"], 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(Main::ourShader->uniforms["view"], 1, GL_FALSE, &ourCamera->GetViewMatrix()[0][0]);
}

void render() {
    Main::chunks[glm::vec3(0.0f, 0.0f, 0.0f)]->renderChunk();

    // for (std::shared_ptr<Object>& obj : Main::objects) {
    //     obj->render();
    // }
    //
    // Main::objects.clear();
}

void mainLoop() {
    preRender();

    render();

    processInput(window);
    glfwSwapBuffers(window);
    glfwPollEvents();

    float currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;
}

int main() {
    emscripten_get_canvas_element_size("#canvas", &windowWidth, &windowHeight);
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, onResize);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    window = glfwCreateWindow(windowWidth, windowHeight, "MineWeb", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    PacketHelper::registerPacket(0, []() { return new HandShakePacket(); });
    PacketHelper::registerPacket(1, []() { return new AddMapObject(); });
    PacketHelper::registerPacket(2, []() { return new GenerateChunk(); });

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    Main::ourShader = new Shader("/assets/shaders/vertex.glsl", "/assets/shaders/fragment.glsl");
    ourCamera = new Camera();

    Main::vertexManager = new VertexManager();
    Main::vertexManager->initVBO(0, vertices, sizeof(vertices));

    Main::textureManager = new TextureManager();
    Main::textureManager->startInit();
    Main::textureManager->addTexture("/assets/textures/stone.png", 1);
    Main::textureManager->addTexture("/assets/textures/dirt.png", 2);
    Main::textureManager->addTexture("/assets/textures/grass.png", 3);
    Main::textureManager->endInit();

    Main::isSingleplayer = true;
    Main::serverInstance.setCallback(SocketClient::on_message);
    SocketClient::on_open();
    // SocketClient::connect();

    GenerateChunk pkt;
    pkt.chunkpos = glm::vec3(0.0f, 0.0f, 0.0f);
    SocketClient::sendPacket(&pkt);

    glfwMakeContextCurrent(window);
    glViewport(0, 0, windowWidth, windowHeight);
    emscripten_set_main_loop(&mainLoop, 0, 1);

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

