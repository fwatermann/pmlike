//
// Created by finnw on 30.05.2023.
//

#include "Game.hpp"
#include "util/Log.hpp"
#include "world/generator/NoiseGenerator.hpp"

using namespace pmlike;

Game::Game(GLFWwindow *window) : window(window) {
    world::ChunkGenerator *generator = new world::generator::NoiseGenerator();
    this->world = new world::World(generator);
    this->camera = new render::Camera();
    this->camera->setPosition(glm::vec3(0, 64, 0));

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
}

Game::~Game() = default;

void Game::render(float deltaTime) {
    static bool first = true;
    if (first) {
        glEnable(GL_DEPTH_TEST);
        first = false;
    }

    if (this->font == nullptr) {
        this->font = new render::Font("./assets/fonts/arial.fnt", true);
    }

    this->camera->update();
    world->render(this->camera, deltaTime);

    //HUD
    glClear(GL_DEPTH_BUFFER_BIT);
    int width, height;
    int fps = (int) std::floor((1.0 / deltaTime));
    glfwGetWindowSize(this->window, &width, &height);
    this->font->updateDisplaySize(width, height);
    std::string posString =
            "Pos: " + std::to_string(this->camera->position.x) + ", " + std::to_string(this->camera->position.y) +
            ", " + std::to_string(this->camera->position.z);
    std::string dirString =
            "Yaw: " + std::to_string(this->camera->getYaw()) + ", Pitch: " + std::to_string(this->camera->getPitch());
    std::string fpsString = "FPS: " + std::to_string(fps) + " FrameTime: " + std::to_string(deltaTime * 1000) + "ms";
    std::string render = "Render: D:" + std::to_string(this->world->renderDistance) + " C:" +
                         std::to_string(this->world->renderedChunks);

    this->font->renderText(fpsString.c_str(), 5, 05, 20, 0, glm::ortho(0.0f, (float) width, (float) height, 0.0f),
                           glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    this->font->renderText(posString.c_str(), 5, 30, 20, 0, glm::ortho(0.0f, (float) width, (float) height, 0.0f),
                           glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    this->font->renderText(dirString.c_str(), 5, 55, 20, 0, glm::ortho(0.0f, (float) width, (float) height, 0.0f),
                           glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    this->font->renderText(render.c_str(), 5, 80, 20, 0, glm::ortho(0.0f, (float) width, (float) height, 0.0f),
                           glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

    //Controls

    float movementSpeed = 5.0f;

    if (glfwGetKey(this->window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
        movementSpeed *= 4;
    }

    if (glfwGetKey(this->window, GLFW_KEY_W) == GLFW_PRESS) {
        this->camera->move(glm::normalize(glm::vec3(this->camera->front.x, 0.0f, this->camera->front.z)) * deltaTime *
                           movementSpeed);
    }
    if (glfwGetKey(this->window, GLFW_KEY_S) == GLFW_PRESS) {
        this->camera->move(glm::normalize(glm::vec3(this->camera->front.x, 0.0f, this->camera->front.z)) * -deltaTime *
                           movementSpeed);
    }
    if (glfwGetKey(this->window, GLFW_KEY_A) == GLFW_PRESS) {
        this->camera->move(this->camera->right * -deltaTime * movementSpeed);
    }
    if (glfwGetKey(this->window, GLFW_KEY_D) == GLFW_PRESS) {
        this->camera->move(this->camera->right * deltaTime * movementSpeed);
    }
    if (glfwGetKey(this->window, GLFW_KEY_SPACE) == GLFW_PRESS) {
        this->camera->move(glm::vec3(0, 1, 0) * deltaTime * movementSpeed);
    }
    if (glfwGetKey(this->window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
        this->camera->move(glm::vec3(0, 1, 0) * -deltaTime * movementSpeed);
    }
}

void Game::mouseInput(int button, int action, int mods) {

}

void Game::keyboardInput(int key, int scancode, int action, int mods) {
    if (key == GLFW_KEY_ESCAPE) {
        glfwSetWindowShouldClose(this->window, GLFW_TRUE);
    }
    if (key == GLFW_KEY_F11) {
        if (action != GLFW_PRESS) return;
        static bool fullscreen = false;
        fullscreen = !fullscreen;
        GLFWmonitor *monitor = glfwGetPrimaryMonitor();
        const GLFWvidmode *mode = glfwGetVideoMode(monitor);
        if (fullscreen) {
            glfwSetWindowMonitor(this->window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
        } else {
            glfwSetWindowMonitor(this->window, nullptr, 0, 0, mode->width, mode->height, mode->refreshRate);
        }
    }
}

void Game::mouseMove(double xpos, double ypos) {
    static double lastX = 0;
    static double lastY = 0;
    static bool first = true;
    static float mouseSense = 0.1f;

    if (first) {
        lastX = xpos;
        lastY = ypos;
        first = false;
        return;
    }

    double xoffset = xpos - lastX;
    double yoffset = lastY - ypos;
    lastX = xpos;
    lastY = ypos;

    this->camera->setYawRelative((float) xoffset * mouseSense);
    this->camera->setPitchRelative((float) yoffset * mouseSense);
}

void Game::resize(int width, int height) {

}

GLFWwindow *Game::getWindow() {
    return this->window;
}
