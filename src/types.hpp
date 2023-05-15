#pragma once

#include <array>
#include <cstdint>

namespace CHIP8 {

static constexpr int MEMORY_SIZE = 4096;
using Memory = std::array<uint8_t, MEMORY_SIZE>;

static constexpr int STACK_SIZE = 16;
using Stack = std::array<uint16_t, STACK_SIZE>;

static constexpr int KEY_COUNT = 16;
using Keypad = std::array<bool, KEY_COUNT>;

struct Registers {
    static constexpr int V_REG_COUNT = 16;

    std::array<uint8_t, V_REG_COUNT> v{};
    uint16_t pc{};
    uint16_t i{};
    uint8_t sp{};
    uint8_t delayTimer{};
    uint8_t soundTimer{};
};

}