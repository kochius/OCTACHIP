#include <cstdint>
#include <gtest/gtest.h>

#include "fixtures/instruction_test.hpp"
#include "mocks/mock_random.hpp"
#include "core/instructions.hpp"
#include "core/opcode.hpp"
#include "core/types.hpp"

using namespace CHIP8;

TEST_F(InstructionTest, RND_VX_BYTE_SetsVxToRndAndByte) {
    const uint16_t x = 0x0;
    const uint8_t byte = 0x42;
    const Opcode opcode = 0xC000 | (x << 8) | byte;

    const uint8_t expectedRandomValue = 0x02;

    // Check that the mock RNG is working as expected
    ASSERT_EQ(0x37, random.generateNumber());

    instructions::RND_VX_BYTE(opcode, registers, random);

    // RND_VX_BYTE should bitwise AND a random number with the specified byte
    // and store the result in Vx
    EXPECT_EQ(expectedRandomValue, registers.v[x]);
}