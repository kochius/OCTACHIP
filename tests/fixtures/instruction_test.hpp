#pragma once

#include <cstdint>
#include <gtest/gtest.h>

#include "mocks/mock_random.hpp"
#include "core/frame.hpp"
#include "core/types.hpp"

namespace CHIP8 {

class InstructionTest : public ::testing::Test {
protected:
    Memory memory{};
    Registers registers{};
    Stack stack{};
    Frame frame{};
    Keypad keypad{};
    MockRandom<uint8_t> random{};

    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
    const uint16_t address = 0x251;
    const uint8_t byte = 0x42;
};

}