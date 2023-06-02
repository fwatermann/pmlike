#include <iostream>
#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "Game.hpp"

void renderLoop();
void setup();
void cleanup();
void keyboardInput(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouseInput(GLFWwindow* window, int button, int action, int mods);
void mouseMove(GLFWwindow* window, double xpos, double ypos);
void resize(GLFWwindow* window, int width, int height);

GLFWwindow* window;
pmlike::Game* game;

int main() {
    setup();
    renderLoop();
    cleanup();
}

void setup() {

    glfwSetErrorCallback([](int error, const char* description) {
        std::printf("GLFW Error: %s\n", description);
    });

    if(!glfwInit()) {
        exit(EXIT_FAILURE);
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);

    window = glfwCreateWindow(1280, 720, "PM-LIKE", NULL, NULL);
    if(!window) {
        glfwTerminate();
        exit(EXIT_FAILURE);
    }

    glfwMakeContextCurrent(window);

    if(!gladLoadGLLoader((GLADloadproc) glfwGetProcAddress)) {
        std::printf("Failed to initialize GLAD\n");
        exit(EXIT_FAILURE);
    }

    glfwSwapInterval(1);

    glfwSetKeyCallback(window, keyboardInput);
    glfwSetMouseButtonCallback(window, mouseInput);
    glfwSetCursorPosCallback(window, mouseMove);
    glfwSetFramebufferSizeCallback(window, resize);

    if(glfwRawMouseMotionSupported()) {
        glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
    }

    game = new pmlike::Game(window);

}

void keyboardInput(GLFWwindow* window, int key, int scancode, int action, int mods) {
    game->keyboardInput(key, scancode, action, mods);
}

void mouseInput(GLFWwindow* window, int button, int action, int mods) {
    if(window == game->getWindow()) {
        game->mouseInput(button, action, mods);
    }
}

void mouseMove(GLFWwindow* window, double xpos, double ypos) {
    if(window == game->getWindow()) {
        game->mouseMove(xpos, ypos);
    }
}

void resize(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
    if(window == game->getWindow()) {
        game->resize(width, height);
    }
}

void renderLoop() {
    std::chrono::steady_clock::time_point lastFrame = std::chrono::steady_clock::now();
    while(!glfwWindowShouldClose(window)) {
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now();
        float deltaTime = (std::chrono::duration_cast<std::chrono::nanoseconds>(now - lastFrame).count() / 1000000000.0f);
        lastFrame = now;

        glClearColor(0.20f, 0.20f, 0.27f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        game->render(deltaTime);
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    glfwDestroyWindow(window);
    glfwTerminate();
}

void cleanup() {

    delete game;

}