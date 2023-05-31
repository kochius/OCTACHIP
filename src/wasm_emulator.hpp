#pragma once

#include "core/frame.hpp"
#include "core/interpreter.hpp"
#include "io/input.hpp"
#include "io/renderer.hpp"

namespace CHIP8 {

struct Emulator {
    static constexpr int INTSTRUCTIONS_PER_UPDATE = 10;
    static constexpr double UPDATES_PER_SECOND = 60.0;
    static constexpr double DELTA_TIME = 1.0 / UPDATES_PER_SECOND;

    Interpreter interpreter{};
    Input input{};
    Renderer renderer{Frame::WIDTH, Frame::HEIGHT, 10, "CHIP-8 Emulator"};
};

}