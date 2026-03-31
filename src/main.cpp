#include <emscripten.h>
#include <iostream>
#include <emscripten/html5.h>
#include <GLES3/gl3.h>
#include <GLFW/glfw3.h>
#include <thread>
#include <emscripten/heap.h>

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
#include "../lib/json/json.hpp"
#include "Protocol/Packets/ChatMessage.hpp"
#include "Protocol/Packets/LightMap.hpp"

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
float lastCounter = 0.0f;

int selectedblock = 1;
bool sprint = false;

float maxH = 0.05f;

GLuint vertexShader;
GLuint fragmentShader;
GLuint shaderProgram;

bool freeCamLock = false;
bool placeLock = false;
bool breakLock = false;
bool saveLock = false;
bool loadLock = false;
bool raiseLock = false;
bool lowerLock = false;
bool fxaaLock = false;
bool chatLock = false;

int renderDistance = 5; // 3 min
Frustum cameraFrustum;

float ambientLevel = 0.2f;

int fxaaFlag = 0;
float fquad[] = {
    -1.0f,  1.0f,  0.0f, 1.0f,
    -1.0f, -1.0f,  0.0f, 0.0f,
     1.0f, -1.0f,  1.0f, 0.0f,

    -1.0f,  1.0f,  0.0f, 1.0f,
     1.0f, -1.0f,  1.0f, 0.0f,
     1.0f,  1.0f,  1.0f, 1.0f
};
GLuint fbo;
GLuint rbo;
GLuint ftex;
GLuint fvao;
GLuint fvbo;

glm::vec3 colorLerp(glm::vec3 endcolor, glm::vec3 startcolor, float step) {
    return (endcolor - startcolor) * step + startcolor;
}

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

extern "C" {
    int load_world() {
        Main::serverAddress = "localhost";
        Main::serverInstance.loadWorld();
        SocketClient::getInstance().connect();
        std::shared_ptr<Element> e = Main::menuManager->getElement("ipenter");
        static_cast<EnterElement*>(e.get())->enteractive = false;
        Main::menuManager->active = false;
        Main::gameUIManager->active = true;
        Main::chatUIManager->active = true;
        if (Main::isMobile) { Main::touchManager->active = true; }
        return 0;
    }

    int input_char(int charcode) {
        if (!Main::isMobile) { return 0; }
        if (charcode > 127) { return 0; }
        std::shared_ptr<Element> e = Main::menuManager->getElement("ipenter");
        EnterElement* ipenter = static_cast<EnterElement*>(e.get());
        if (charcode == 8) {
            ipenter->removeChar();
            return 0;
        }
        ipenter->addChar((char)charcode);
        return 0;
    }
}

EM_BOOL onResize(int, const EmscriptenUiEvent* e, void*) {
    windowWidth = e->windowInnerWidth;
    windowHeight = e->windowInnerHeight;
    glViewport(0, 0, windowWidth, windowHeight);
    glfwSetWindowSize(window, windowWidth, windowHeight);

    std::shared_ptr<Element> gl = Main::menuManager->getElement("gamelabel");
    gl->setPosition((windowWidth/2)-80, gl->y);
    std::shared_ptr<Element> vl = Main::menuManager->getElement("verlabel");
    vl->setPosition((windowWidth/2)-60, vl->y);

    std::shared_ptr<Element> cfs = Main::touchManager->getElement("callfullscreen");
    cfs->setPosition((windowWidth/18)*17, windowHeight/12);

    std::shared_ptr<Element> lmb = Main::touchManager->getElement("leftclick");
    lmb->setPosition((windowWidth/18)*2, windowHeight/6);
    std::shared_ptr<Element> rmb = Main::touchManager->getElement("rightclick");
    rmb->setPosition((windowWidth/18)*4, windowHeight/6);

    std::shared_ptr<Element> spc = Main::touchManager->getElement("movejump");
    spc->setPosition((windowWidth/18)*15, (windowHeight/18)*15);

    std::shared_ptr<Element> mvf = Main::touchManager->getElement("moveforward");
    mvf->setPosition((windowWidth/18)*3, (windowHeight/18)*12);
    std::shared_ptr<Element> mvb = Main::touchManager->getElement("movebackward");
    mvb->setPosition((windowWidth/18)*3, (windowHeight/18)*16);
    std::shared_ptr<Element> mvl = Main::touchManager->getElement("moveleft");
    mvl->setPosition((windowWidth/18)*2, (windowHeight/18)*14);
    std::shared_ptr<Element> mvr = Main::touchManager->getElement("moveright");
    mvr->setPosition((windowWidth/18)*4, (windowHeight/18)*14);

    // std::shared_ptr<Element> lmb = Main::menuManager->getElement("leftclick");
    // static_cast<TextElement*>(vl.get())->setPosition((windowWidth/2)-60, static_cast<TextElement*>(vl.get())->y);

    return EM_TRUE;
}

void processInput()
{
    if (Main::serverConnected) {
        if (Main::chatUIManager->getElement("chatenter")->active) { return; }

        glm::vec3 rotation = Main::localPlayer->object->rotation;
        if (!ourCamera->freeCam) {
            bool moved = false;
            float speed = (sprint ? 0.075f : 0.05f);
            glm::vec3 totalvelocity = glm::vec3(0.0f, 0.0f, 0.0f);
            if (InputHandler::isKeyPressed("KeyW")) {
                totalvelocity += glm::vec3(speed, 0.0f, 0.0f);
                moved = true;
            }
            if (InputHandler::isKeyPressed("KeyS")) {
                totalvelocity += glm::vec3(-speed, 0.0f, 0.0f);
                moved = true;
            }
            if (InputHandler::isKeyPressed("KeyA")) {
                totalvelocity += glm::vec3(0.0f, 0.0f, -speed);
                moved = true;
            }
            if (InputHandler::isKeyPressed("KeyD")) {
                totalvelocity += glm::vec3(0.0f, 0.0f, speed);
                moved = true;
            }
            Main::physicsEngine->addVelocityClampedRotation(Main::localPlayer->object, totalvelocity, glm::vec3((sprint ? maxH*1.5 : maxH), 0.1f, (sprint ? maxH*1.5 : maxH)));
            if (InputHandler::isKeyPressed("Space") && Main::physicsEngine->isOnFoot(Main::localPlayer->object)) {
                Main::physicsEngine->addVelocityClamped(Main::localPlayer->object, glm::vec3(0.0f, 0.1f, 0.0f), glm::vec3((sprint ? maxH*1.5 : maxH), 0.1f, (sprint ? maxH*1.5 : maxH)));
                moved = true;
            }
            if (moved) {
                PlayerAuthInput packet;
                SocketClient::getInstance().sendPacket(&packet);
            }
        }
        else {
            if (InputHandler::isKeyPressed("KeyW"))
                ourCamera->Position += glm::vec3(5.0f * deltaTime, 0.0f, 5.0f * deltaTime) * ourCamera->Front;
            if (InputHandler::isKeyPressed("KeyS"))
                ourCamera->Position += glm::vec3(-5.0f * deltaTime, 0.0f, -5.0f * deltaTime) * ourCamera->Front;
            if (InputHandler::isKeyPressed("KeyA"))
                ourCamera->Position += glm::vec3(-5.0f * deltaTime, 0.0f, -5.0f * deltaTime) * ourCamera->Right;
            if (InputHandler::isKeyPressed("KeyD"))
                ourCamera->Position += glm::vec3(5.0f * deltaTime, 0.0f, 5.0f * deltaTime) * ourCamera->Right;
            if (InputHandler::isKeyPressed("Space"))
                ourCamera->Position += glm::vec3(0.0f, 5.0f * deltaTime, 0.0f) * ourCamera->Up;
            if (InputHandler::isKeyPressed("ShiftLeft"))
                ourCamera->Position += glm::vec3(0.0f, -5.0f * deltaTime, 0.0f) * ourCamera->Up;
        }

        if (InputHandler::isMousePressed(MOUSE_LEFT) && Main::serverConnected) {
            emscripten_request_pointerlock("canvas", EM_TRUE);
        }

        if (InputHandler::isKeyPressed("KeyC") && !freeCamLock) {
            ourCamera->freeCam = !ourCamera->freeCam;
            ourCamera->Yaw = rotation.y;
            freeCamLock = true;
        }
        if (InputHandler::isKeyReleased("KeyC") && freeCamLock) {
            freeCamLock = false;
        }

        if (InputHandler::isKeyPressed("KeyY") && !raiseLock) {
            ambientLevel = std::min(ambientLevel + 0.2f, 1.0f);
            raiseLock = true;
        }
        if (InputHandler::isKeyReleased("KeyY") && raiseLock) {
            raiseLock = false;
        }

        if (InputHandler::isKeyPressed("KeyH") && !lowerLock) {
            ambientLevel = std::max(ambientLevel - 0.2f, 0.0f);
            lowerLock = true;
        }
        if (InputHandler::isKeyReleased("KeyH") && lowerLock) {
            lowerLock = false;
        }

        if (((InputHandler::isMousePressed(MOUSE_LEFT) && checkPointerLock() && !Main::isMobile) || (InputHandler::isKeyPressed("LeftMouse") && Main::isMobile)) && !breakLock) {
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
                SocketClient::getInstance().sendPacket(&packet);
            }
            else { std::cout << "nothing" << std::endl; }

            breakLock = true;
        }
        if (((InputHandler::isMouseReleased(MOUSE_LEFT) && !Main::isMobile) || (InputHandler::isKeyReleased("LeftMouse") && Main::isMobile)) && breakLock) {
            breakLock = false;
        }

        if (((InputHandler::isMousePressed(MOUSE_RIGHT) && checkPointerLock() && !Main::isMobile) || (InputHandler::isKeyPressed("RightMouse") && Main::isMobile)) && !placeLock) {
            glm::vec3 p = Main::localPlayer->object->position + ourCamera->offset;
            glm::vec3 r = glm::vec3(0.0f, Main::localPlayer->object->rotation.y, ourCamera->Pitch);
            RaycastResult obj = Main::physicsEngine->raycast(4.0f, p, r);
            if (obj.hit) {
                if (!Main::physicsEngine->possibleCollision(obj.prevobject->position, glm::vec3(0.5f, 0.5f, 0.5f), Main::localPlayer->object)) {
                    std::shared_ptr<LightObject> wood = std::make_shared<LightObject>(obj.prevobject->position, obj.prevobject->rotation, 0, 5, true, glm::vec3(0.5f, 0.5f, 0.5f), 10);
                    // std::cout << obj.prevchunkpos.x << " " << obj.prevchunkpos.y << " " << obj.prevchunkpos.z << std::endl;
                    // std::cout << obj.prevblockpos.x << " " << obj.prevblockpos.y << " " << obj.prevblockpos.z << std::endl;
                    // Main::chunks[obj.prevchunkpos]->addBlock(obj.prevblockpos, wood);
                    // Main::chunks[obj.prevchunkpos]->initTranslations();
                    EditChunk packet;
                    packet.chunkpos = obj.prevchunkpos;
                    packet.blockpos = obj.prevblockpos;
                    packet.id = selectedblock;
                    SocketClient::getInstance().sendPacket(&packet);
                }
                else { std::cout << "blocked" << std::endl; }
            }
            else { std::cout << "nothing" << std::endl; }

            placeLock = true;
        }
        if (((InputHandler::isMouseReleased(MOUSE_RIGHT) && !Main::isMobile) || (InputHandler::isKeyReleased("RightMouse") && Main::isMobile)) && placeLock) {
            placeLock = false;
        }

        if (InputHandler::isKeyPressed("KeyP") && !saveLock && Main::isSingleplayer) {
            Main::serverInstance.saveWorld();
            emscripten_run_script("saveFileFromMemoryFSToDisk('/world.mww','world.mww')");
            saveLock = true;
        }
        if (InputHandler::isKeyReleased("KeyP") && saveLock) {
            saveLock = false;
        }

        if (InputHandler::isKeyPressed("ShiftLeft")) {
            sprint = true;
        }
        if (InputHandler::isKeyReleased("ShiftLeft")) {
            sprint = false;
        }

        if (InputHandler::isKeyPressed("Digit1")) {
            selectedblock = 1;
        }
        if (InputHandler::isKeyPressed("Digit2")) {
            selectedblock = 2;
        }
        if (InputHandler::isKeyPressed("Digit3")) {
            selectedblock = 3;
        }
        if (InputHandler::isKeyPressed("Digit4")) {
            selectedblock = 4;
        }
        if (InputHandler::isKeyPressed("Digit5")) {
            selectedblock = 5;
        }

        if (InputHandler::isKeyPressed("KeyM")) {
            assert(true == false);
        }

        if (InputHandler::isKeyPressed("KeyL") && !fxaaLock) {
            fxaaFlag = !fxaaFlag;
            printf("FXAA: %d\n", fxaaFlag);
            fxaaLock = true;
        }
        if (InputHandler::isKeyReleased("KeyL") && fxaaLock) {
            fxaaLock = false;
        }

        if (InputHandler::isKeyPressed("KeyT") && !chatLock) {
            Main::chatUIManager->getElement("chatenter")->active = true;
            chatLock = true;
        }
        if (InputHandler::isKeyReleased("KeyT") && chatLock) {
            chatLock = false;
        }
    }
}

void preRender() {
    // 0.005f, 0.05f, 0.12f night
    // 0.53f, 0.81f, 0.98f day
    glm::vec3 ambientColor = colorLerp(glm::vec3(0.53f, 0.81f, 0.98f), glm::vec3(0.005f, 0.05f, 0.12f), ambientLevel);
    glClearColor(ambientColor.r, ambientColor.g, ambientColor.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glDisable(GL_BLEND);

    glm::mat4 projection;
    projection = glm::perspective(glm::radians(60.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);

    Main::entityShader->use();
    glUniformMatrix4fv(Main::entityShader->uniforms["projection"], 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(Main::entityShader->uniforms["view"], 1, GL_FALSE, &ourCamera->GetViewMatrix()[0][0]);
    glUniform1i(Main::entityShader->uniforms["textureSampler"], 0);

    Main::localPlayer->object->setposition(Main::localPlayer->object->position);
    Main::localPlayer->object->render();
    for (auto& entity : Main::entities) {
        entity->object->setposition(entity->object->position);
        entity->object->render();
    }

    Main::ourShader->use();
    glUniformMatrix4fv(Main::ourShader->uniforms["projection"], 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(Main::ourShader->uniforms["view"], 1, GL_FALSE, &ourCamera->GetViewMatrix()[0][0]);
    glUniform1f(Main::ourShader->uniforms["ambientLevel"], ambientLevel);
    glUniform1i(Main::ourShader->uniforms["textureSampler"], 0);
    glUniform1i(Main::ourShader->uniforms["chunkSampler"], 2);
}

void render() {
    glm::vec3 playerPos = Main::localPlayer->object->position;
    glm::vec3 playerChunk = glm::vec3(floor(playerPos.x / 8.0f), floor(playerPos.y / 8.0f), floor(playerPos.z / 8.0f));

    std::string coordsstr = "x: " + std::to_string(playerPos.x) + " y: " + std::to_string(playerPos.y) + " z: " + std::to_string(playerPos.z) + " cx: " + std::to_string((int)playerChunk.x) + " cy: " + std::to_string((int)playerChunk.y) + " cz: " + std::to_string((int)playerChunk.z) + " blk: " + std::to_string(selectedblock) + " mem: " + std::to_string(emscripten_get_heap_size());
    std::shared_ptr<Element> e = Main::gameUIManager->getElement("coords");
    static_cast<TextElement*>(e.get())->setText(coordsstr);

    if (!ourCamera->freeCam) {
        cameraFrustum = ourCamera->createFrustumFromCamera(glm::radians(60.0f), (float)windowWidth / (float)windowHeight, 0.1f, 100.0f);
    }

    // std::cout << playerChunk.x << " " << playerChunk.y << " " << playerChunk.z << std::endl;

    std::set<glm::vec3, vec3Comparator> currentChunks;

    for (int i = -renderDistance; i <= renderDistance; i++) {
        for (int j = -renderDistance; j <= renderDistance; j++) {
            for (int k = -renderDistance; k <= renderDistance; k++) {
                if (abs(i)+abs(j)+abs(k) > renderDistance) { continue; }

                glm::vec3 requestedChunk = glm::vec3(playerChunk.x + (float)i, playerChunk.y + (float)j, playerChunk.z + (float)k);
                currentChunks.insert(requestedChunk);

                if (Main::chunks.find(requestedChunk) == Main::chunks.end()) {
                    if (!Main::requestedChunks.contains(requestedChunk)) {
                        Main::requestedChunks.insert(requestedChunk);
                        GenerateChunk pkt;
                        pkt.chunkpos = requestedChunk;
                        SocketClient::getInstance().sendPacket(&pkt);
                    }
                    continue;
                }

                // if (Main::map[requestedChunk]->blocks.size() > 256) {
                //     std::cout << requestedChunk.x << " " << requestedChunk.y << std::endl;
                // }

                std::shared_ptr<ChunkMap> chunk = Main::chunks[requestedChunk];

                // if (requestedChunk == glm::vec3(0.0f, 0.0f, 0.0f)) {
                //     chunk->renderChunk();
                // }

                if (chunk->checkCull(cameraFrustum, requestedChunk)) {
                    chunk->renderChunk();
                }
            }
        }
    }

    std::set<glm::vec3, vec3Comparator> redundantChunks;

    for (auto& chunk : Main::chunks) {
        if (!currentChunks.contains(chunk.first)) {
            redundantChunks.insert(chunk.first);
        }
    }

    for (auto& chunk : redundantChunks) {
        Main::chunks.erase(chunk);
        Main::requestedChunks.erase(chunk);
    }

    // std::cout << Main::chunks.size() << " Tchunksize" << std::endl;
    // std::cout << currentChunks.size() << " Cchunksize" << std::endl;
    // std::cout << Main::requestedChunks.size() << " Rchunksize" << std::endl;

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

    Main::menuManager->render();
    Main::gameUIManager->render();
    Main::touchManager->render();
    Main::chatUIManager->render();

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

    Main::menuManager->poll(Main::mouseX, Main::mouseY, stateMask);
    Main::gameUIManager->poll(Main::mouseX, Main::mouseY, stateMask);
    Main::touchManager->poll(Main::mouseX, Main::mouseY, stateMask);
    Main::chatUIManager->poll(Main::mouseX, Main::mouseY, stateMask);

    std::shared_ptr<Element> e = Main::gameUIManager->getElement("crosshair");
    e->setPosition(windowWidth/2-10, windowHeight/2-10);
}

void loadAssets() {
    nlohmann::json assets;
    std::ifstream manifestfile("/assets/manifest.json");

    if (!manifestfile) {
        std::cout << "no assets manifest found" << std::endl;
    } else manifestfile >> assets;

    for (auto& element : assets["blocks"].items()) {
        nlohmann::json block = element.value();
        int id = block["id"];
        std::string texture = block["texture"];
        Main::textureManager->addTexture(texture, id);
    }

    for (auto& element : assets["entities"].items()) {
        nlohmann::json entity = element.value();
        int id = entity["id"];
        std::string texture = entity["texture"];
        Main::textureManager->addTexture(texture, id);
    }
}

void mainLoop() {
    if (Main::serverConnected) {
        glBindFramebuffer(GL_FRAMEBUFFER, fbo);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, ftex);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, windowWidth, windowHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, ftex, 0);
        glBindRenderbuffer(GL_RENDERBUFFER, rbo);
        glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, windowWidth, windowHeight);
        glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, rbo);
        if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
            std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

        preRender();

        render();

        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        Main::fxaaShader->use();
        glUniform1i(Main::fxaaShader->uniforms["uEnabled"], fxaaFlag);
        glUniform1i(Main::fxaaShader->uniforms["uFrameTex"], 3);
        glUniform2f(Main::fxaaShader->uniforms["uViewportSize"], (float)windowWidth, (float)windowHeight);

        glBindVertexArray(fvao);
        glActiveTexture(GL_TEXTURE3);
        glBindTexture(GL_TEXTURE_2D, ftex);
        glBindBuffer(GL_ARRAY_BUFFER, fvbo);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
        glEnableVertexAttribArray(1);
        glDrawArrays(GL_TRIANGLES, 0, 6);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
    }

    if (!Main::serverConnected) {
        glClearColor(0.25f, 0.25f, 0.25f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    postRender();

    processInput();
    glfwSwapBuffers(window);
    glfwPollEvents();

    float currentFrame = (float)glfwGetTime();
    deltaTime = currentFrame - lastFrame;
    lastFrame = currentFrame;

    if ((float)glfwGetTime() > lastCounter + 1.0f) {
        int fps = (int)std::round(1.0f / deltaTime);
        std::shared_ptr<Element> e = Main::gameUIManager->getElement("fpscounter");
        static_cast<TextElement*>(e.get())->setText("FPS: " + std::to_string(fps));
        static_cast<TextElement*>(e.get())->color = glm::vec3(std::max(1.0f - ((float)fps / 255.0f), 0.0f), 0.0f, 0.0f);
        lastCounter = (float)glfwGetTime();
    }

    if (Main::isSingleplayer) {
        // std::cout << SocketClient::getInstance().clientPacketQueue.size() << " queue" << std::endl;
        if (!SocketClient::getInstance().clientPacketQueue.empty()) {
            // std::lock_guard<std::mutex> guard(SocketClient::getInstance().clientPacketQueueMutex);
            if (SocketClient::getInstance().clientPacketQueueMutex.try_lock()) {
                std::pair<ClientSession*, std::vector<uint8_t>> packet = SocketClient::getInstance().clientPacketQueue.front();
                PacketHelper::decodePacket(packet.second);
                SocketClient::getInstance().clientPacketQueue.pop_front();
                SocketClient::getInstance().clientPacketQueueMutex.unlock();
            }
        }
    }
}

int main() {
    bool ticklooprunning = true;

    emscripten_get_canvas_element_size("#canvas", &windowWidth, &windowHeight);
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, onResize);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_SAMPLES, 0);
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
    emscripten_set_touchmove_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, InputHandler::touchMoved);
    emscripten_set_touchstart_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, InputHandler::touchStart);
    emscripten_set_touchend_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, InputHandler::touchEnd);


    PacketHelper::registerPacket(0, []() { return new HandShakePacket(); });
    PacketHelper::registerPacket(1, []() { return new EditChunk(); });
    PacketHelper::registerPacket(2, []() { return new GenerateChunk(); });
    PacketHelper::registerPacket(3, []() { return new EntityAction(); });
    PacketHelper::registerPacket(4, []() { return new PlayerAuthInput(); });
    PacketHelper::registerPacket(5, []() { return new LightMap(); });
    PacketHelper::registerPacket(6, []() { return new ChatMessage(); });

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    L_SUBSCRIBE(MouseMoveEvent, [](MouseMoveEvent* event) {
        int x = event->posx;
        int y = event->posy;
        int dx = event->deltax;
        int dy = event->deltay;

        Main::mouseX = x;
        Main::mouseY = y;

        if (checkPointerLock() || Main::isMobile)
            ourCamera->ProcessMouseMovement(dx, -dy);
    });

    Main::ourShader = new Shader("/assets/shaders/vertex.glsl", "/assets/shaders/fragment.glsl", {{"view", 0}, {"projection", 0}, {"textureSampler", 0}, {"ambientLevel", 0}, {"chunkSampler", 0}});
    Main::fontShader = new Shader("/assets/shaders/vertfont.glsl", "/assets/shaders/fragfont.glsl", {{"projection", 0}, {"textureSampler", 0}, {"color", 0}, {"background", 0}, {"texindex", 0}});
    Main::entityShader = new Shader("/assets/shaders/vertentity.glsl", "/assets/shaders/fragentity.glsl", {{"view", 0}, {"projection", 0}, {"textureSampler", 0}});
    Main::fxaaShader = new Shader("/assets/shaders/vertFXAA.glsl", "/assets/shaders/fragFXAA.glsl", {{"uEnabled", 0}, {"uFrameTex", 0}, {"uViewportSize", 0}});

    std::vector<int> fontsizes = {20, 40};
    Main::fontManager = new FontManager();
    Main::fontManager->init("/assets/fonts/montserrat.ttf", fontsizes);

    Main::vertexManager = new VertexManager();
    Main::vertexManager->initVBO(0, vertices, sizeof(vertices));
    Main::vertexManager->initVBO(1, playervertices, sizeof(playervertices));

    Main::textureManager = new TextureManager();
    Main::textureManager->startInit(64, 64, 64);

    loadAssets();

    // // blocks
    // Main::textureManager->addTexture("/assets/textures/stone.png", 1);
    // Main::textureManager->addTexture("/assets/textures/dirt.png", 2);
    // Main::textureManager->addTexture("/assets/textures/grass.png", 3);
    // Main::textureManager->addTexture("/assets/textures/wood.png", 4);
    //
    // //entities
    // Main::textureManager->addTexture("/assets/textures/player.png", 49);

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

    {
        Main::chatUIManager = new GUIManager();
        Main::chatUIManager->active = false;
        std::shared_ptr<EnterElement> chatenter = std::make_shared<EnterElement>("chatenter", [](int, int, int){}, 20, 500, 20, Main::fontManager, 128, "", true);
        chatenter->color = glm::vec3(1.0f, 1.0f, 1.0f);
        chatenter->bcolor = glm::vec3(0.5f, 0.5f, 0.5f);
        chatenter->active = false;
        chatenter->enteractive = true;
        Main::chatUIManager->addElement(chatenter);
        for (int i = 0; i < 5; i++){
            std::shared_ptr<TextElement> chatrow = std::make_shared<TextElement>("chatrow" + std::to_string(i), [](int, int, int){}, 20, 450 - (i * 25), 20, Main::fontManager, false);
            chatrow->color = glm::vec3(1.0f, 1.0f, 1.0f);
            chatrow->setText("");
            Main::chatUIManager->addElement(chatrow);
        }
    }

    {
        Main::gameUIManager = new GUIManager();
        Main::gameUIManager->active = false;
        std::shared_ptr<TextElement> fpscounter = std::make_shared<TextElement>("fpscounter", [](int, int, int){}, 0, 20, 20, Main::fontManager, false);
        fpscounter->color = glm::vec3(0.0f, 0.0f, 0.0f);
        fpscounter->setText("FPS: 0");
        Main::gameUIManager->addElement(fpscounter);
        std::shared_ptr<TextElement> crosshair = std::make_shared<TextElement>("crosshair", [](int, int, int){}, 0, 0, 20, Main::fontManager, false);
        crosshair->color = glm::vec3(1.0f, 1.0f, 1.0f);
        crosshair->setText("+");
        Main::gameUIManager->addElement(crosshair);
        std::shared_ptr<TextElement> coords = std::make_shared<TextElement>("coords", [](int, int, int){}, 0, 40, 20, Main::fontManager, false);
        coords->color = glm::vec3(0.0f, 0.0f, 0.0f);
        coords->setText("x: 0, y: 0, z: 0, cx: 0, cy: 0, cz: 0, blk: 1, mem: 0");
        Main::gameUIManager->addElement(coords);
    }

    {
        Main::menuManager = new GUIManager();
        std::shared_ptr<TextElement> gamelabel = std::make_shared<TextElement>("gamelabel", [](int, int, int){}, (windowWidth/2)-80, 40, 40, Main::fontManager, false);
        gamelabel->color = glm::vec3(0.25f, 0.75f, 0.25f);
        gamelabel->setText("MineWeb");
        Main::menuManager->addElement(gamelabel);
        std::shared_ptr<TextElement> verlabel = std::make_shared<TextElement>("verlabel", [](int, int, int){}, (windowWidth/2)-60, 80, 20, Main::fontManager, false);
        verlabel->color = glm::vec3(0.5f, 0.5f, 0.5f);
        verlabel->setText("Version: dev");
        Main::menuManager->addElement(verlabel);
        std::shared_ptr<TextElement> iplabel = std::make_shared<TextElement>("iplabel", [](int, int, int){}, 100, 160, 20, Main::fontManager, false);
        iplabel->color = glm::vec3(0.1f, 0.1f, 0.1f);
        iplabel->setText("Server IP:");
        Main::menuManager->addElement(iplabel);
        std::shared_ptr<EnterElement> ipenter = std::make_shared<EnterElement>("ipenter", nullptr, 100, 200, 20, Main::fontManager, 64, "", true);
        ipenter->callback = [&, ipenter](int x, int y, int stateMask) {
            if (stateMask != (LEFT_CLICK)) { return; }
            if (ipenter == nullptr) { printf("nullptr\b"); }
            if (x > (ipenter->x - 5) && x < (ipenter->x + 10 * ipenter->maxlen) && y > (ipenter->y - 25) && y < (ipenter->y + 5)) {
                ipenter->enteractive = true;
                if (Main::isMobile) { emscripten_run_script("callKeyboard(false)"); }
            }
            else {
                ipenter->enteractive = false;
                if (Main::isMobile) { emscripten_run_script("callKeyboard(true)"); }
            }
        };
        ipenter->color = glm::vec3(1.0f, 1.0f, 1.0f);
        ipenter->bcolor = glm::vec3(0.5f, 0.5f, 0.5f);
        Main::menuManager->addElement(ipenter);
        std::shared_ptr<TextElement> joinbutton = std::make_shared<TextElement>("joinbutton", nullptr, 100, 240, 20, Main::fontManager, true);
        joinbutton->callback = [&, joinbutton, ipenter](int x, int y, int stateMask) {
            if (stateMask != (LEFT_CLICK)) { return; }
            if (x > (joinbutton->x - 5) && x < (joinbutton->x - 5 + 10 * joinbutton->text.length()) && y > (joinbutton->y - 25) && y < (joinbutton->y + 5)) {
                Main::serverAddress = ipenter->text;
                SocketClient::getInstance().connect();
                ipenter->enteractive = false;
                Main::menuManager->active = false;
                Main::gameUIManager->active = true;
                Main::chatUIManager->active = true;
                if (Main::isMobile) { Main::touchManager->active = true; }
            }
        };
        joinbutton->color = glm::vec3(1.0f, 1.0f, 1.0f);
        joinbutton->bcolor = glm::vec3(1.0f, 0.5f, 0.5f);
        joinbutton->setText("Join Server");
        Main::menuManager->addElement(joinbutton);
        std::shared_ptr<TextElement> localbutton = std::make_shared<TextElement>("localbutton", nullptr, 100, 320, 20, Main::fontManager, true);
        localbutton->callback = [&, localbutton, ipenter](int x, int y, int stateMask) {
            if (stateMask != (LEFT_CLICK)) { return; }
            if (x > (localbutton->x - 5) && x < (localbutton->x - 5 + 10 * localbutton->text.length()) && y > (localbutton->y - 25) && y < (localbutton->y + 5)) {
                Main::serverAddress = "localhost";
                SocketClient::getInstance().connect();
                ipenter->enteractive = false;
                Main::menuManager->active = false;
                Main::gameUIManager->active = true;
                Main::chatUIManager->active = true;
                if (Main::isMobile) { Main::touchManager->active = true; }
            }
        };
        localbutton->color = glm::vec3(1.0f, 1.0f, 1.0f);
        localbutton->bcolor = glm::vec3(0.5f, 1.0f, 0.5f);
        localbutton->setText("Play Offline");
        Main::menuManager->addElement(localbutton);
        std::shared_ptr<TextElement> importbutton = std::make_shared<TextElement>("importbutton", nullptr, 100, 360, 20, Main::fontManager, true);
        importbutton->callback = [&, importbutton](int x, int y, int stateMask) {
            if (stateMask != (LEFT_CLICK)) { loadLock = false; return; }
            if (x > (importbutton->x - 5) && x < (importbutton->x - 5 + 10 * importbutton->text.length()) && y > (importbutton->y - 25) && y < (importbutton->y + 5) && !loadLock) {
                emscripten_run_script("uploadSave()");
                loadLock = true;
            }
        };
        importbutton->color = glm::vec3(1.0f, 1.0f, 1.0f);
        importbutton->bcolor = glm::vec3(0.5f, 0.5f, 1.0f);
        importbutton->setText("Load Saved World (Will Start Offline Mode)");
        Main::menuManager->addElement(importbutton);
    }

    {
        Main::touchManager = new GUIManager();
        std::shared_ptr<TextElement> callfullscreen = std::make_shared<TextElement>("callfullscreen", nullptr, 600, 20, 20, Main::fontManager, true);
        callfullscreen->callback = [&, callfullscreen](int x, int y, int stateMask) {
            if (stateMask != (LEFT_CLICK)) { return; }
            if (x > (callfullscreen->x - 5) && x < (callfullscreen->x - 5 + 10 * callfullscreen->text.length()) && y > (callfullscreen->y - 25) && y < (callfullscreen->y + 5)) {
                emscripten_request_fullscreen("canvas", EM_TRUE);
            }
        };
        callfullscreen->color = glm::vec3(1.0f, 1.0f, 1.0f);
        callfullscreen->bcolor = glm::vec3(1.0f, 1.0f, 0.5f);
        callfullscreen->setText("FLSCRN");
        Main::touchManager->addElement(callfullscreen);
        std::shared_ptr<TextElement> moveforward = std::make_shared<TextElement>("moveforward", nullptr, 150, 250, 20, Main::fontManager, true);
        moveforward->callback = [&, moveforward](int x, int y, int stateMask) {
            if (stateMask != (LEFT_CLICK)) { InputHandler::removeKey("KeyW"); return; }
            if (x > (moveforward->x - 5) && x < (moveforward->x - 5 + 10 * moveforward->text.length()) && y > (moveforward->y - 25) && y < (moveforward->y + 5)) {
                InputHandler::addKey("KeyW");
            }
            else {
                InputHandler::removeKey("KeyW");
            }
        };
        moveforward->color = glm::vec3(1.0f, 1.0f, 1.0f);
        moveforward->bcolor = glm::vec3(0.5f, 0.5f, 0.5f);
        moveforward->setText("/\\");
        Main::touchManager->addElement(moveforward);
        std::shared_ptr<TextElement> movebackward = std::make_shared<TextElement>("movebackward", nullptr, 150, 350, 20, Main::fontManager, true);
        movebackward->callback = [&, movebackward](int x, int y, int stateMask) {
            if (stateMask != (LEFT_CLICK)) { InputHandler::removeKey("KeyS"); return; }
            if (x > (movebackward->x - 5) && x < (movebackward->x - 5 + 10 * movebackward->text.length()) && y > (movebackward->y - 25) && y < (movebackward->y + 5)) {
                InputHandler::addKey("KeyS");
            }
            else {
                InputHandler::removeKey("KeyS");
            }
        };
        movebackward->color = glm::vec3(1.0f, 1.0f, 1.0f);
        movebackward->bcolor = glm::vec3(0.5f, 0.5f, 0.5f);
        movebackward->setText("\\/");
        Main::touchManager->addElement(movebackward);
        std::shared_ptr<TextElement> moveright = std::make_shared<TextElement>("moveright", nullptr, 200, 300, 20, Main::fontManager, true);
        moveright->callback = [&, moveright](int x, int y, int stateMask) {
            if (stateMask != (LEFT_CLICK)) { InputHandler::removeKey("KeyD"); return; }
            if (x > (moveright->x - 5) && x < (moveright->x - 5 + 10 * moveright->text.length()) && y > (moveright->y - 25) && y < (moveright->y + 5)) {
                InputHandler::addKey("KeyD");
            }
            else {
                InputHandler::removeKey("KeyD");
            }
        };
        moveright->color = glm::vec3(1.0f, 1.0f, 1.0f);
        moveright->bcolor = glm::vec3(0.5f, 0.5f, 0.5f);
        moveright->setText(">>");
        Main::touchManager->addElement(moveright);
        std::shared_ptr<TextElement> moveleft = std::make_shared<TextElement>("moveleft", nullptr, 100, 300, 20, Main::fontManager, true);
        moveleft->callback = [&, moveleft](int x, int y, int stateMask) {
            if (stateMask != (LEFT_CLICK)) { InputHandler::removeKey("KeyA"); return; }
            if (x > (moveleft->x - 5) && x < (moveleft->x - 5 + 10 * moveleft->text.length()) && y > (moveleft->y - 25) && y < (moveleft->y + 5)) {
                InputHandler::addKey("KeyA");
            }
            else {
                InputHandler::removeKey("KeyA");
            }
        };
        moveleft->color = glm::vec3(1.0f, 1.0f, 1.0f);
        moveleft->bcolor = glm::vec3(0.5f, 0.5f, 0.5f);
        moveleft->setText("<<");
        Main::touchManager->addElement(moveleft);
        std::shared_ptr<TextElement> movejump = std::make_shared<TextElement>("movejump", nullptr, 650, 300, 20, Main::fontManager, true);
        movejump->callback = [&, movejump](int x, int y, int stateMask) {
            if (stateMask != (LEFT_CLICK)) { InputHandler::removeKey("Space"); return; }
            if (x > (movejump->x - 5) && x < (movejump->x - 5 + 10 * movejump->text.length()) && y > (movejump->y - 25) && y < (movejump->y + 5)) {
                InputHandler::addKey("Space");
            }
            else {
                InputHandler::removeKey("Space");
            }
        };
        movejump->color = glm::vec3(1.0f, 1.0f, 1.0f);
        movejump->bcolor = glm::vec3(0.5f, 0.5f, 0.5f);
        movejump->setText("^");
        Main::touchManager->addElement(movejump);
        std::shared_ptr<TextElement> leftclick = std::make_shared<TextElement>("leftclick", nullptr, 75, 50, 20, Main::fontManager, true);
        leftclick->callback = [&, leftclick](int x, int y, int stateMask) {
            if (stateMask != (LEFT_CLICK)) { InputHandler::removeKey("LeftMouse"); return; }
            if (x > (leftclick->x - 5) && x < (leftclick->x - 5 + 10 * leftclick->text.length()) && y > (leftclick->y - 25) && y < (leftclick->y + 5)) {
                InputHandler::addKey("LeftMouse");
            }
            else {
                InputHandler::removeKey("LeftMouse");
            }
        };
        leftclick->color = glm::vec3(1.0f, 1.0f, 1.0f);
        leftclick->bcolor = glm::vec3(0.5f, 0.5f, 1.0f);
        leftclick->setText("LMB");
        Main::touchManager->addElement(leftclick);
        std::shared_ptr<TextElement> rightclick = std::make_shared<TextElement>("rightclick", nullptr, 150, 50, 20, Main::fontManager, true);
        rightclick->callback = [&, rightclick](int x, int y, int stateMask) {
            if (stateMask != (LEFT_CLICK)) { InputHandler::removeKey("RightMouse"); return; }
            if (x > (rightclick->x - 5) && x < (rightclick->x - 5 + 10 * rightclick->text.length()) && y > (rightclick->y - 25) && y < (rightclick->y + 5)) {
                InputHandler::addKey("RightMouse");
            }
            else {
                InputHandler::removeKey("RightMouse");
            }
        };
        rightclick->color = glm::vec3(1.0f, 1.0f, 1.0f);
        rightclick->bcolor = glm::vec3(1.0f, 0.5f, 0.5f);
        rightclick->setText("RMB");
        Main::touchManager->addElement(rightclick);
        Main::touchManager->active = false;
    }

    L_SUBSCRIBE(KeyEvent, [](KeyEvent* event) {
        if (event->state && !Main::isMobile) {
            EnterElement* ipenter = static_cast<EnterElement*>(Main::menuManager->getElement("ipenter").get());

            if (ipenter->enteractive) {
                if (event->key == "Backspace") {
                    ipenter->removeChar();
                    return;
                }

                std::cout << strlen(event->code) << std::endl;
                if (strlen(event->code) > 1) { return; }

                int keycode = (int)*(event->code);
                ipenter->addChar((char)keycode);
            }

            EnterElement* chatenter = static_cast<EnterElement*>(Main::chatUIManager->getElement("chatenter").get());

            if (chatenter->active) {
                if (event->key == "Enter") {
                    ChatMessage packet;
                    packet.message = chatenter->text;
                    SocketClient::getInstance().sendPacket(&packet);

                    chatenter->text = "0";
                    chatenter->removeChar();
                    chatenter->active = false;
                    return;
                }
                if (event->key == "Backspace") {
                    chatenter->removeChar();
                    return;
                }

                std::cout << strlen(event->code) << std::endl;
                if (strlen(event->code) > 1) { return; }

                int keycode = (int)*(event->code);
                chatenter->addChar((char)keycode);
            }
        }
    });

    Main::physicsEngine = std::make_unique<PhysicsEngine>(&Main::chunks);

    // Main::isSingleplayer = true;
    // Main::serverInstance.setCallback(SocketClient::getInstance().on_message);
    // SocketClient::getInstance().on_open();
    // SocketClient::getInstance().connect();

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

    glGenFramebuffers(1, &fbo);
    glGenRenderbuffers(1, &rbo);
    glGenTextures(1, &ftex);
    glGenVertexArrays(1, &fvao);
    glGenBuffers(1, &fvbo);
    glBindBuffer(GL_ARRAY_BUFFER, fvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 4 * 6, &fquad[0], GL_STATIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    glfwMakeContextCurrent(window);
    glViewport(0, 0, windowWidth, windowHeight);

    emscripten_set_main_loop(&mainLoop, 0, 1);

    ticklooprunning = false;
    tick.join();

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

