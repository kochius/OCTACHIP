#include <cstdint>
#include <gtest/gtest.h>

#include "fixtures/instruction_test.hpp"
#include "core/instructions.hpp"
#include "core/opcode.hpp"
#include "core/types.hpp"

using namespace CHIP8;

TEST_F(InstructionTest, ADD_VX_BYTE_NoCarry_AddsByteToVx) {
    const uint16_t x = 0x0;
    const uint8_t byte = 0x42;
    const Opcode opcode = 0x7000 | (x << 8) | byte;

    registers.v[x] = 0x23;
    const uint8_t vxPlusByte = 0x65;

    instructions::ADD_VX_BYTE(opcode, registers);

    // ADD_VX_BYTE should add the specified byte to Vx and store the result in 
    // Vx
    EXPECT_EQ(vxPlusByte, registers.v[x]);
}

TEST_F(InstructionTest, ADD_VX_BYTE_Carry_AddsByteToVx) {
    const uint16_t x = 0x0;
    const uint8_t byte = 0xFF;
    const Opcode opcode = 0x7000 | (x << 8) | byte;

    registers.v[x] = 0xFF;
    const uint8_t vxPlusByte = 0xFE;

    ASSERT_EQ(0x00, registers.v[0xF]);

    instructions::ADD_VX_BYTE(opcode, registers);

    // ADD_VX_BYTE should add the specified byte to Vx and store the lower 8 
    // bits of the result in Vx
    // ADD_VX_BYTE should ignore carry and leave the carry flag as it is
    EXPECT_EQ(vxPlusByte, registers.v[x]);
    EXPECT_EQ(0x00, registers.v[0xF]);
}

TEST_F(InstructionTest, OR_VX_VY_OrsVxWithVy) {
    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
    const Opcode opcode = 0x8001 | (x << 8) | (y << 4);

    registers.v[x] = 0x42;
    registers.v[y] = 0x23;
    const uint8_t vxOrVy = 0x63;

    instructions::OR_VX_VY(opcode, registers);

    // OR_VX_VY should bitwise OR Vx with Vy and store the result in Vx
    EXPECT_EQ(vxOrVy, registers.v[x]);
}

TEST_F(InstructionTest, AND_VX_VY_AndsVxWithVy) {
    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
    const Opcode opcode = 0x8002 | (x << 8) | (y << 4);

    registers.v[x] = 0x42;
    registers.v[y] = 0x23;
    const uint8_t vxAndVy = 0x02;

    instructions::AND_VX_VY(opcode, registers);

    // AND_VX_VY should bitwise AND Vx with Vy and store the result in Vx
    EXPECT_EQ(vxAndVy, registers.v[x]);
}

TEST_F(InstructionTest, XOR_VX_VY_XorsVxWithVy) {
    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
    const Opcode opcode = 0x8003 | (x << 8) | (y << 4);

    registers.v[x] = 0x42;
    registers.v[y] = 0x23;
    const uint8_t vxXorVy = 0x61;

    instructions::XOR_VX_VY(opcode, registers);

    // XOR_VX_VY should bitwise XOR Vx with Vy and store the result in Vx
    EXPECT_EQ(vxXorVy, registers.v[x]);
}

TEST_F(InstructionTest, ADD_VX_VY_NoCarry_AddsVyToVxAndClearsVF) {
    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
    const Opcode opcode = 0x8004 | (x << 8) | (y << 4);

    registers.v[x] = 0x42;
    registers.v[y] = 0x23;
    const uint8_t vxPlusVy = 0x65;

    instructions::ADD_VX_VY(opcode, registers);

    // ADD_VX_VY should add Vy to Vx and store the result in Vx
    EXPECT_EQ(vxPlusVy, registers.v[x]);

    // (Vx + Vy) <= 255, so ADD_VX_VY should clear the carry flag VF
    EXPECT_EQ(0x00, registers.v[0xF]);
}

TEST_F(InstructionTest, ADD_VX_VY_Carry_AddsVyToVxAndSetsVF) {
    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
    const Opcode opcode = 0x8004 | (x << 8) | (y << 4);

    registers.v[x] = 0xFF;
    registers.v[y] = 0xFF;
    const uint8_t vxPlusVy = 0xFE;

    instructions::ADD_VX_VY(opcode, registers);

    // ADD_VX_VY should add Vy to Vx and store the result in Vx
    EXPECT_EQ(vxPlusVy, registers.v[x]);
    
    // (Vx + Vy) > 255, so ADD_VX_VY should set the carry flag VF
    EXPECT_EQ(0x01, registers.v[0xF]);
}

TEST_F(InstructionTest, SUB_VX_VY_NoBorrow_SubtractsVyFromVxAndSetsVF) {
    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
    const Opcode opcode = 0x8005 | (x << 8) | (y << 4);

    registers.v[x] = 0x42;
    registers.v[y] = 0x23;
    const uint8_t vxMinusVy = 0x1F;

    instructions::SUB_VX_VY(opcode, registers);

    // SUB_VX_VY should subtract Vy from Vx and store the result in Vx
    EXPECT_EQ(vxMinusVy, registers.v[x]);

    // Vx >= Vy, so SUB_VX_VY should set the borrow flag VF
    EXPECT_EQ(0x01, registers.v[0xF]);
}

TEST_F(InstructionTest, SUB_VX_VY_Borrow_SubtractsVyFromVxAndClearsVF) {
    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
    const Opcode opcode = 0x8005 | (x << 8) | (y << 4);

    registers.v[x] = 0x23;
    registers.v[y] = 0x42;
    const uint8_t vxMinusVy = 0xE1;

    instructions::SUB_VX_VY(opcode, registers);

    // SUB_VX_VY should subtract Vy from Vx and store the result in Vx
    EXPECT_EQ(vxMinusVy, registers.v[x]);

    // Vx < Vy, so SUB_VX_VY should clear the borrow flag
    EXPECT_EQ(0x00, registers.v[0xF]);
}

TEST_F(InstructionTest, SHR_VX_VY_LsbZero_DividesVxByTwoAndClearsVF) {
    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
    const Opcode opcode = 0x8006 | (x << 8) | (y << 4);

    registers.v[x] = 0b11110000; // 240
    const uint8_t vxDividedByTwo = 0b01111000; // 120

    instructions::SHR_VX_VY(opcode, registers);

    // SHR_VX_VY should divide Vx by 2 and store the result in Vx
    EXPECT_EQ(vxDividedByTwo, registers.v[x]);

    // Least significant bit == 0, so SHR_VX_VY should clear VF
    EXPECT_EQ(0x00, registers.v[0xF]);
}

TEST_F(InstructionTest, SHR_VX_VY_LsbOne_DividesVxByTwoAndSetsVF) {
    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
    const Opcode opcode = 0x8006 | (x << 8) | (y << 4);
    
    registers.v[x] = 0b11110001; // 241
    const uint8_t vxDividedByTwo = 0b01111000; // 120

    instructions::SHR_VX_VY(opcode, registers);

    // SHR_VX_VY should divide Vx by 2 and store the result in Vx
    EXPECT_EQ(vxDividedByTwo, registers.v[x]);

    // Least significant bit == 1, so SHR_VX_VY should set VF
    EXPECT_EQ(0x01, registers.v[0xF]);
}

TEST_F(InstructionTest, SUBN_VX_VY_NoBorrow_SubtractsVxFromVyAndSetsVF) {
    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
    const Opcode opcode = 0x8007 | (x << 8) | (y << 4);

    registers.v[x] = 0x23;
    registers.v[y] = 0x42;
    const uint8_t vyMinusVx = 0x1F;

    instructions::SUBN_VX_VY(opcode, registers);

    // SUBN_VX_VY should subtract Vx from Vy and store the result in Vx
    EXPECT_EQ(vyMinusVx, registers.v[x]);

    // Vy >= Vx, so SUBN_VX_VY should set the borrow flag VF
    EXPECT_EQ(0x01, registers.v[0xF]);
}

TEST_F(InstructionTest, SUBN_VX_VY_Borrow_SubtractsVxFromVyAndClearsVF) {
    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
    const Opcode opcode = 0x8007 | (x << 8) | (y << 4);

    registers.v[x] = 0x42;
    registers.v[y] = 0x23;
    const uint8_t vyMinusVx = 0xE1;

    instructions::SUBN_VX_VY(opcode, registers);

    // SUBN_VX_VY should subtract Vx from Vy and store the result in Vx
    EXPECT_EQ(vyMinusVx, registers.v[x]);

    // Vy < Vx, so SUBN_VX_VY should clear the borrow flag VF
    EXPECT_EQ(0x00, registers.v[0xF]);
}

TEST_F(InstructionTest, SHL_VX_VY_MsbZero_MultipliesVxByTwoAndClearsVF) {
    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
    const Opcode opcode = 0x800E | (x << 8) | (y << 4);

    registers.v[x] = 0b01110000; // 112
    const uint8_t vxTimesTwo = 0b11100000; // 224

    instructions::SHL_VX_VY(opcode, registers);

    // SHL_VX_VY should multiply Vx by 2 and store the result in Vx
    EXPECT_EQ(vxTimesTwo, registers.v[x]);

    // Most significant bit == 0, so SHL_VX_VY should clear VF
    EXPECT_EQ(0x00, registers.v[0xF]);
}

TEST_F(InstructionTest, SHL_VX_VY_MsbOne_MultipliesVxByTwoAndSetsVF) {
    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
    const Opcode opcode = 0x800E | (x << 8) | (y << 4);

    registers.v[x] = 0b11110000; // 240
    const uint8_t vxTimesTwo = 0b11100000; // 224

    instructions::SHL_VX_VY(opcode, registers);

    // SHL_VX_VY should multiply Vx by 2 and store the result in Vx
    EXPECT_EQ(vxTimesTwo, registers.v[x]);

    // Most significant bit == 1, so SHL_VX_VY should set VF
    EXPECT_EQ(0x01, registers.v[0xF]);
}

TEST_F(InstructionTest, ADD_I_VX_AddsVxToIndexRegister) {
    const uint16_t x = 0x0;
    const Opcode opcode = 0xF01E | (x << 8);

    registers.v[x] = 0x42;
    registers.i = 0x619;
    const uint16_t vxPlusI = 0x65B;

    instructions::ADD_I_VX(opcode, registers);

    // ADD_I_VX should add Vx to the index register and store the result in the 
    // index register
    EXPECT_EQ(vxPlusI, registers.i);
}