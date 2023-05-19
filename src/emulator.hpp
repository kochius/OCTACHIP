#pragma once

#include <filesystem>

#include "core/interpreter.hpp"
#include "io/input.hpp"
#include "io/renderer.hpp"

namespace CHIP8 {

class Emulator {
public:
    Emulator();
    void loadRom(const std::filesystem::path& romPath);
    void run(const int ticksPerSecond);
private:
    static constexpr int framesPerSecond = 60;

    Interpreter interpreter;
    Input input;
    Renderer renderer;
    
    long getCurrentTime() const;
};

}