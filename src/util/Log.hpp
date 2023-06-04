//
// Created by finnw on 22.08.2022.
//

#ifndef FORGOTTENENGINE_LOG_HPP
#define FORGOTTENENGINE_LOG_HPP

#include <cstdint>
#include <iostream>
#include <string>
#include <thread>
#include <sstream>

#define LOG_EF(format, ...) pmlike::util::log::err(__FILE__, __LINE__, format, __VA_ARGS__)
#define LOG_E(message) pmlike::util::log::err(__FILE__, __LINE__, message)
#define LOG_WF(format, ...) pmlike::util::log::warn(__FILE__, __LINE__, format, __VA_ARGS__)
#define LOG_W(message) pmlike::util::log::warn(__FILE__, __LINE__, message)
#define LOG_IF(format, ...) pmlike::util::log::info(__FILE__, __LINE__, format, __VA_ARGS__)
#define LOG_I(message) pmlike::util::log::info(__FILE__, __LINE__, message)
#define LOG_DF(format, ...) pmlike::util::log::debug(__FILE__, __LINE__, format, __VA_ARGS__)
#define LOG_D(message) pmlike::util::log::debug(__FILE__, __LINE__, message)

#ifndef DEBUGGING
#define DEBUGGING 1
#endif

namespace pmlike::util::log {


    static int longestPrefix = 1;

    template<typename ... Targs>
    static void LogPrint(FILE *file, const char *level, const char *fileName, const uint64_t lineNr, const char *format,
                         Targs ... args) {
        std::string fname = std::string(fileName);
        fname = fname.substr(fname.find_last_of('/') + 1);
        std::stringstream threadId;
        threadId << std::this_thread::get_id();
        std::string prefix = std::string("[" + fname + ":" + std::to_string(lineNr) + "@" + threadId.str() + "/" + std::string(level) + "]");
        if (prefix.length() > longestPrefix) longestPrefix = prefix.length();
        std::string fout = std::string("%" + std::to_string(longestPrefix) + "s ") + format + "\n";
#ifdef DEBUG
        std::setvbuf(file, nullptr, _IONBF, 0);
#endif
        std::fprintf(file, fout.c_str(), prefix.c_str(), args...);
    }

    template<typename ... Targs>
    static void debug(const char *fileName, const uint64_t lineNr, const char *format, Targs ... args) {
        if (!DEBUGGING) return;
        LogPrint(stdout, "DEBUG", fileName, lineNr, format, args...);
    }

    static void debug(const char *fileName, const uint64_t lineNr, const char *message) {
        if (!DEBUGGING) return;
        LogPrint(stdout, "DEBUG", fileName, lineNr, message);
    }

    template<typename ... Targs>
    static void info(const char *fileName, const uint64_t lineNr, const char *format, Targs ... args) {
        LogPrint(stdout, "INFO", fileName, lineNr, format, args...);
    }

    static void info(const char *fileName, const uint64_t lineNr, const char *message) {
        LogPrint(stdout, "INFO", fileName, lineNr, message);
    }

    template<typename ... Targs>
    static void warn(const char *fileName, const uint64_t lineNr, const char *format, Targs ... args) {
        LogPrint(stderr, "WARNING", fileName, lineNr, format, args...);
    }

    static void warn(const char *fileName, const uint64_t lineNr, const char *message) {
        LogPrint(stderr, "WARNING", fileName, lineNr, message);
    }

    template<typename ... Targs>
    static void err(const char *fileName, const uint64_t lineNr, const char *format, Targs ... args) {
        LogPrint(stderr, "ERROR", fileName, lineNr, format, args...);
    }

    static void err(const char *fileName, const uint64_t lineNr, const char *message) {
        LogPrint(stderr, "ERROR", fileName, lineNr, message);
    }
}


#endif //FORGOTTENENGINE_LOG_HPP
