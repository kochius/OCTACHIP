#include <cstdint>
#include <gtest/gtest.h>

#include "fixtures/instruction_test.hpp"
#include "core/instructions.hpp"
#include "core/opcode.hpp"
#include "core/types.hpp"

using namespace CHIP8;

TEST_F(InstructionTest, RET_NonEmptyStack_ReturnsFromSubroutine) {
    const uint16_t address = 0x251;
    const Opcode opcode = 0x2000 | address;

    const uint16_t initialPcValue = registers.pc;
    const uint8_t initialSpValue = registers.sp;

    // Call a subroutine at the specified address
    instructions::CALL_ADDR(opcode, registers, stack);

    instructions::RET(registers, stack);

    // RET should restore the initial program counter and stack pointer values
    EXPECT_EQ(initialPcValue, registers.pc);
    EXPECT_EQ(initialSpValue, registers.sp);
}

TEST_F(InstructionTest, RET_EmptyStack_ThrowsException) {
    // RET should throw an exception if the stack is empty
    EXPECT_THROW(instructions::RET(registers, stack), std::underflow_error);
}

TEST_F(InstructionTest, JP_ADDR_SetsProgramCounterToAddress) {
    const uint16_t address = 0x251;
    const Opcode opcode = 0x1000 | address;
    
    instructions::JP_ADDR(opcode, registers);

    // JP_ADDR should set the program counter to the specified address
    EXPECT_EQ(address, registers.pc);
}

TEST_F(InstructionTest, CALL_ADDR_NonFullStack_CallsSubroutineAtAddress) {
    const uint16_t address = 0x251;
    const Opcode opcode = 0x2000 | address;

    const uint16_t initialPcValue = registers.pc;
    const uint16_t incrementedSpValue = registers.sp + 1;

    instructions::CALL_ADDR(opcode, registers, stack);

    // CALL_ADDR should push the initial program counter onto stack
    EXPECT_EQ(initialPcValue, stack[registers.sp - 1]);

    // CALL_ADDR should set the program counter to the specified address
    EXPECT_EQ(address, registers.pc);

    // CALL_ADDR should increment the stack pointer
    EXPECT_EQ(incrementedSpValue, registers.sp);
}

TEST_F(InstructionTest, CALL_ADDR_FullStack_ThrowsException) {
    const uint16_t address = 0x251;
    const Opcode opcode = 0x2000 | address;

    // CALL_ADDR should throw an exception when the stack limit is exceeded
    EXPECT_NO_THROW({
        for (int i = 0; i < STACK_SIZE; i++) {
            instructions::CALL_ADDR(opcode, registers, stack);
        }
    });
    EXPECT_THROW(instructions::CALL_ADDR(opcode, registers, stack), 
        std::overflow_error);
}

TEST_F(InstructionTest, SE_VX_BYTE_VxEqualToByte_SkipsInstruction) {
    const uint16_t x = 0x0;
    const uint8_t byte = 0x42;
    const Opcode opcode = 0x3000 | (x << 8) | byte;

    registers.v[x] = byte;
    const uint16_t incrementedPcValue = registers.pc + 2;

    instructions::SE_VX_BYTE(opcode, registers);

    // Vx == byte, so SE_VX_BYTE should increment the program counter
    EXPECT_EQ(incrementedPcValue, registers.pc);
}

TEST_F(InstructionTest, SE_VX_BYTE_VxNotEqualToByte_DoesNotSkipInstruction) {
    const uint16_t x = 0x0;
    const uint8_t byte = 0x42;
    const Opcode opcode = 0x3000 | (x << 8) | byte;

    registers.v[x] = byte + 1; // Vx != byte
    const uint16_t initialPcValue = registers.pc;

    instructions::SE_VX_BYTE(opcode, registers);

    // Vx != byte, so SE_VX_BYTE should NOT increment the program counter
    EXPECT_EQ(initialPcValue, registers.pc);
}

TEST_F(InstructionTest, SNE_VX_BYTE_VxEqualToByte_DoesNotSkipInstruction) {
    const uint16_t x = 0x0;
    const uint8_t byte = 0x42;
    const Opcode opcode = 0x4000 | (x << 8) | byte;

    registers.v[x] = byte;
    const uint16_t initialPcValue = registers.pc;

    instructions::SNE_VX_BYTE(opcode, registers);

    // Vx == byte, so SNE_VX_BYTE should NOT increment the program counter
    EXPECT_EQ(initialPcValue, registers.pc);
}

TEST_F(InstructionTest, SNE_VX_BYTE_VxNotEqualToByte_SkipsInstruction) {
    const uint16_t x = 0x0;
    const uint8_t byte = 0x42;
    const Opcode opcode = 0x4000 | (x << 8) | byte;

    registers.v[x] = byte + 1; // Vx != byte
    const uint16_t incrementedPcValue = registers.pc + 2;

    instructions::SNE_VX_BYTE(opcode, registers);

    // Vx != byte, so SNE_VX_BYTE should increment the program counter
    EXPECT_EQ(incrementedPcValue, registers.pc);
}

TEST_F(InstructionTest, SE_VX_VY_VxEqualToVy_SkipsInstruction) {
    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
    const Opcode opcode = 0x5000 | (x << 8) | (y << 4);

    registers.v[x] = registers.v[y];
    const uint16_t incrementedPcValue = registers.pc + 2;
    
    instructions::SE_VX_VY(opcode, registers);

    // Vx == Vy, so SE_VX_VY should increment the program counter
    EXPECT_EQ(incrementedPcValue, registers.pc);
}

TEST_F(InstructionTest, SE_VX_VY_VxNotEqualToVy_DoesNotSkipInstruction) {
    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
    const Opcode opcode = 0x5000 | (x << 8) | (y << 4);

    registers.v[x] = registers.v[y] + 1; // Vx != Vy
    const uint16_t initialPcValue = registers.pc;

    instructions::SE_VX_VY(opcode, registers);

    // Vx != Vy, so SE_VX_VY should NOT increment the program counter
    EXPECT_EQ(initialPcValue, registers.pc);
}

TEST_F(InstructionTest, SNE_VX_VY_VxEqualToVy_DoesNotSkipInstruction) {
    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
    const Opcode opcode = 0x9000 | (x << 8) | (y << 4);

    registers.v[x] = registers.v[y];
    const uint16_t initialPcValue = registers.pc;

    instructions::SNE_VX_VY(opcode, registers);

    // Vx == Vy, so SNE_VX_VY should NOT increment the program counter
    EXPECT_EQ(initialPcValue, registers.pc);
}

TEST_F(InstructionTest, SNE_VX_VY_VxNotEqualToVy_SkipsInstruction) {
    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
    const Opcode opcode = 0x9000 | (x << 8) | (y << 4);

    registers.v[x] = registers.v[y] + 1; // Vx != Vy
    const uint16_t incrementedPcValue = registers.pc + 2;

    instructions::SNE_VX_VY(opcode, registers);

    // Vx != Vy, so SNE_VX_VY should increment the program counter
    EXPECT_EQ(incrementedPcValue, registers.pc);
}

TEST_F(InstructionTest, JP_V0_ADDR_SetsProgramCounterToV0PlusAddress) {
    const uint16_t address = 0x251;
    const Opcode opcode = 0xB000 | address;

    registers.v[0x0] = 0x42;
    const uint16_t expectedPcValue = 0x293;

    instructions::JP_V0_ADDR(opcode, registers);

    // JP_VO_ADDR should add the offset in V0 to the specified address and store
    // the result in the program counter
    EXPECT_EQ(expectedPcValue, registers.pc);
}