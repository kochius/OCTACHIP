#include <cstdint>
#include <gtest/gtest.h>

#include "fixtures/VxVyInstructionTest.hpp"
#include "src/core/frame.hpp"
#include "src/core/instructions.hpp"

using namespace CHIP8;

TEST(InstructionTest, CLS_ClearsFrame) {
    constexpr size_t FRAME_SIZE = Frame::WIDTH * Frame::HEIGHT;
    Frame frame;

    // turn on some pixels
    frame[0] = true;
    frame[FRAME_SIZE - 1] = true;

    instructions::CLS(frame);

    // make sure all pixels are off now
    for (size_t i = 0; i < FRAME_SIZE; i++) {
        EXPECT_EQ(false, frame[i]);
    }
}

TEST(InstructionTest, RET_NonEmptyStack_ReturnsFromSubroutine) {
    Registers registers;
    Stack stack;

    // Call a subroutine at address 0x400
    constexpr uint16_t address = 0x400;
    const Opcode opcode = 0x2000 | address;
    const uint16_t oldPcValue = registers.pc;
    const uint8_t oldSpValue = registers.sp;
    instructions::CALL_addr(opcode, registers, stack);

    instructions::RET(registers, stack);

    // RET should restore the previous values to pc and sp
    EXPECT_EQ(oldPcValue, registers.pc);
    EXPECT_EQ(oldSpValue, registers.sp);
}

TEST(InstructionTest, RET_EmptyStack_ThrowsException) {
    Registers registers;
    Stack stack;

    // RET should throw an exception if there's nothing to return to
    try {
        instructions::RET(registers, stack);
        FAIL();
    }
    catch (std::exception& ex) {
        EXPECT_STREQ("RET: Attempted to return from a subroutine, but the stack"
            " is empty", ex.what());
    }
}

TEST(InstructionTest, JP_addr_SetsProgramCounterToNNN) {
    constexpr uint16_t address = 0x600;
    const Opcode opcode = 0x1000 | address;
    Registers registers;
    
    instructions::JP_addr(opcode, registers);

    // PC should jump to address 0x600
    EXPECT_EQ(address, registers.pc);
}

TEST(InstructionTest, CALL_addr_NonFullStack_CallsSubroutineNNN) {
    constexpr uint16_t address = 0x800;
    const Opcode opcode = 0x2000 | address;
    Registers registers;
    Stack stack;
    const uint16_t oldPcValue = registers.pc;
    const uint16_t newSpValue = registers.sp + 1;

    instructions::CALL_addr(opcode, registers, stack);

    // old pc value should be pushed onto stack
    // sp should be incremented
    // pc should jump to the specified address
    EXPECT_EQ(oldPcValue, stack[registers.sp - 1]);
    EXPECT_EQ(newSpValue, registers.sp);
    EXPECT_EQ(address, registers.pc);
}

TEST(InstructionTest, CALL_addr_FullStack_ThrowsException) {
    constexpr uint16_t address = 0x800;
    const Opcode opcode = 0x2000 | address;
    Registers registers;
    Stack stack;

    // CALL_addr should throw an exception when too many addresses are pushed
    // onto the stack
    try {
        for (size_t i = 0; i <= STACK_SIZE; i++) {
            instructions::CALL_addr(opcode, registers, stack);
        }
        FAIL();
    }
    catch (std::exception& ex) {
        EXPECT_STREQ("CALL_addr: Stack overflow error", ex.what());
    }
}

TEST(InstructionTest, SE_Vx_byte_VxEqualToByte_SkipsNextInstruction) {
    const uint16_t x = 0x5;
    const uint16_t kk = 0x24;
    Opcode opcode = 0x3000 | (x << 8) | kk;
    Registers registers;

    const uint16_t newPcValue = registers.pc + 2;
    registers.v[x] = kk;

    instructions::SE_Vx_byte(opcode, registers);

    // SE_Vx_byte should increment pc by 2 because Vx == kk
    EXPECT_EQ(newPcValue, registers.pc);
}

TEST(InstructionTest, SE_Vx_byte_VxNotEqualToByte_DoesNotSkipNextInstruction) {
    const uint16_t x = 0x5;
    const uint16_t kk = 0x24;
    Opcode opcode = 0x3000 | (x << 8) | kk;
    Registers registers;

    const uint16_t oldPcValue = registers.pc;

    instructions::SE_Vx_byte(opcode, registers);

    // SE_Vx_byte should NOT increment pc by 2 because Vx != kk
    EXPECT_EQ(oldPcValue, registers.pc);
}

TEST(InstructionTest, SNE_Vx_byte_VxEqualToByte_DoesNotSkipNextInstruction) {
    const uint16_t x = 0x5;
    const uint16_t kk = 0x24;
    Opcode opcode = 0x4000 | (x << 8) | kk;
    Registers registers;

    const uint16_t oldPcValue = registers.pc;
    registers.v[x] = kk;

    instructions::SNE_Vx_byte(opcode, registers);

    // SNE_Vx_byte should NOT increment pc by 2 because Vx == kk
    EXPECT_EQ(oldPcValue, registers.pc);
}

TEST(InstructionTest, SNE_Vx_byte_VxNotEqualToByte_SkipsNextInstruction) {
    const uint16_t x = 0x5;
    const uint16_t kk = 0x24;
    Opcode opcode = 0x4000 | (x << 8) | kk;
    Registers registers;

    const uint16_t newPcValue = registers.pc + 2;

    instructions::SNE_Vx_byte(opcode, registers);

    // SNE_Vx_byte should increment pc by 2 because Vx != kk
    EXPECT_EQ(newPcValue, registers.pc);
}

TEST_F(VxVyInstructionTest, SE_Vx_Vy_VxEqualToVy_SkipsNextInstruction) {
    registers.v[x] = 0x42;
    registers.v[y] = registers.v[x];
    const uint16_t newPcValue = registers.pc + 2;

    instructions::SE_Vx_Vy(opcode, registers);

    // SE_Vx_Vy should increment pc by 2 because Vx == Vy
    EXPECT_EQ(newPcValue, registers.pc);
}

TEST_F(VxVyInstructionTest, 
    SE_Vx_Vy_VxNotEqualToVy_DoesNotSkipNextInstruction) {
    registers.v[x] = 0x42;
    registers.v[y] = 0x55;
    const uint16_t oldPcValue = registers.pc;

    instructions::SE_Vx_Vy(opcode, registers);

    // SE_Vx_Vy should NOT increment pc by 2 because Vx != Vy
    EXPECT_EQ(oldPcValue, registers.pc);
}

TEST(InstructionTest, LD_Vx_byte_SetsVxEqualToByte) {
    const uint16_t x = 0x8;
    const uint16_t byte = 0x24;
    Opcode opcode = 0x6000 | (x << 8) | byte;
    Registers registers;

    instructions::LD_Vx_byte(opcode, registers);

    // LD_Vx_byte should set Vx equal to the lower byte of the opcode
    EXPECT_EQ(byte, registers.v[x]);
}

TEST(InstructionTest, ADD_Vx_byte_SetsVxEqualToVxPlusByte) {
    const uint16_t x = 0x8;
    const uint16_t byte = 0x24;
    Opcode opcode = 0x7000 | (x << 8) | byte;
    Registers registers;

    const uint8_t vxPlusByte = registers.v[x] + byte;

    instructions::ADD_Vx_byte(opcode, registers);

    // ADD_Vx_byte should add byte to Vx and store the result in Vx
    EXPECT_EQ(vxPlusByte, registers.v[x]);
}

TEST_F(VxVyInstructionTest, LD_Vx_Vy_SetsVxEqualToVy) {
    registers.v[x] = 0x00;
    registers.v[y] = 0x38;

    instructions::LD_Vx_Vy(opcode, registers);

    // LD_Vx_Vy should set Vx equal to the value in Vy
    EXPECT_EQ(registers.v[y], registers.v[x]);
}

TEST_F(VxVyInstructionTest, OR_Vx_Vy_SetsVxEqualToVxOrVy) {
    registers.v[x] = 0x00;
    registers.v[y] = 0x38;
    const uint8_t vxOrVy = registers.v[x] | registers.v[y];

    instructions::OR_Vx_Vy(opcode, registers);

    // OR_Vx_Vy should bitwise OR Vx with Vy and store the result in Vx
    EXPECT_EQ(vxOrVy, registers.v[x]);
}

TEST_F(VxVyInstructionTest, AND_Vx_Vy_SetsVxEqualToVxAndVy) {
    registers.v[x] = 0x00;
    registers.v[y] = 0x38;
    const uint8_t vxAndVy = registers.v[x] & registers.v[y];

    instructions::AND_Vx_Vy(opcode, registers);

    // AND_Vx_Vy should bitwise AND Vx with Vy and store the result in Vx
    EXPECT_EQ(vxAndVy, registers.v[x]);
}

TEST_F(VxVyInstructionTest, XOR_Vx_Vy_SetsVxEqualToVxXorVy) {
    registers.v[x] = 0x00;
    registers.v[y] = 0x38;
    const uint8_t vxXorVy = registers.v[x] ^ registers.v[y];

    instructions::XOR_Vx_Vy(opcode, registers);

    // XOR_Vx_Vy should bitwise XOR Vx with Vy and store the result in Vx
    EXPECT_EQ(vxXorVy, registers.v[x]);
}

TEST_F(VxVyInstructionTest, 
    ADD_Vx_Vy_NoCarry_SetsVxEqualToVxPlusVyAndClearsVF) {
    registers.v[x] = registers.v[y] = 0x01;
    const uint8_t vxPlusVy = registers.v[x] + registers.v[y];

    instructions::ADD_Vx_Vy(opcode, registers);

    // ADD_Vx_Vy should add Vy to Vx and store the result in Vx
    // Because the result is less than 255, VF is cleared
    EXPECT_EQ(vxPlusVy, registers.v[x]);
    EXPECT_EQ(0, registers.v[0xF]);
}

TEST_F(VxVyInstructionTest, ADD_Vx_Vy_Carry_SetsVxEqualToVxPlusVyAndSetsVF) {
    registers.v[x] = registers.v[y] = 0xFF;
    const uint8_t vxPlusVy = registers.v[x] + registers.v[y];

    instructions::ADD_Vx_Vy(opcode, registers);

    // ADD_Vx_Vy should add Vy to Vx and store the result in Vx
    // Because the result is greater than 255, VF is set
    EXPECT_EQ(vxPlusVy, registers.v[x]);
    EXPECT_EQ(1, registers.v[0xF]);
}

TEST_F(VxVyInstructionTest, 
    SUB_Vx_Vy_NoBorrow_SetsVxEqualToVxMinusVyAndSetsVF) {
    registers.v[x] = 0xFF;
    registers.v[y] = 0x01;
    const uint8_t vxMinusVy = registers.v[x] - registers.v[y];

    instructions::SUB_Vx_Vy(opcode, registers);

    // SUB_Vx_Vy should subtract Vy from Vx and store the result in Vx
    // Because the Vx is greater than Vy, VF is set
    EXPECT_EQ(vxMinusVy, registers.v[x]);
    EXPECT_EQ(1, registers.v[0xF]);
}

TEST_F(VxVyInstructionTest, 
    SUB_Vx_Vy_Borrow_SetsVxEqualToVxMinusVyAndClearsVF) {
    registers.v[x] = 0x01;
    registers.v[y] = 0xFF;
    const uint8_t vxMinusVy = registers.v[x] - registers.v[y];

    instructions::SUB_Vx_Vy(opcode, registers);

    // SUB_Vx_Vy should subtract Vy from Vx and store the result in Vx
    // Because the Vx is less than Vy, VF is cleared
    EXPECT_EQ(vxMinusVy, registers.v[x]);
    EXPECT_EQ(0, registers.v[0xF]);
}

TEST_F(VxVyInstructionTest, SHR_Vx_Vy_LsbOne_DividesVxByTwoAndSetsVF) {
    registers.v[x] = 0x01;
    const uint8_t vxDividedByTwo = registers.v[x] >> 1;

    instructions::SHR_Vx_Vy(opcode, registers);

    // SHR_Vx_Vy should divide Vx by two and store the result in Vx
    // Because the least significant bit is 1, VF is set
    EXPECT_EQ(vxDividedByTwo, registers.v[x]);
    EXPECT_EQ(1, registers.v[0xF]);
}

TEST_F(VxVyInstructionTest, 
    SHR_Vx_Vy_LsbZero_DividesVxByTwoAndClearsVF) {
    registers.v[x] = 0x10;
    const uint8_t vxDividedByTwo = registers.v[x] >> 1;

    instructions::SHR_Vx_Vy(opcode, registers);

    // SHR_Vx_Vy should divide Vx by two and store the result in Vx
    // Because the least significant bit is 0, VF is cleared
    EXPECT_EQ(vxDividedByTwo, registers.v[x]);
    EXPECT_EQ(0, registers.v[0xF]);
}

TEST_F(VxVyInstructionTest, 
    SUBN_Vx_Vy_NoBorrow_SetsVxEqualToVyMinusVxAndSetsVF) {
    registers.v[x] = 0x01;
    registers.v[y] = 0xFF;
    const uint8_t vyMinusVx = registers.v[y] - registers.v[x];

    instructions::SUBN_Vx_Vy(opcode, registers);

    // SUBN_Vx_Vy should subtract Vx from Vy and store the result in Vx
    // Because the Vx is less than Vy, VF is set
    EXPECT_EQ(vyMinusVx, registers.v[x]);
    EXPECT_EQ(1, registers.v[0xF]);
}

TEST_F(VxVyInstructionTest, 
    SUBN_Vx_Vy_Borrow_SetsVxEqualToVyMinusVxAndClearsVF) {
    registers.v[x] = 0xFF;
    registers.v[y] = 0x01;
    const uint8_t vyMinusVx = registers.v[y] - registers.v[x];

    instructions::SUBN_Vx_Vy(opcode, registers);

    // SUBN_Vx_Vy should subtract Vx from Vy and store the result in Vx
    // Because the Vx is greater than Vy, VF is cleared
    EXPECT_EQ(vyMinusVx, registers.v[x]);
    EXPECT_EQ(0, registers.v[0xF]);
}

TEST_F(VxVyInstructionTest, SHL_Vx_Vy_MsbOne_MultipliesVxByTwoAndSetsVF) {
    registers.v[x] = 0b10000000;
    const uint8_t vxTimesTwo = registers.v[x] << 1;

    instructions::SHL_Vx_Vy(opcode, registers);

    // SHL_Vx_Vy should multiply Vx by 2 and store the result in Vx
    // Because the most significant bit of Vx is 1, VF is set
    EXPECT_EQ(vxTimesTwo, registers.v[x]);
    EXPECT_EQ(1, registers.v[0xF]);
}

TEST_F(VxVyInstructionTest, SHL_Vx_Vy_MsbZero_MultipliesVxByTwoAndClearsVF) {
    registers.v[x] = 0b00000001;
    const uint8_t vxTimesTwo = registers.v[x] << 1;

    instructions::SHL_Vx_Vy(opcode, registers);

    // SHL_Vx_Vy should multiply Vx by 2 and store the result in Vx
    // Because the most significant bit of Vx is 0, VF is cleared
    EXPECT_EQ(vxTimesTwo, registers.v[x]);
    EXPECT_EQ(0, registers.v[0xF]);
}