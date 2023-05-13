#pragma once

#include "interpreter.hpp"
#include "io/renderer.hpp"

#include <filesystem>

namespace CHIP8 {

class Emulator {
public:
    Emulator();
    void loadRom(std::filesystem::path romPath);
    void run(const int ticksPerSecond);
private:
    static constexpr int framesPerSecond = 60;

    Interpreter interpreter;
    // Input input;
    // Speaker speaker;
    Renderer renderer;
    
    long getCurrentTime() const;
};

}

//