//
// Created by finnw on 30.05.2023.
//

#include "glm/gtx/string_cast.hpp"
#include "Game.hpp"
#include "./util/Log.hpp"
#include "./world/generator/NoiseGenerator.hpp"
#include "util/SystemResources.hpp"
#include "util/LaunchParameter.hpp"
#include <deque>
#include <numeric>

using namespace pmlike;

Game::Game(GLFWwindow *window) : window(window) {
    std::shared_ptr<world::generator::NoiseGenerator> generator = std::make_shared<world::generator::NoiseGenerator>();
    this->camera = std::make_shared<render::Camera>();
    this->camera->setPosition(glm::vec3(0, 64, 0));
    this->debug = pmlike::util::LaunchParameter::debug;

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    util::os::initCPU();
}

Game::~Game() = default;

void Game::render(float deltaTime) {
    static bool first = true;
    if (first) {
        glEnable(GL_DEPTH_TEST);
        first = false;
        world::World::getInstance()->setGenerator(std::make_shared<world::generator::NoiseGenerator>());
        world::World::getInstance()->start();
    }

    if (this->debugFont == nullptr) {
        this->debugFont = new render::Font("./assets/fonts/arial.fnt", false);
    }

    this->camera->update();
    world::World::getInstance()->render(this->camera, deltaTime);

    //HUD
    glClear(GL_DEPTH_BUFFER_BIT);
    this->renderDebugTexts(deltaTime);

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
    if(key == GLFW_KEY_F3) {
        if(action != GLFW_PRESS) return;
        debug = !debug;
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

void Game::renderDebugTexts(float deltaTime) {
    if(this->debugFont == nullptr) return;

    // Smooth FPS
    int rawFPS = (int) std::floor((1.0 / deltaTime));
    static int lastFPS = -1;
    static float lastFrameTime = 0.0f;
    if (lastFPS == -1) {
        // sets first value to refresh rate
        rawFPS = glfwGetVideoMode(glfwGetPrimaryMonitor())->refreshRate;
    }
    static std::deque<std::pair<int, float>> fpsTimingHistory;
    fpsTimingHistory.push_back(std::make_pair(rawFPS, deltaTime * 1000));

    int width, height;

    glfwGetWindowSize(this->window, &width, &height);
    this->debugFont->updateDisplaySize(width, height);
    glm::mat4 projection = glm::ortho(0.0f, (float) width, (float) height, 0.0f);

    if(!debug) {
        this->debugFont->renderText((std::string("PMLike Version ") + GAME_VERSION).c_str(), 5, 05, 20, 0, projection, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
        return;
    }

    static int totalVirtualMemory = 0;
    static int totalUsedVirtualMemory = 0;
    static double cpuUsage = -1;
    static std::chrono::time_point nextUpdate = std::chrono::system_clock::now();

    if(nextUpdate - std::chrono::system_clock::now() < std::chrono::seconds(0)) {
        totalVirtualMemory = (int) ((pmlike::util::os::getTotalVirtualMemory() / util::os::MB));
        totalUsedVirtualMemory = (int) ((pmlike::util::os::getUsedVirtualMemoryOfCurrentProcess() /  util::os::MB));
        cpuUsage = pmlike::util::os::getCPUUsageOfCurrentProcess();
        nextUpdate = std::chrono::system_clock::now() + std::chrono::milliseconds(500);
        
        // calcs the average fps and frame time
        int fpsSum = 0;
        float frameTimeSum = 0.0f;
        for(auto &pair : fpsTimingHistory) {
            fpsSum += pair.first;
            frameTimeSum += pair.second;
        }
        lastFPS = (int) std::floor(fpsSum / fpsTimingHistory.size());
        lastFrameTime = frameTimeSum / fpsTimingHistory.size();
        fpsTimingHistory.clear();
    }

    std::string posString = "Pos: " + std::to_string(this->camera->position.x) + ", " + std::to_string(this->camera->position.y) + ", " + std::to_string(this->camera->position.z);
    std::string dirString = "Yaw: " + std::to_string(this->camera->getYaw()) + ", Pitch: " + std::to_string(this->camera->getPitch());
    std::string fpsString = "FPS: " + std::to_string(lastFPS) + " FrameTime: " + std::to_string(lastFrameTime) + "ms";
    std::string render = "Render: D:" + glm::to_string(world::World::getInstance()->getLoadDistance()) + " C:" + std::to_string(world::World::getInstance()->renderedChunks);
    std::string chunks = "Chunks: Q: " + std::to_string(world::World::getInstance()->getNumberOfQueuedChunks()) + " L: " + std::to_string(world::World::getInstance()->getNumberOfLoadedChunks());
    std::string systemMem = "Memory: " + std::to_string(totalUsedVirtualMemory) + "/" + std::to_string(totalVirtualMemory) + "MB";
    std::string systemCpu = "CPU: " + std::to_string(cpuUsage) + "%";


    this->debugFont->renderText(fpsString.c_str(), 5, 05, 20, 0, projection, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    this->debugFont->renderText(posString.c_str(), 5, 30, 20, 0, projection, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    this->debugFont->renderText(dirString.c_str(), 5, 55, 20, 0, projection, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    this->debugFont->renderText(render.c_str(), 5, 80, 20, 0, projection, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    this->debugFont->renderText(chunks.c_str(), 5, 105, 20, 0, projection, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    this->debugFont->renderText(systemMem.c_str(), width - 5, 5, 20, 0, projection, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), pmlike::render::Alignment::END, pmlike::render::Alignment::BEGIN);
    this->debugFont->renderText(systemCpu.c_str(), width - 5, 30, 20, 0, projection, glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), pmlike::render::Alignment::END, pmlike::render::Alignment::BEGIN);
}
