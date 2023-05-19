#pragma once

#include <gtest/gtest.h>

#include "src/core/types.hpp"
#include "src/core/opcode.hpp"

namespace CHIP8 {

class SkipVxByteTest : public ::testing::Test {
protected:
    const uint16_t x = 0x5;
    const uint16_t kk = 0x24;
    Opcode opcode = 0x3000 | (x << 8) | kk;
    Registers registers;
};

}