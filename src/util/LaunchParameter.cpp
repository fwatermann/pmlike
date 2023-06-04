//
// Created by finnw on 05.06.2023.
//
#include <string>
#include "LaunchParameter.hpp"
#include "Log.hpp"

using namespace pmlike::util;

bool LaunchParameter::debug = false;
int LaunchParameter::windowWidth = 1280;
int LaunchParameter::windowHeight = 720;

void pmlike::util::LaunchParameter::parse(int argc, char **argv) {
    for (int i = 0; i < argc; i++) {
        std::string arg = argv[i];
        if (arg == "--debug") {
            LaunchParameter::debug = true;
            LOG_I("Parameter debug = true");
        }
        if (arg == "--width") {
            if(argc < i + 1) {
                LOG_E("Missing argument for --width");
                continue;
            }
            LaunchParameter::windowWidth = std::stoi(argv[i + 1]);
            LOG_IF("Parameter windowWith = %d", LaunchParameter::windowWidth);
        }
        if (arg == "--height") {
            if(argc < i + 1) {
                LOG_E("Missing argument for --height");
                continue;
            }
            LaunchParameter::windowHeight = std::stoi(argv[i + 1]);
            LOG_IF("Parameter windowHeight = %d", LaunchParameter::windowHeight);
        }
    }
}
