#pragma once

#include <filesystem>

#include "core/interpreter.hpp"
#include "io/input.hpp"
#include "io/renderer.hpp"

namespace OCTACHIP {

class Emulator {
public:
    Emulator(const int windowScale);
    void loadRom(const std::filesystem::path& romPath);
    void run(const int ticksPerSecond);
private:
    static constexpr int FRAMES_PER_SECOND = 60;

    Interpreter interpreter;
    Input input;
    Renderer renderer;
    
    long getCurrentTime() const;
};

}