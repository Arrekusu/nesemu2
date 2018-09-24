#pragma once

#include <iostream>

enum class nes_log_level{
    ERROR,
    INFO,
    DEBUG
};
class nes_logger {
private:
    nes_log_level _level = nes_log_level::INFO;
public:
    static void log(const char* message){
        std::cout << message << std::endl;
    }
};

#define NES_LOG(message) nes_logger::log(message)