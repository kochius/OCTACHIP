#pragma once

#include <cstdint>
#include <gtest/gtest.h>

#include "src/core/opcode.hpp"
#include "src/core/types.hpp"

namespace CHIP8 {

class VxVyInstructionTest : public ::testing::Test {
protected:
    const uint16_t x = 0x0;
    const uint16_t y = 0x8;
    Opcode opcode = (x << 8) | (y << 4);
    Registers registers;
};

}