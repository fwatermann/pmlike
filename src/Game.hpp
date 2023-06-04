//
// Created by finnw on 30.05.2023.
//

#ifndef PM_LIKE_GAME_HPP
#define PM_LIKE_GAME_HPP

#include "GLFW/glfw3.h"
#include "world/World.hpp"
#include "render/Font.hpp"

#define GAME_VERSION "0.1"

namespace pmlike {

    class Game {

        public:
            Game(GLFWwindow *window);

            ~Game();

            void render(float deltaTime);

            void mouseInput(int button, int action, int mods);

            void keyboardInput(int key, int scancode, int action, int mods);

            void mouseMove(double xpos, double ypos);

            void resize(int width, int height);

            GLFWwindow *getWindow();

        private:
            bool debug = false;
            std::shared_ptr<render::Camera> camera;

            GLFWwindow *window;
            pmlike::render::Font *debugFont = nullptr;

            void renderDebugTexts(float deltaTime);

    };

} // pmlike

#endif //PM_LIKE_GAME_HPP
