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
#include "Event/EventBus.hpp"
#include "Event/Events/TickEvent.hpp"
#include "Objects/EntityObject.hpp"
#include "Protocol/PacketHelper.hpp"
#include "Protocol/Socket.hpp"
#include "Protocol/Packets/AddMapObject.hpp"
#include "Protocol/Packets/GenerateChunk.hpp"
#include "Protocol/Packets/HandShakePacket.hpp"
#include "Utils/VertexManager.hpp"

float vertices[] = {
    // Back face
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // Bottom-left
     0.5f,  0.5f, -0.5f,  0.25f, 0.25f, // top-right
     0.5f, -0.5f, -0.5f,  0.25f, 0.0f, // bottom-right
     0.5f,  0.5f, -0.5f,  0.25f, 0.25f, // top-right
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f, // bottom-left
    -0.5f,  0.5f, -0.5f,  0.0f, 0.25f, // top-left
    // Front face
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
     0.5f, -0.5f,  0.5f,  0.25f, 0.0f, // bottom-right
     0.5f,  0.5f,  0.5f,  0.25f, 0.25f, // top-right
     0.5f,  0.5f,  0.5f,  0.25f, 0.25f, // top-right
    -0.5f,  0.5f,  0.5f,  0.0f, 0.25f, // top-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    // Left face
    -0.5f,  0.5f,  0.5f,  0.25f, 0.0f, // top-right
    -0.5f,  0.5f, -0.5f,  0.25f, 0.25f, // top-left
    -0.5f, -0.5f, -0.5f,  0.0f, 0.25f, // bottom-left
    -0.5f, -0.5f, -0.5f,  0.0f, 0.25f, // bottom-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    -0.5f,  0.5f,  0.5f,  0.25f, 0.0f, // top-right
    // Right face
     0.5f,  0.5f,  0.5f,  0.25f, 0.0f, // top-left
     0.5f, -0.5f, -0.5f,  0.0f, 0.25f, // bottom-right
     0.5f,  0.5f, -0.5f,  0.25f, 0.25f, // top-right
     0.5f, -0.5f, -0.5f,  0.0f, 0.25f, // bottom-right
     0.5f,  0.5f,  0.5f,  0.25f, 0.0f, // top-left
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-left
    // Bottom face
    -0.5f, -0.5f, -0.5f,  0.0f, 0.25f, // top-right
     0.5f, -0.5f, -0.5f,  0.25f, 0.25f, // top-left
     0.5f, -0.5f,  0.5f,  0.25f, 0.0f, // bottom-left
     0.5f, -0.5f,  0.5f,  0.25f, 0.0f, // bottom-left
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f, // bottom-right
    -0.5f, -0.5f, -0.5f,  0.0f, 0.25f, // top-right
    // Top face
    -0.5f,  0.5f, -0.5f,  0.0f, 0.25f, // top-left
     0.5f,  0.5f,  0.5f,  0.25f, 0.0f, // bottom-right
     0.5f,  0.5f, -0.5f,  0.25f, 0.25f, // top-right
     0.5f,  0.5f,  0.5f,  0.25f, 0.0f, // bottom-right
    -0.5f,  0.5f, -0.5f,  0.0f, 0.25f, // top-left
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f  // bottom-left
};

float playervertices[] = {
    // Back face
    -0.25f, -0.75f, -0.25f,  0.0f, 0.0f, // Bottom-left
     0.25f,  0.75f, -0.25f,  0.25f, 0.375f, // top-right
     0.25f, -0.75f, -0.25f,  0.25f, 0.0f, // bottom-right
     0.25f,  0.75f, -0.25f,  0.25f, 0.375f, // top-right
    -0.25f, -0.75f, -0.25f,  0.0f, 0.0f, // bottom-left
    -0.25f,  0.75f, -0.25f,  0.0f, 0.375f, // top-left
    // Front face
    -0.25f, -0.75f,  0.25f,  0.0f, 0.0f, // bottom-left
     0.25f, -0.75f,  0.25f,  0.25f, 0.0f, // bottom-right
     0.25f,  0.75f,  0.25f,  0.25f, 0.375f, // top-right
     0.25f,  0.75f,  0.25f,  0.25f, 0.375f, // top-right
    -0.25f,  0.75f,  0.25f,  0.0f, 0.375f, // top-left
    -0.25f, -0.75f,  0.25f,  0.0f, 0.0f, // bottom-left
    // Left face
    -0.25f,  0.75f,  0.25f,  0.0f, 0.0f, // top-right
    -0.25f,  0.75f, -0.25f,  0.25f, 0.0f, // top-left
    -0.25f, -0.75f, -0.25f,  0.25f, 0.375f, // bottom-left
    -0.25f, -0.75f, -0.25f,  0.25f, 0.375f, // bottom-left
    -0.25f, -0.75f,  0.25f,  0.0f, 0.375f, // bottom-right
    -0.25f,  0.75f,  0.25f,  0.0f, 0.0f, // top-right
    // Right face
     0.25f,  0.75f,  0.25f,  0.25f, 0.0f, // top-left
     0.25f, -0.75f, -0.25f,  0.5f, 0.375f, // bottom-right
     0.25f,  0.75f, -0.25f,  0.5f, 0.0f, // top-right
     0.25f, -0.75f, -0.25f,  0.5f, 0.375f, // bottom-right
     0.25f,  0.75f,  0.25f,  0.25f, 0.0f, // top-left
     0.25f, -0.75f,  0.25f,  0.25f, 0.375f, // bottom-left
    // Bottom face
    -0.25f, -0.75f, -0.25f,  0.5f, 0.25f, // top-right
     0.25f, -0.75f, -0.25f,  0.75f, 0.25f, // top-left
     0.25f, -0.75f,  0.25f,  0.75f, 0.0f, // bottom-left
     0.25f, -0.75f,  0.25f,  0.75f, 0.0f, // bottom-left
    -0.25f, -0.75f,  0.25f,  0.5f, 0.0f, // bottom-right
    -0.25f, -0.75f, -0.25f,  0.5f, 0.25f, // top-right
    // Top face
    -0.25f,  0.75f, -0.25f,  0.5f, 0.25f, // top-left
     0.25f,  0.75f,  0.25f,  0.75f, 0.0f, // bottom-right
     0.25f,  0.75f, -0.25f,  0.75f, 0.25f, // top-right
     0.25f,  0.75f,  0.25f,  0.75f, 0.0f, // bottom-right
    -0.25f,  0.75f, -0.25f,  0.5f, 0.25f, // top-left
    -0.25f,  0.75f,  0.25f,  0.5f, 0.0f  // bottom-left
};

Camera* ourCamera;

GLFWwindow* window = nullptr;

int windowWidth = 800;
int windowHeight = 600;

float deltaTime = 0.0f;
float lastFrame = 0.0f;

float maxH = 0.05f;

float lastX = 400, lastY = 300;

GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;

bool firstMouse = true;
bool freeCamLock = false;

int tps = 60;
int mps = 1000 / tps;
long long ltt = duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();

void do_tick() {
    long long now = duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    long long elapsed = now - ltt;

    if (elapsed >= mps) {
        int ticksToRun = (int) (elapsed / mps);
        for (int i = 0; i < ticksToRun; i++) {
            TickEvent event;
            EventBus::getInstance().publish(&event);
        }
        ltt = duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
}

EM_BOOL onResize(int, const EmscriptenUiEvent* e, void*) {
    windowWidth = e->windowInnerWidth;
    windowHeight = e->windowInnerHeight;
    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetWindowSize(window, windowWidth, windowHeight);

    return EM_TRUE;
}

void processInput(GLFWwindow *window)
{
    glm::vec3 rotation = Main::localPlayer->object->rotation;
    if (!ourCamera->freeCam) {
        glm::vec3 totalvelocity = glm::vec3(0.0f, 0.0f, 0.0f);
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            totalvelocity += glm::vec3(0.0025f, 0.0f, 0.0f);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            totalvelocity += glm::vec3(-0.0025f, 0.0f, 0.0f);
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            totalvelocity += glm::vec3(0.0f, 0.0f, -0.0025f);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            totalvelocity += glm::vec3(0.0f, 0.0f, 0.0025f);
        Main::physicsEngine->addVelocityClampedRotation(Main::localPlayer->object, totalvelocity, glm::vec3(maxH, 0.1f, maxH));
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS && Main::physicsEngine->isOnFoot(Main::localPlayer->object)) {
            Main::physicsEngine->addVelocityClamped(Main::localPlayer->object, glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(0.0f, 0.1f, 0.0f));
        }
    }
    else {
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            ourCamera->Position += glm::vec3(1.0f * deltaTime, 0.0f, 1.0f * deltaTime) * ourCamera->Front;
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            ourCamera->Position += glm::vec3(-1.0f * deltaTime, 0.0f, -1.0f * deltaTime) * ourCamera->Front;
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            ourCamera->Position += glm::vec3(-1.0f * deltaTime, 0.0f, -1.0f * deltaTime) * ourCamera->Right;
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            ourCamera->Position += glm::vec3(1.0f * deltaTime, 0.0f, 1.0f * deltaTime) * ourCamera->Right;
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS)
        emscripten_request_pointerlock("canvas", EM_TRUE);

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS && !freeCamLock) {
        ourCamera->freeCam = !ourCamera->freeCam;
        ourCamera->Yaw = rotation.y;
        freeCamLock = true;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_RELEASE && freeCamLock) {
        freeCamLock = false;
    }
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
    do_tick();
    glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    Main::ourShader->use();

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(45.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

    glUniformMatrix4fv(Main::ourShader->uniforms["projection"], 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(Main::ourShader->uniforms["view"], 1, GL_FALSE, &ourCamera->GetViewMatrix()[0][0]);
}

void render() {
    glm::vec3 playerPos = Main::localPlayer->object->position;
    glm::vec3 playerChunk = glm::vec3(floor(playerPos.x / 8.0f), floor(playerPos.y / 8.0f), floor(playerPos.z / 8.0f));

    // std::cout << playerChunk.x << " " << playerChunk.y << " " << playerChunk.z << std::endl;

    for (int i = -1; i < 2; i++) {
        for (int j = -1; j < 2; j++) {
            for (int k = -1; k < 2; k++) {
                glm::vec3 requestedChunk = glm::vec3(playerChunk.x + (float)i, playerChunk.y + (float)j, playerChunk.z + (float)k);

                if (Main::chunks.find(requestedChunk) == Main::chunks.end()) {
                    if (std::find(Main::requestedChunks.begin(), Main::requestedChunks.end(), requestedChunk) == Main::requestedChunks.end()) {
                        Main::requestedChunks.push_back(requestedChunk);
                        GenerateChunk pkt;
                        pkt.chunkpos = requestedChunk;
                        SocketClient::sendPacket(&pkt);
                    }
                    continue;
                }

                // if (Main::map[requestedChunk]->blocks.size() > 256) {
                //     std::cout << requestedChunk.x << " " << requestedChunk.y << std::endl;
                // }

                std::shared_ptr<ChunkMap> chunk = Main::chunks[requestedChunk];
                chunk->renderChunk();
            }
        }
    }

    Main::localPlayer->object->render();

    // AABB kek = GetAABB::CP2AABB(Main::chunks[glm::vec3(0.0f, -1.0f, 0.0f)]->getBlock(glm::vec3(0.0f, 0.0f, 0.0f))->collider, Main::chunks[glm::vec3(0.0f, -1.0f, 0.0f)]->getBlock(glm::vec3(0.0f, 0.0f, 0.0f))->position);
    // std::cout << kek.AA.x << " " << kek.AA.y << " " << kek.AA.z << std::endl;
    // std::cout << kek.BB.x << " " << kek.BB.y << " " << kek.BB.z << std::endl;

    // std::cout << "rot: " << Main::localPlayer->object->rotation.y << std::endl;
    // std::cout << "yaw: " << ourCamera->Yaw << std::endl;

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

    Main::vertexManager = new VertexManager();
    Main::vertexManager->initVBO(0, vertices, sizeof(vertices));
    Main::vertexManager->initVBO(1, playervertices, sizeof(playervertices));

    Main::textureManager = new TextureManager();
    Main::textureManager->startInit(64, 64, 64);

    // blocks
    Main::textureManager->addTexture("/assets/textures/stone.png", 1);
    Main::textureManager->addTexture("/assets/textures/dirt.png", 2);
    Main::textureManager->addTexture("/assets/textures/grass.png", 3);

    //entities
    Main::textureManager->addTexture("/assets/textures/player.png", 49);

    Main::textureManager->endInit();

    L_SUBSCRIBE(TickEvent, [](TickEvent* event) {
        Main::physicsEngine->step();
        if (!ourCamera->freeCam) {
            ourCamera->updateCameraPosition();
        }
        else {
            ourCamera->updateCameraVectors();
        }
    });

    Main::physicsEngine = std::make_unique<PhysicsEngine>(&Main::chunks);

    Main::isSingleplayer = true;
    Main::serverInstance.setCallback(SocketClient::on_message);
    SocketClient::on_open();
    // SocketClient::connect();

    Main::localPlayer = std::make_shared<Entity>();
    Main::localPlayer->uuid = "local";
    Main::localPlayer->object = std::make_shared<EntityObject>(glm::vec3(0.0, 1.0f, 0.0), glm::vec3(0.0f, 0.0f, 0.0f), 1, 49, true, glm::vec3(0.375f, 0.75f, 0.375f));

    Main::physicsEngine->registerObject(Main::localPlayer->object, 1.0f);

    ourCamera = new Camera(Main::localPlayer);

    glfwMakeContextCurrent(window);
    glViewport(0, 0, windowWidth, windowHeight);
    emscripten_set_main_loop(&mainLoop, 0, 1);

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

