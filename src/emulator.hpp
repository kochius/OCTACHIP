#pragma once

#include <chrono>
#include <filesystem>

#include "core/interpreter.hpp"
#include "io/input.hpp"
#include "io/renderer.hpp"

namespace OCTACHIP {

class Emulator {
public:
    Emulator(const std::filesystem::path& romPath, 
        const int instructionsPerSecond, const int windowScale);
    void run();
private:
    static constexpr double UPDATES_PER_SECOND = 60.0;
    static constexpr double UPDATE_INTERVAL = 1.0 / UPDATES_PER_SECOND;

    const int instructionsPerUpdate;

    Interpreter interpreter;
    Input input;
    Renderer renderer;

    double getDeltaTime(
        std::chrono::high_resolution_clock::time_point& lastUpdateTime);
};

}