#include <emscripten.h>
#include <iostream>
#include <emscripten/html5.h>
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
#include <thread>

#include "main.hpp"

#define GLM_FORCE_PURE
#include "../lib/glm/glm.hpp"
#include "../lib/glm/gtc/matrix_transform.hpp"
#include "../lib/glm/gtc/type_ptr.hpp"

#include "Shader.hpp"
#include "Camera.hpp"
#include "InputHandler.hpp"
#include "Objects/BlockObject.hpp"

#include "../MineWebServer/src/Server.hpp"
#include "Event/EventBus.hpp"
#include "Event/Events/KeyEvent.hpp"
#include "Event/Events/MouseMoveEvent.hpp"
#include "Event/Events/TickEvent.hpp"
#include "GUI/Elements/EnterElement.hpp"
#include "GUI/Elements/TextElement.hpp"
#include "Objects/EntityObject.hpp"
#include "Protocol/PacketHelper.hpp"
#include "Protocol/Socket.hpp"
#include "Protocol/Packets/EditChunk.hpp"
#include "Protocol/Packets/EntityAction.hpp"
#include "Protocol/Packets/GenerateChunk.hpp"
#include "Protocol/Packets/HandShakePacket.hpp"
#include "Protocol/Packets/PlayerAuthInput.hpp"
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

GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;

bool freeCamLock = false;
bool placeLock = false;
bool breakLock = false;

int renderDistance = 5;
Frustum cameraFrustum;

bool checkPointerLock() {
    EmscriptenPointerlockChangeEvent lastChangeEvent;
    auto haveLockInfo = emscripten_get_pointerlock_status(&lastChangeEvent) == EMSCRIPTEN_RESULT_SUCCESS;
    if (!haveLockInfo) {
        std::cout << "Can't get pointer lock info" << std::endl;
        return false;
    }

    if (!lastChangeEvent.isActive) {
        return false;
    }

    return true;
}

EM_BOOL onResize(int, const EmscriptenUiEvent* e, void*) {
    windowWidth = e->windowInnerWidth;
    windowHeight = e->windowInnerHeight;
    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetWindowSize(window, windowWidth, windowHeight);

    return EM_TRUE;
}

void processInput()
{
    if (Main::serverConnected) {
        glm::vec3 rotation = Main::localPlayer->object->rotation;
        if (!ourCamera->freeCam) {
            glm::vec3 totalvelocity = glm::vec3(0.0f, 0.0f, 0.0f);
            if (InputHandler::isKeyPressed("KeyW"))
                totalvelocity += glm::vec3(0.05f, 0.0f, 0.0f);
            if (InputHandler::isKeyPressed("KeyS"))
                totalvelocity += glm::vec3(-0.05f, 0.0f, 0.0f);
            if (InputHandler::isKeyPressed("KeyA"))
                totalvelocity += glm::vec3(0.0f, 0.0f, -0.05f);
            if (InputHandler::isKeyPressed("KeyD"))
                totalvelocity += glm::vec3(0.0f, 0.0f, 0.05f);
            Main::physicsEngine->addVelocityClampedRotation(Main::localPlayer->object, totalvelocity, glm::vec3(maxH, 0.1f, maxH));
            if (InputHandler::isKeyPressed("Space") && Main::physicsEngine->isOnFoot(Main::localPlayer->object)) {
                Main::physicsEngine->addVelocityClamped(Main::localPlayer->object, glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3(maxH, 0.1f, maxH));
            }
            // if (Main::physicsEngine->getVelocity(Main::localPlayer->object) != glm::vec3(0.0f, 0.0f, 0.0f)) {
            PlayerAuthInput packet;
            SocketClient::sendPacket(&packet);
            // }
        }
        else {
            if (InputHandler::isKeyPressed("KeyW"))
                ourCamera->Position += glm::vec3(2.0f * deltaTime, 0.0f, 2.0f * deltaTime) * ourCamera->Front;
            if (InputHandler::isKeyPressed("KeyS"))
                ourCamera->Position += glm::vec3(-2.0f * deltaTime, 0.0f, -2.0f * deltaTime) * ourCamera->Front;
            if (InputHandler::isKeyPressed("KeyA"))
                ourCamera->Position += glm::vec3(-2.0f * deltaTime, 0.0f, -2.0f * deltaTime) * ourCamera->Right;
            if (InputHandler::isKeyPressed("KeyD"))
                ourCamera->Position += glm::vec3(2.0f * deltaTime, 0.0f, 2.0f * deltaTime) * ourCamera->Right;
            if (InputHandler::isKeyPressed("Space"))
                ourCamera->Position += glm::vec3(0.0f, 2.0f * deltaTime, 0.0f) * ourCamera->Up;
            if (InputHandler::isKeyPressed("ShiftLeft"))
                ourCamera->Position += glm::vec3(0.0f, -2.0f * deltaTime, 0.0f) * ourCamera->Up;
        }

        if (InputHandler::isMousePressed(MOUSE_LEFT))
            emscripten_request_pointerlock("canvas", EM_TRUE);

        if (InputHandler::isKeyPressed("KeyC") && !freeCamLock) {
            ourCamera->freeCam = !ourCamera->freeCam;
            ourCamera->Yaw = rotation.y;
            freeCamLock = true;
        }
        if (InputHandler::isKeyReleased("KeyC") && freeCamLock) {
            freeCamLock = false;
        }

        if (InputHandler::isMousePressed(MOUSE_LEFT) && checkPointerLock() && !breakLock) {
            glm::vec3 p = Main::localPlayer->object->position + ourCamera->offset;
            glm::vec3 r = glm::vec3(0.0f, Main::localPlayer->object->rotation.y, ourCamera->Pitch);
            RaycastResult obj = Main::physicsEngine->raycast(3.0f, p, r);
            if (obj.hit) {
                // std::shared_ptr<AirObject> air = std::make_shared<AirObject>(obj.object->position, obj.object->rotation);
                // Main::chunks[obj.chunkpos]->addBlock(obj.blockpos, air);
                // Main::chunks[obj.chunkpos]->initTranslations();
                EditChunk packet;
                packet.chunkpos = obj.chunkpos;
                packet.blockpos = obj.blockpos;
                packet.id = 0;
                SocketClient::sendPacket(&packet);
            }
            else { std::cout << "nothing" << std::endl; }

            breakLock = true;
        }
        if (InputHandler::isMouseReleased(MOUSE_LEFT) && breakLock) {
            breakLock = false;
        }

        if (InputHandler::isMousePressed(MOUSE_RIGHT) && checkPointerLock() && !placeLock) {
            glm::vec3 p = Main::localPlayer->object->position + ourCamera->offset;
            glm::vec3 r = glm::vec3(0.0f, Main::localPlayer->object->rotation.y, ourCamera->Pitch);
            RaycastResult obj = Main::physicsEngine->raycast(4.0f, p, r);
            if (obj.hit) {
                if (!Main::physicsEngine->possibleCollision(obj.prevobject->position, glm::vec3(0.5f, 0.5f, 0.5f), Main::localPlayer->object)) {
                    std::shared_ptr<BlockObject> wood = std::make_shared<BlockObject>(obj.prevobject->position, obj.prevobject->rotation, 0, 4, true, glm::vec3(0.5f, 0.5f, 0.5f));
                    // std::cout << obj.prevchunkpos.x << " " << obj.prevchunkpos.y << " " << obj.prevchunkpos.z << std::endl;
                    // std::cout << obj.prevblockpos.x << " " << obj.prevblockpos.y << " " << obj.prevblockpos.z << std::endl;
                    // Main::chunks[obj.prevchunkpos]->addBlock(obj.prevblockpos, wood);
                    // Main::chunks[obj.prevchunkpos]->initTranslations();
                    EditChunk packet;
                    packet.chunkpos = obj.prevchunkpos;
                    packet.blockpos = obj.prevblockpos;
                    packet.id = 4;
                    SocketClient::sendPacket(&packet);
                }
                else { std::cout << "blocked" << std::endl; }
            }
            else { std::cout << "nothing" << std::endl; }

            placeLock = true;
        }
        if (InputHandler::isMouseReleased(MOUSE_RIGHT) && placeLock) {
            placeLock = false;
        }
    }
}

void preRender() {
    glClearColor(0.53f, 0.81f, 0.98f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    Main::ourShader->use();

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(60.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

    glUniformMatrix4fv(Main::ourShader->uniforms["projection"], 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(Main::ourShader->uniforms["view"], 1, GL_FALSE, &ourCamera->GetViewMatrix()[0][0]);
    glUniform1i(Main::ourShader->uniforms["textureSampler"], 0);
}

void render() {
    glm::vec3 playerPos = Main::localPlayer->object->position;
    glm::vec3 playerChunk = glm::vec3(floor(playerPos.x / 8.0f), floor(playerPos.y / 8.0f), floor(playerPos.z / 8.0f));

    std::string coordsstr = "x: " + std::to_string(playerPos.x) + " y: " + std::to_string(playerPos.y) + " z: " + std::to_string(playerPos.z) + " cx: " + std::to_string((int)playerChunk.x) + " cy: " + std::to_string((int)playerChunk.y) + " cz: " + std::to_string((int)playerChunk.z);
    std::shared_ptr<Element> e = Main::guiManager->getElement("coords");
    dynamic_cast<TextElement*>(e.get())->setText(coordsstr, glm::vec3(0.0f, 0.0f, 0.0f));

    if (!ourCamera->freeCam) {
        cameraFrustum = ourCamera->createFrustumFromCamera(glm::radians(60.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
    }

    // std::cout << playerChunk.x << " " << playerChunk.y << " " << playerChunk.z << std::endl;

    for (int i = -renderDistance; i <= renderDistance; i++) {
        for (int j = -renderDistance; j <= renderDistance; j++) {
            for (int k = -renderDistance; k <= renderDistance; k++) {
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

                if (chunk->checkCull(cameraFrustum, requestedChunk)) {
                    chunk->renderChunk();
                }
            }
        }
    }

    Main::localPlayer->object->setposition(Main::localPlayer->object->position);
    Main::localPlayer->object->render();

    for (auto& entity : Main::entities) {
        entity->object->render();
    }

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

void postRender() {
    glDisable(GL_CULL_FACE);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    Main::fontShader->use();

    glm::mat4 projection = glm::ortho(0.0f, (float)windowWidth, (float)windowHeight, 0.0f, -1.0f, 1.0f);

    glUniformMatrix4fv(Main::fontShader->uniforms["projection"], 1, GL_FALSE, &projection[0][0]);
    glUniform1i(Main::fontShader->uniforms["textureSampler"], 1);

    Main::guiManager->render();

    int stateMask = 0x0000;

    if (InputHandler::isMousePressed(MOUSE_LEFT))
        stateMask |= LEFT_CLICK;
    if (InputHandler::isMousePressed(MOUSE_RIGHT))
        stateMask |= RIGHT_CLICK;
    if (InputHandler::isMousePressed(MOUSE_MIDDLE))
        stateMask |= MIDDLE_CLICK;
    if (InputHandler::isKeyPressed("ControlLeft"))
        stateMask |= CTRL_FLAG;
    if (InputHandler::isKeyPressed("ShiftLeft"))
        stateMask |= SHIFT_FLAG;

    Main::guiManager->poll(stateMask);

    std::shared_ptr<Element> e = Main::guiManager->getElement("crosshair");
    e->setPosition(windowWidth/2-10, windowHeight/2-10);
}

void mainLoop() {

    if (Main::serverConnected) {
        preRender();

        render();
    }

    postRender();

    processInput();
    glfwSwapBuffers(window);
    glfwPollEvents();

    float currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    int fps = (int)std::round(1.0f / deltaTime);
    std::shared_ptr<Element> e = Main::guiManager->getElement("fpscounter");
    dynamic_cast<TextElement*>(e.get())->setText("FPS: " + std::to_string(fps), glm::vec3(std::max(1.0f - ((float)fps / 255.0f), 0.0f), 0.0f, 0.0f));
}

int main() {
    bool ticklooprunning = true;

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

    emscripten_set_keydown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, InputHandler::keyPressed);
    emscripten_set_keyup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, InputHandler::keyReleased);
    emscripten_set_mousemove_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, InputHandler::mouseMoved);
    emscripten_set_mousedown_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, InputHandler::mouseButton);
    emscripten_set_mouseup_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, InputHandler::mouseButton);

    PacketHelper::registerPacket(0, []() { return new HandShakePacket(); });
    PacketHelper::registerPacket(1, []() { return new EditChunk(); });
    PacketHelper::registerPacket(2, []() { return new GenerateChunk(); });
    PacketHelper::registerPacket(3, []() { return new EntityAction(); });
    PacketHelper::registerPacket(4, []() { return new PlayerAuthInput(); });

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    L_SUBSCRIBE(MouseMoveEvent, [](MouseMoveEvent* event) {
        int x = event->posx;
        int y = event->posy;
        int dx = event->deltax;
        int dy = event->deltay;

        if (checkPointerLock())
            ourCamera->ProcessMouseMovement(dx, -dy);
    });

    Main::ourShader = new Shader("/assets/shaders/vertex.glsl", "/assets/shaders/fragment.glsl", {{"view", 0}, {"projection", 0}, {"textureSampler", 0}});
    Main::fontShader = new Shader("/assets/shaders/vertfont.glsl", "/assets/shaders/fragfont.glsl", {{"projection", 0}, {"textureSampler", 0}, {"color", 0}});

    Main::fontManager = new FontManager();
    Main::fontManager->init("/assets/fonts/montserrat.ttf");

    Main::vertexManager = new VertexManager();
    Main::vertexManager->initVBO(0, vertices, sizeof(vertices));
    Main::vertexManager->initVBO(1, playervertices, sizeof(playervertices));

    Main::textureManager = new TextureManager();
    Main::textureManager->startInit(64, 64, 64);

    // blocks
    Main::textureManager->addTexture("/assets/textures/stone.png", 1);
    Main::textureManager->addTexture("/assets/textures/dirt.png", 2);
    Main::textureManager->addTexture("/assets/textures/grass.png", 3);
    Main::textureManager->addTexture("/assets/textures/wood.png", 4);

    //entities
    Main::textureManager->addTexture("/assets/textures/player.png", 49);

    Main::textureManager->endInit();

    L_SUBSCRIBE(TickEvent, [](TickEvent* event) {
        if (!Main::serverConnected) { return; }

        Main::physicsEngine->step();
        if (!ourCamera->freeCam) {
            ourCamera->updateCameraPosition();
        }
        else {
            ourCamera->updateCameraVectors();
        }
    });

    Main::guiManager = new GUIManager();
    std::shared_ptr<TextElement> fpscounter = std::make_shared<TextElement>("fpscounter", [](int){}, 0, 20, Main::fontManager);
    fpscounter->setText("FPS: 0", glm::vec3(0.0f, 0.0f, 0.0f));
    Main::guiManager->addElement(fpscounter);
    std::shared_ptr<TextElement> crosshair = std::make_shared<TextElement>("crosshair", [](int){}, 0, 0, Main::fontManager);
    crosshair->setText("+", glm::vec3(1.0f, 1.0f, 1.0f));
    Main::guiManager->addElement(crosshair);
    std::shared_ptr<TextElement> coords = std::make_shared<TextElement>("coords", [](int){}, 0, 40, Main::fontManager);
    coords->setText("x: 0, y: 0, z: 0, cx: 0, cy: 0, cz: 0", glm::vec3(0.0f, 0.0f, 0.0f));
    Main::guiManager->addElement(coords);
    std::shared_ptr<EnterElement> ipenter = std::make_shared<EnterElement>("ipenter", [](int){}, 100, 200, Main::fontManager, 64, "localhost");
    ipenter->color = glm::vec3(1.0f, 1.0f, 1.0f);
    ipenter->enteractive = true;
    Main::guiManager->addElement(ipenter);

    L_SUBSCRIBE(KeyEvent, [](KeyEvent* event) {
        if (event->state && !Main::serverConnected) {
            std::shared_ptr<Element> e = Main::guiManager->getElement("ipenter");

            if (event->key == "Enter") {
                Main::serverAddress = dynamic_cast<EnterElement*>(e.get())->text;
                SocketClient::connect();
                dynamic_cast<EnterElement*>(e.get())->enteractive = false;
                dynamic_cast<EnterElement*>(e.get())->active = false;
            }

            if (event->key == "Backspace") {
                dynamic_cast<EnterElement*>(e.get())->removeChar();
            }

            if (strlen(event->code) > 1) { return; }

            int keycode = (int)*(event->code);
            std::cout << strlen(event->code) << std::endl;
            dynamic_cast<EnterElement*>(e.get())->addChar((char)keycode);
        }
    });

    Main::physicsEngine = std::make_unique<PhysicsEngine>(&Main::chunks);

    // Main::isSingleplayer = true;
    // Main::serverInstance.setCallback(SocketClient::on_message);
    // SocketClient::on_open();
    // SocketClient::connect();

    Main::localPlayer = std::make_shared<Entity>();
    Main::localPlayer->uuid = "local";
    Main::localPlayer->object = std::make_shared<EntityObject>(glm::vec3(0.0, 4.0f, 0.0), glm::vec3(0.0f, 0.0f, 0.0f), 1, 49, true, glm::vec3(0.25f, 0.75f, 0.25f));

    Main::physicsEngine->registerObject(Main::localPlayer->object, 1.0f);

    ourCamera = new Camera(Main::localPlayer);

    std::thread tick([&]() {
        int tps = 60;
        int mps = 1000 / tps;
        long long ltt = duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
        while (ticklooprunning) {
            long long now = duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            long long elapsed = now - ltt;

            if (elapsed >= mps) {
                int ticksToRun = (int) (elapsed / mps);
                ltt += ticksToRun * mps;

                for (int i = 0; i < ticksToRun; i++) {
                    TickEvent event;
                    EventBus::getInstance().publish(&event);
                }
            }

            std::this_thread::sleep_for(std::chrono::milliseconds(1));
         }
    });

    glfwMakeContextCurrent(window);
    glViewport(0, 0, windowWidth, windowHeight);

    emscripten_set_main_loop(&mainLoop, 0, 1);

    ticklooprunning = false;
    tick.join();

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

