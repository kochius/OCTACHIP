#include <cstdint>
#include <exception>
#include <gtest/gtest.h>

#include "src/frame.hpp"
#include "src/instructions.hpp"

using namespace CHIP8;

TEST(InstructionTest, CLS_ClearsFrame) {
    constexpr size_t FRAME_SIZE = Frame::WIDTH * Frame::HEIGHT;
    Frame frame;

    // turn on some pixels
    frame[0] = true;
    frame[FRAME_SIZE - 1] = true;

    CLS(frame);

    // make sure all pixels are off now
    for (size_t i = 0; i < FRAME_SIZE; i++) {
        EXPECT_EQ(false, frame[i]);
    }
}

TEST(InstructionTest, RET_ReturnsFromSubroutine) {
    Registers registers;
    Stack stack;

    // RET should throw an exception if there's nothing to return to
    try {
        RET(registers, stack);
        FAIL();
    }
    catch (std::exception& ex) {
        EXPECT_STREQ("RET: Attempted to return from a subroutine, but the stack "
            "is empty", ex.what());
    }

    // Call a subroutine at address 0x400
    const Opcode opcode = 0x2400;
    const uint16_t oldPcValue = registers.pc;
    const uint8_t oldSpValue = registers.sp;
    CALL_addr(opcode, registers, stack);

    RET(registers, stack);

    // RET should restore the previous values to pc and sp
    EXPECT_EQ(oldPcValue, registers.pc);
    EXPECT_EQ(oldSpValue, registers.sp);
}

TEST(InstructionTest, JP_addr_SetsProgramCounterToNNN) {
    const uint16_t address = 0x600;
    const Opcode opcode = 0x1000 | address;
    Registers registers;
    
    JP_addr(opcode, registers);

    // PC should jump to address 0x600
    EXPECT_EQ(address, registers.pc);
}

TEST(InstructionTest, CALL_addr_CallsSubroutineNNN) {
    const uint16_t address = 0x800;
    const Opcode opcode = 0x2000 | address;
    Registers registers;
    Stack stack;
    const uint16_t oldPcValue = registers.pc;
    const uint16_t newSpValue = registers.sp + 1;

    CALL_addr(opcode, registers, stack);

    // old pc value should be pushed onto stack, sp should be incremented, and
    // pc should jump to the specified address
    EXPECT_EQ(oldPcValue, stack[registers.sp - 1]);
    EXPECT_EQ(newSpValue, registers.sp);
    EXPECT_EQ(address, registers.pc);

    // CALL_addr should throw an exception when too many addresses are pushed
    // onto the stack
    try {
        for (size_t i = newSpValue; i <= STACK_SIZE; i++) {
            CALL_addr(opcode, registers, stack);
        }
        FAIL();
    }
    catch (std::exception& ex) {
        EXPECT_STREQ("CALL_addr: Stack overflow error", ex.what());
    }
}