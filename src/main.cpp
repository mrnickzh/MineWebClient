#include <algorithm>
#include <emscripten.h>
#include <iostream>
#include <emscripten/html5.h>
#include <GLFW/glfw3.h>

#define GLM_FORCE_PURE
#include "../lib/glm/glm.hpp"

GLFWwindow* window = nullptr;

int windowWidth = 800;
int windowHeight = 600;

EM_BOOL onResize(int, const EmscriptenUiEvent* e, void*) {
    windowWidth = e->windowInnerWidth;
    windowHeight = e->windowInnerHeight;

    return EM_TRUE;
}

void mainLoop() {
    glClearColor(0.0f, 0.25f, 0.25f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glfwSwapBuffers(window);

    glfwPollEvents();
}

int main() {
    emscripten_get_canvas_element_size("#canvas", &windowWidth, &windowHeight);
    emscripten_set_resize_callback(EMSCRIPTEN_EVENT_TARGET_WINDOW, nullptr, EM_FALSE, onResize);
    //emscripten_request_pointerlock("canvas", EM_TRUE);

    if (!glfwInit())
        exit(EXIT_FAILURE);

    window = glfwCreateWindow(windowWidth, windowHeight, "MineWeb", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return EXIT_FAILURE;
    }

    glfwMakeContextCurrent(window);
    emscripten_set_main_loop(&mainLoop, 0, 1);

    glfwDestroyWindow(window);
    glfwTerminate();

    return EXIT_SUCCESS;
}

