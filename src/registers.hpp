#pragma once

#include <array>
#include <cstdint>

struct Registers {
    static constexpr int V_REG_COUNT = 16;

    std::array<uint8_t, V_REG_COUNT> v{};
    uint16_t pc{};
    uint16_t i{};
    uint8_t delayTimer{};
    uint8_t soundTimer{};
};