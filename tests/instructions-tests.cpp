#include <cstdint>
#include <gtest/gtest.h>
#include <stdexcept>
#include <vector>

#include "mocks.hpp"
#include "src/core/frame.hpp"
#include "src/core/instructions.hpp"
#include "src/core/opcode.hpp"
#include "src/core/types.hpp"

using namespace CHIP8;

class InstructionTest : public ::testing::Test {
protected:
    Memory memory{};
    Registers registers{};
    Stack stack{};
    Frame frame{};
    Keypad keypad{};
    MockRandom random{};

    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
    const uint16_t address = 0x251;
    const uint8_t byte = 0x42;
};

TEST_F(InstructionTest, CLS_ClearsFrame) {
    // Turn pixels on
    frame[0] = true;
    frame[1] = true;
    frame[2] = true;

    instructions::CLS(frame);

    // CLS should turn off all pixels
    for (int i = 0; i < Frame::WIDTH * Frame::HEIGHT; i++) {
        EXPECT_EQ(false, frame[i]);
    }
}

TEST_F(InstructionTest, RET_NonEmptyStack_ReturnsFromSubroutine) {
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
    const Opcode opcode = 0x1000 | address;
    
    instructions::JP_ADDR(opcode, registers);

    // JP_ADDR should set the program counter to the specified address
    EXPECT_EQ(address, registers.pc);
}

TEST_F(InstructionTest, CALL_ADDR_NonFullStack_CallsSubroutineAtAddress) {
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
    const Opcode opcode = 0x3000 | (x << 8) | byte;

    registers.v[x] = byte;
    const uint16_t incrementedPcValue = registers.pc + 2;

    instructions::SE_VX_BYTE(opcode, registers);

    // Vx == byte, so SE_VX_BYTE should increment the program counter
    EXPECT_EQ(incrementedPcValue, registers.pc);
}

TEST_F(InstructionTest, SE_VX_BYTE_VxNotEqualToByte_DoesNotSkipInstruction) {
    const Opcode opcode = 0x3000 | (x << 8) | byte;

    registers.v[x] = byte + 1; // Vx != byte
    const uint16_t initialPcValue = registers.pc;

    instructions::SE_VX_BYTE(opcode, registers);

    // Vx != byte, so SE_VX_BYTE should NOT increment the program counter
    EXPECT_EQ(initialPcValue, registers.pc);
}

TEST_F(InstructionTest, SNE_VX_BYTE_VxEqualToByte_DoesNotSkipInstruction) {
    const Opcode opcode = 0x4000 | (x << 8) | byte;

    registers.v[x] = byte;
    const uint16_t initialPcValue = registers.pc;

    instructions::SNE_VX_BYTE(opcode, registers);

    // Vx == byte, so SNE_VX_BYTE should NOT increment the program counter
    EXPECT_EQ(initialPcValue, registers.pc);
}

TEST_F(InstructionTest, SNE_VX_BYTE_VxNotEqualToByte_SkipsInstruction) {
    const Opcode opcode = 0x4000 | (x << 8) | byte;

    registers.v[x] = byte + 1; // Vx != byte
    const uint16_t incrementedPcValue = registers.pc + 2;

    instructions::SNE_VX_BYTE(opcode, registers);

    // Vx != byte, so SNE_VX_BYTE should increment the program counter
    EXPECT_EQ(incrementedPcValue, registers.pc);
}

TEST_F(InstructionTest, SE_VX_VY_VxEqualToVy_SkipsInstruction) {
    const Opcode opcode = 0x5000 | (x << 8) | (y << 4);

    registers.v[x] = registers.v[y];
    const uint16_t incrementedPcValue = registers.pc + 2;
    
    instructions::SE_VX_VY(opcode, registers);

    // Vx == Vy, so SE_VX_VY should increment the program counter
    EXPECT_EQ(incrementedPcValue, registers.pc);
}

TEST_F(InstructionTest, SE_VX_VY_VxNotEqualToVy_DoesNotSkipInstruction) {
    const Opcode opcode = 0x5000 | (x << 8) | (y << 4);

    registers.v[x] = registers.v[y] + 1; // Vx != Vy
    const uint16_t initialPcValue = registers.pc;

    instructions::SE_VX_VY(opcode, registers);

    // Vx != Vy, so SE_VX_VY should NOT increment the program counter
    EXPECT_EQ(initialPcValue, registers.pc);
}

TEST_F(InstructionTest, LD_VX_BYTE_SetsVxToByte) {
    const Opcode opcode = 0x6000 | (x << 8) | byte;

    instructions::LD_VX_BYTE(opcode, registers);

    // LD_VX_BYTE should set Vx to the specified byte
    EXPECT_EQ(byte, registers.v[x]);
}

TEST_F(InstructionTest, ADD_VX_BYTE_AddsByteToVx) {
    const Opcode opcode = 0x7000 | (x << 8) | byte;

    registers.v[x] = 0x23;
    constexpr uint8_t vxPlusByte = 0x65;

    instructions::ADD_VX_BYTE(opcode, registers);

    // ADD_VX_BYTE should add the specified byte to Vx and store the result in 
    // Vx
    EXPECT_EQ(vxPlusByte, registers.v[x]);
}

TEST_F(InstructionTest, LD_VX_VY_SetsVxToVy) {
    const Opcode opcode = 0x8000 | (x << 8) | (y << 4);

    registers.v[y] = 0x23;

    instructions::LD_VX_VY(opcode, registers);

    // LD_VX_VY should set Vx to Vy
    EXPECT_EQ(registers.v[y], registers.v[x]);
}

TEST_F(InstructionTest, OR_VX_VY_OrsVxWithVy) {
    const Opcode opcode = 0x8001 | (x << 8) | (y << 4);

    registers.v[x] = 0x42;
    registers.v[y] = 0x23;
    constexpr uint8_t vxOrVy = 0x63;

    instructions::OR_VX_VY(opcode, registers);

    // OR_VX_VY should bitwise OR Vx with Vy and store the result in Vx
    EXPECT_EQ(vxOrVy, registers.v[x]);
}

TEST_F(InstructionTest, AND_VX_VY_AndsVxWithVy) {
    const Opcode opcode = 0x8002 | (x << 8) | (y << 4);

    registers.v[x] = 0x42;
    registers.v[y] = 0x23;
    constexpr uint8_t vxAndVy = 0x02;

    instructions::AND_VX_VY(opcode, registers);

    // AND_VX_VY should bitwise AND Vx with Vy and store the result in Vx
    EXPECT_EQ(vxAndVy, registers.v[x]);
}

TEST_F(InstructionTest, XOR_VX_VY_XorsVxWithVy) {
    const Opcode opcode = 0x8003 | (x << 8) | (y << 4);

    registers.v[x] = 0x42;
    registers.v[y] = 0x23;
    constexpr uint8_t vxXorVy = 0x61;

    instructions::XOR_VX_VY(opcode, registers);

    // XOR_VX_VY should bitwise XOR Vx with Vy and store the result in Vx
    EXPECT_EQ(vxXorVy, registers.v[x]);
}

TEST_F(InstructionTest, ADD_VX_VY_NoCarry_AddsVyToVxAndClearsVF) {
    const Opcode opcode = 0x8004 | (x << 8) | (y << 4);

    registers.v[x] = 0x42;
    registers.v[y] = 0x23;
    constexpr uint8_t vxPlusVy = 0x65;

    instructions::ADD_VX_VY(opcode, registers);

    // ADD_VX_VY should add Vy to Vx and store the result in Vx
    EXPECT_EQ(vxPlusVy, registers.v[x]);

    // (Vx + Vy) <= 255, so ADD_VX_VY should clear the carry flag VF
    EXPECT_EQ(0x00, registers.v[0xF]);
}

TEST_F(InstructionTest, ADD_VX_VY_Carry_AddsVyToVxAndSetsVF) {
    const Opcode opcode = 0x8004 | (x << 8) | (y << 4);

    registers.v[x] = 0xFF;
    registers.v[y] = 0xFF;
    constexpr uint8_t vxPlusVy = 0xFE;

    instructions::ADD_VX_VY(opcode, registers);

    // ADD_VX_VY should add Vy to Vx and store the result in Vx
    EXPECT_EQ(vxPlusVy, registers.v[x]);
    
    // (Vx + Vy) > 255, so ADD_VX_VY should set the carry flag VF
    EXPECT_EQ(0x01, registers.v[0xF]);
}

TEST_F(InstructionTest, SUB_VX_VY_NoBorrow_SubtractsVyFromVxAndSetsVF) {
    const Opcode opcode = 0x8005 | (x << 8) | (y << 4);

    registers.v[x] = 0x42;
    registers.v[y] = 0x23;
    constexpr uint8_t vxMinusVy = 0x1F;

    instructions::SUB_VX_VY(opcode, registers);

    // SUB_VX_VY should subtract Vy from Vx and store the result in Vx
    EXPECT_EQ(vxMinusVy, registers.v[x]);

    // Vx >= Vy, so SUB_VX_VY should set the borrow flag VF
    EXPECT_EQ(0x01, registers.v[0xF]);
}

TEST_F(InstructionTest, SUB_VX_VY_Borrow_SubtractsVyFromVxAndClearsVF) {
    const Opcode opcode = 0x8005 | (x << 8) | (y << 4);

    registers.v[x] = 0x23;
    registers.v[y] = 0x42;
    constexpr uint8_t vxMinusVy = 0xE1;

    instructions::SUB_VX_VY(opcode, registers);

    // SUB_VX_VY should subtract Vy from Vx and store the result in Vx
    EXPECT_EQ(vxMinusVy, registers.v[x]);

    // Vx < Vy, so SUB_VX_VY should clear the borrow flag
    EXPECT_EQ(0x00, registers.v[0xF]);
}

TEST_F(InstructionTest, SHR_VX_VY_LsbZero_DividesVxByTwoAndClearsVF) {
    const Opcode opcode = 0x8006 | (x << 8) | (y << 4);

    registers.v[x] = 0b11110000; // 240
    constexpr uint8_t vxDividedByTwo = 0b01111000; // 120

    instructions::SHR_VX_VY(opcode, registers);

    // SHR_VX_VY should divide Vx by 2 and store the result in Vx
    EXPECT_EQ(vxDividedByTwo, registers.v[x]);

    // Least significant bit == 0, so SHR_VX_VY should clear VF
    EXPECT_EQ(0x00, registers.v[0xF]);
}

TEST_F(InstructionTest, SHR_VX_VY_LsbOne_DividesVxByTwoAndSetsVF) {
    const Opcode opcode = 0x8006 | (x << 8) | (y << 4);
    
    registers.v[x] = 0b11110001; // 241
    constexpr uint8_t vxDividedByTwo = 0b01111000; // 120

    instructions::SHR_VX_VY(opcode, registers);

    // SHR_VX_VY should divide Vx by 2 and store the result in Vx
    EXPECT_EQ(vxDividedByTwo, registers.v[x]);

    // Least significant bit == 1, so SHR_VX_VY should set VF
    EXPECT_EQ(0x01, registers.v[0xF]);
}

TEST_F(InstructionTest, SUBN_VX_VY_NoBorrow_SubtractsVxFromVyAndSetsVF) {
    const Opcode opcode = 0x8007 | (x << 8) | ( y << 4);

    registers.v[x] = 0x23;
    registers.v[y] = 0x42;
    constexpr uint8_t vyMinusVx = 0x1F;

    instructions::SUBN_VX_VY(opcode, registers);

    // SUBN_VX_VY should subtract Vx from Vy and store the result in Vx
    EXPECT_EQ(vyMinusVx, registers.v[x]);

    // Vy >= Vx, so SUBN_VX_VY should set the borrow flag VF
    EXPECT_EQ(0x01, registers.v[0xF]);
}

TEST_F(InstructionTest, SUBN_VX_VY_Borrow_SubtractsVxFromVyAndClearsVF) {
    const Opcode opcode = 0x8007 | (x << 8) | (y << 4);

    registers.v[x] = 0x42;
    registers.v[y] = 0x23;
    constexpr uint8_t vyMinusVx = 0xE1;

    instructions::SUBN_VX_VY(opcode, registers);

    // SUBN_VX_VY should subtract Vx from Vy and store the result in Vx
    EXPECT_EQ(vyMinusVx, registers.v[x]);

    // Vy < Vx, so SUBN_VX_VY should clear the borrow flag VF
    EXPECT_EQ(0x00, registers.v[0xF]);
}

TEST_F(InstructionTest, SHL_VX_VY_MsbZero_MultipliesVxByTwoAndClearsVF) {
    const Opcode opcode = 0x800E | (x << 8) | (y << 4);

    registers.v[x] = 0b01110000; // 112
    constexpr uint8_t vxTimesTwo = 0b11100000; // 224

    instructions::SHL_VX_VY(opcode, registers);

    // SHL_VX_VY should multiply Vx by 2 and store the result in Vx
    EXPECT_EQ(vxTimesTwo, registers.v[x]);

    // Most significant bit == 0, so SHL_VX_VY should clear VF
    EXPECT_EQ(0x00, registers.v[0xF]);
}

TEST_F(InstructionTest, SHL_VX_VY_MsbOne_MultipliesVxByTwoAndSetsVF) {
    const Opcode opcode = 0x800E | (x << 8) | (y << 4);

    registers.v[x] = 0b11110000; // 240
    constexpr uint8_t vxTimesTwo = 0b11100000; // 224

    instructions::SHL_VX_VY(opcode, registers);

    // SHL_VX_VY should multiply Vx by 2 and store the result in Vx
    EXPECT_EQ(vxTimesTwo, registers.v[x]);

    // Most significant bit == 1, so SHL_VX_VY should set VF
    EXPECT_EQ(0x01, registers.v[0xF]);
}

TEST_F(InstructionTest, SNE_VX_VY_VxEqualToVy_DoesNotSkipInstruction) {
    const Opcode opcode = 0x9000 | (x << 8) | (y << 4);

    registers.v[x] = registers.v[y];
    const uint16_t initialPcValue = registers.pc;

    instructions::SNE_VX_VY(opcode, registers);

    // Vx == Vy, so SNE_VX_VY should NOT increment the program counter
    EXPECT_EQ(initialPcValue, registers.pc);
}

TEST_F(InstructionTest, SNE_VX_VY_VxNotEqualToVy_SkipsInstruction) {
    const Opcode opcode = 0x9000 | (x << 8) | (y << 4);

    registers.v[x] = registers.v[y] + 1; // Vx != Vy
    const uint16_t incrementedPcValue = registers.pc + 2;

    instructions::SNE_VX_VY(opcode, registers);

    // Vx != Vy, so SNE_VX_VY should increment the program counter
    EXPECT_EQ(incrementedPcValue, registers.pc);
}

TEST_F(InstructionTest, LD_I_ADDR_SetsIndexRegisterToAddress) {
    const Opcode opcode = 0xA000 | address;

    instructions::LD_I_ADDR(opcode, registers);

    // LD_I_ADDR should set the index register to the specified address
    EXPECT_EQ(address, registers.i);
}

TEST_F(InstructionTest, JP_V0_ADDR_SetsProgramCounterToV0PlusAddress) {
    const Opcode opcode = 0xB000 | address;

    registers.v[0x0] = 0x42;
    constexpr uint16_t expectedPcValue = 0x293;

    instructions::JP_V0_ADDR(opcode, registers);

    // JP_VO_ADDR should add the offset in V0 to the specified address and store
    // the result in the program counter
    EXPECT_EQ(expectedPcValue, registers.pc);
}

TEST_F(InstructionTest, RND_VX_BYTE_SetsVxToRndAndByte) {
    const Opcode opcode = 0xC000 | (x << 8) | byte;

    constexpr uint8_t expectedRandomValue = 0x02;

    // Check that the mock RNG is working as expected
    ASSERT_EQ(0x37, random.generateNumber());

    instructions::RND_VX_BYTE(opcode, registers, random);

    // RND_VX_BYTE should bitwise AND a random number with the specified byte
    // and store the result in Vx
    EXPECT_EQ(expectedRandomValue, registers.v[x]);
}

TEST_F(InstructionTest, 
    DRW_VX_VY_NIBBLE_BlankScreen_DrawsSpriteOnScreenAndClearsVF) {
    // Vectorized representation of the sprite to draw on screen
    const std::vector<std::vector<uint8_t>> sprite = {
        {0, 1, 0, 1, 0, 1, 0, 1},
        {1, 0, 1 ,0 ,1 ,0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1}
    };

    const uint8_t n = sprite.size();
    const Opcode opcode = 0xD000 | (x << 8) | (y << 4) | n;

    // Load the sprite data into memory
    registers.i = 0x200;
    memory[registers.i] = 0b01010101;
    memory[registers.i + 1] = 0b10101010;
    memory[registers.i + 2] = 0b00000000;
    memory[registers.i + 3] = 0b11111111;

    instructions::DRW_VX_VY_NIBBLE(opcode, memory, registers, frame);

    // DRW_VX_VY_NIBBLE should draw the sprite from memory onto the frame
    for (int row = 0; row < sprite.size(); row++) {
        for (int col = 0; col < sprite.front().size(); col++) {
            int pixelLoc = Frame::WIDTH * row + col;
            EXPECT_EQ(sprite[row][col], frame[pixelLoc]);
        }
    }

    // Started with a blank screen, so DRW_VX_VY_NIBBLE should clear the 
    // collision flag VF
    EXPECT_EQ(0x00, registers.v[0xF]);
}

TEST_F(InstructionTest, 
    DRW_VX_VY_NIBBLE_RepeatDraw_ClearsSpriteFromScreenAndSetsVF) {
    // Vectorized representation of the sprite to draw on screen
    const std::vector<std::vector<uint8_t>> sprite = {
        {0, 1, 0, 1, 0, 1, 0, 1},
        {1, 0, 1 ,0 ,1 ,0, 1, 0},
        {0, 0, 0, 0, 0, 0, 0, 0},
        {1, 1, 1, 1, 1, 1, 1, 1}
    };

    const uint8_t n = sprite.size();
    const Opcode opcode = 0xD000 | (x << 8) | (y << 4) | n;

    // Load the sprite data into memory
    registers.i = 0x200;
    memory[registers.i] = 0b01010101;
    memory[registers.i + 1] = 0b10101010;
    memory[registers.i + 2] = 0b00000000;
    memory[registers.i + 3] = 0b11111111;

    // Repeating the draw instruction will toggle the sprite on and off
    instructions::DRW_VX_VY_NIBBLE(opcode, memory, registers, frame);
    instructions::DRW_VX_VY_NIBBLE(opcode, memory, registers, frame);

    // The second call to DRW_VX_VY_NIBBLE should clear the frame
    for (int i = 0; i < Frame::WIDTH * Frame::HEIGHT; i++) {
        EXPECT_EQ(false, frame[i]);
    }
    
    // The second draw call overlapped the first sprite, so DRW_VX_VY_NIBBLE 
    // should set the collision flag VF
    EXPECT_EQ(0x01, registers.v[0xF]);
}

TEST_F(InstructionTest, SKP_VX_VxPressed_SkipsInstruction) {
    const Opcode opcode = 0xE09E | (x << 8);

    registers.v[x] = 0xA;
    keypad[registers.v[x]] = true; // Set key Vx as pressed
    const uint16_t incrementedPcValue = registers.pc + 2;

    instructions::SKP_VX(opcode, registers, keypad);

    // Key Vx pressed, so SKP_VX should increment the program counter
    EXPECT_EQ(incrementedPcValue, registers.pc);
}

TEST_F(InstructionTest, SKP_VX_VxNotPressed_DoesNotSkipInstruction) {
    const Opcode opcode = 0xE09E | (x << 8);

    registers.v[x] = 0xB;
    keypad[registers.v[x]] = false; // Set key Vx as not pressed
    const uint16_t initialPcValue = registers.pc;

    instructions::SKP_VX(opcode, registers, keypad);

    // Key Vx not pressed, so SKP_VX should NOT increment the program counter
    EXPECT_EQ(initialPcValue, registers.pc);
}

TEST_F(InstructionTest, SKNP_VX_VxPressed_DoesNotSkipInstruction) {
    const Opcode opcode = 0xE0A1 | (x << 8);

    registers.v[x] = 0xC;
    keypad[registers.v[x]] = true; // Set key Vx as pressed
    const uint16_t initialPcValue = registers.pc;

    instructions::SKNP_VX(opcode, registers, keypad);

    // Key Vx pressed, so SKNP_VX should NOT increment the program counter
    EXPECT_EQ(initialPcValue, registers.pc);
}

TEST_F(InstructionTest, SKNP_VX_VxNotPressed_SkipsInstruction) {
    const Opcode opcode = 0xE0A1 | (x << 8);

    registers.v[x] = 0xD;
    keypad[registers.v[x]] = false; // Set key Vx as not pressed
    const uint16_t incrementedPcValue = registers.pc + 2;

    instructions::SKNP_VX(opcode, registers, keypad);

    // Key Vx not pressed, so SKNP_VX should increment the program counter
    EXPECT_EQ(incrementedPcValue, registers.pc);
}

TEST_F(InstructionTest, LD_VX_DT_SetsVxToDelayTimer) {
    const Opcode opcode = 0xF007 | (x << 8);

    registers.delayTimer = 0x42;

    instructions::LD_VX_DT(opcode, registers);

    // LD_VX_DY should set Vx to the delay timer value
    EXPECT_EQ(registers.delayTimer, registers.v[x]);
}

TEST_F(InstructionTest, LD_VX_K_NoKeyPressed_DecrementsProgramCounter) {
    const Opcode opcode = 0xF00A | (x << 8);

    registers.pc = 0x002;
    const uint16_t decrementedPcValue = registers.pc - 2;

    instructions::LD_VX_K(opcode, registers, keypad);

    // No key press detected, so LD_VX_K should decrement the program counter
    EXPECT_EQ(decrementedPcValue, registers.pc);
}

TEST_F(InstructionTest, LD_VX_K_KeyPressed_SetVxToPressedKey) {
    const Opcode opcode = 0xF00A | (x << 8);

    const uint8_t initialPcValue = registers.pc;
    constexpr uint8_t key = 0xE;
    keypad[key] = true; // Set key E as pressed

    instructions::LD_VX_K(opcode, registers, keypad);

    // Key press detected, so LD_VX_K should NOT decrement the program counter
    EXPECT_EQ(initialPcValue, registers.pc);

    // LD_VX_K should set Vx to the value of the pressed key
    EXPECT_EQ(key, registers.v[x]);
}

TEST_F(InstructionTest, LD_DT_VX_SetsDelayTimerToVx) {
    const Opcode opcode = 0xF015 | (x << 8);

    registers.v[x] = 0xF0;

    instructions::LD_DT_VX(opcode, registers);

    // LD_DT_VX should set the delay timer to Vx
    EXPECT_EQ(registers.v[x], registers.delayTimer);
}

TEST_F(InstructionTest, LD_ST_VX_SetsSoundTimerToVx) {
    const Opcode opcode = 0xF018 | (x << 8);

    registers.v[x] = 0xF0;

    instructions::LD_ST_VX(opcode, registers);

    // LD_ST_VX should set the sound timer to Vx
    EXPECT_EQ(registers.v[x], registers.soundTimer);
}

TEST_F(InstructionTest, ADD_I_VX_AddsVxToIndexRegister) {
    const Opcode opcode = 0xF01E | (x << 8);

    registers.v[x] = 0x42;
    registers.i = 0x619;
    constexpr uint16_t vxPlusI = 0x65B;

    instructions::ADD_I_VX(opcode, registers);

    // ADD_I_VX should add Vx to the index register and store the result in the 
    // index register
    EXPECT_EQ(vxPlusI, registers.i);
}

TEST_F(InstructionTest, LD_F_VX_SetsIndexRegisterToVxSpriteAddress) {
    const Opcode opcode = 0xF029 | (x << 8);

    registers.v[x] = 0xE;
    constexpr uint16_t startAddress = 0x050;
    constexpr int spriteSize = 5;
    constexpr uint16_t spriteAddress = 0x096;

    instructions::LD_F_VX(opcode, registers, startAddress, spriteSize);

    // LD_F_VX should set the index register to the address of the sprite for
    // the digit in Vx
    EXPECT_EQ(spriteAddress, registers.i);
}

TEST_F(InstructionTest, LD_B_VX_WritesVxBcdToMemory) {
    const Opcode opcode = 0xF033 | (x << 8);

    constexpr uint8_t hundredsDigit = 2;
    constexpr uint8_t tensDigit = 5;
    constexpr uint8_t onesDigit = 1;
    registers.v[x] = (100 * hundredsDigit) + (10 * tensDigit) + onesDigit;
    registers.i = 0x200;

    instructions::LD_B_VX(opcode, memory, registers);

    // LD_B_VX should store the BCD representation of Vx in memory
    EXPECT_EQ(hundredsDigit, memory[registers.i]);
    EXPECT_EQ(tensDigit, memory[registers.i + 1]);
    EXPECT_EQ(onesDigit, memory[registers.i + 2]);
}

TEST_F(InstructionTest, LD_I_VX_WritesRegistersToMemory) {
    const std::vector<uint8_t> data = {0x11, 0x22, 0x33, 0x44, 0x55};

    const uint16_t x = data.size() - 1;
    const Opcode opcode = 0xF055 | (x << 8);
    
    // Load the data into registers V0 through Vx
    for (int i = 0; i < data.size(); i++) {
        registers.v[i] = data[i];
    }

    registers.i = 0x200;
    
    instructions::LD_I_VX(opcode, memory, registers);

    // LD_I_VX should store the data from registers V0 through Vx in memory
    for (int i = 0; i <= x; i++) {
        EXPECT_EQ(registers.v[i], memory[registers.i + i]);
    }
}

TEST_F(InstructionTest, LD_VX_I_ReadsMemoryIntoRegisters) {
    const std::vector<uint8_t> data = {0x11, 0x22, 0x33, 0x44, 0x55};

    const uint16_t x = data.size() - 1;
    const Opcode opcode = 0xF065 | (x << 8);

    // Load the data into memory starting at the address in I
    registers.i = 0x200;
    for (int i = 0; i < data.size(); i++) {
        memory[registers.i + i] = data[i];
    }
    
    instructions::LD_VX_I(opcode, memory, registers);

    // LD_VX_I should store the data from memory in registers V0 through Vx
    for (int i = 0; i <= x; i++) {
        EXPECT_EQ(memory[registers.i + i], registers.v[i]);
    }
}