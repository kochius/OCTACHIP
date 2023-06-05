#pragma once

#include <chrono>
#include <filesystem>

#include "core/interpreter.hpp"
#include "io/input.hpp"
#include "io/renderer.hpp"

namespace CHIP8 {

class Emulator {
public:
    Emulator(const int windowScale, const int instructionsPerSecond);

    void reset();
    void refreshUpdateTimer();
    void loadRom(const std::filesystem::path& romPath);
    void setSpeed(const int instructionsPerSecond);
    void update();

    uint8_t getRegisterValue(const int index) const;
    uint16_t getProgramCounterValue() const;
    uint16_t getIndexRegisterValue() const;
    uint8_t getStackPointerValue() const;
    uint8_t getDelayTimerValue() const;
    uint8_t getSoundTimerValue() const;
    uint16_t getStackValue(const int index) const;
private:
    static constexpr double UPDATES_PER_SECOND = 60.0;
    static constexpr double UPDATE_INTERVAL = 1.0 / UPDATES_PER_SECOND;

    std::chrono::high_resolution_clock::time_point lastUpdateTime;
    double accumulator;
    int instructionsPerUpdate;

    Interpreter interpreter;
    Input input;
    Renderer renderer;

    double getDeltaTime();
};

}