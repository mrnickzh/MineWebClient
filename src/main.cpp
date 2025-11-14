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
#include "Protocol/Packets/HandShakePacket.hpp"

Camera* ourCamera;

GLFWwindow* window = nullptr;

int windowWidth = 800;
int windowHeight = 600;

float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float lastX = 400, lastY = 300;

float yaw = -90.0f;
float pitch = 0.0f;

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

void mainLoop() {
    glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Main::ourShader->use();

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

    Main::ourShader->setMat4("projection", projection);
    Main::ourShader->setMat4("view", ourCamera->GetViewMatrix());

    for (std::shared_ptr<Object> obj : Main::objects) {
        obj->render();
    }

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


    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);

    glEnable(GL_DEPTH_TEST);

    Main::ourShader = new Shader("/assets/shaders/vertex.glsl", "/assets/shaders/fragment.glsl");
    ourCamera = new Camera();

    Main::isSingleplayer = true;
    Main::serverInstance.setCallback(SocketClient::on_message);
    SocketClient::on_open();

    glfwMakeContextCurrent(window);
    glViewport(0, 0, windowWidth, windowHeight);
    emscripten_set_main_loop(&mainLoop, 0, 1);

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

