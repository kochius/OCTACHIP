#pragma once

#include <cstdint>
#include <gtest/gtest.h>

#include "mocks/mock_random.hpp"
#include "core/types.hpp"

namespace OCTACHIP {

class InstructionTest : public ::testing::Test {
protected:
    Memory memory{};
    Registers registers{};
    Stack stack{};
    Frame frame{};
    Keypad keypad{};
    MockRandom random{};
};

}