#include <cstdint>
#include <gtest/gtest.h>

#include "fixtures/instruction_test.hpp"
#include "core/instructions.hpp"
#include "core/opcode.hpp"
#include "core/types.hpp"

using namespace OCTACHIP;

TEST_F(InstructionTest, CLS_ClearsFrame) {
    // Turn pixels on
    frame[0] = true;
    frame[1] = true;
    frame[2] = true;

    instructions::CLS(frame);

    // CLS should turn off all pixels
    for (int i = 0; i < FRAME_WIDTH * FRAME_HEIGHT; i++) {
        EXPECT_EQ(false, frame[i]);
    }
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

    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
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
    for (unsigned int row = 0; row < sprite.size(); row++) {
        for (unsigned int col = 0; col < sprite.front().size(); col++) {
            int pixelLoc = FRAME_WIDTH * row + col;
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

    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
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
    for (int i = 0; i < FRAME_WIDTH * FRAME_HEIGHT; i++) {
        EXPECT_EQ(false, frame[i]);
    }
    
    // The second draw call overlapped the first sprite, so DRW_VX_VY_NIBBLE 
    // should set the collision flag VF
    EXPECT_EQ(0x01, registers.v[0xF]);
}

TEST_F(InstructionTest, DRW_VX_VY_NIBBLE_MemoryOutOfRange_ThrowsException) {
    const uint16_t x = 0x0;
    const uint16_t y = 0xA;
    const uint8_t n = 0xF;
    const Opcode opcode = 0xD000 | (x << 8) | (y << 4) | n;

    registers.i = MEMORY_SIZE - 1;

    // DRW_VX_VY_NIBBLE should attempt read from memory addresses I to I + n - 1
    // I already points to the last memory address, so DRW_VX_VY_NIBBLE should 
    // throw an exception
    EXPECT_THROW(
        instructions::DRW_VX_VY_NIBBLE(opcode, memory, registers, frame), 
        std::out_of_range
    );
}

TEST_F(InstructionTest, SKP_VX_VxPressed_SkipsInstruction) {
    const uint16_t x = 0x0;
    const Opcode opcode = 0xE09E | (x << 8);

    registers.v[x] = 0xA;
    keypad[registers.v[x]] = true; // Set key Vx as pressed
    const uint16_t incrementedPcValue = registers.pc + 2;

    instructions::SKP_VX(opcode, registers, keypad);

    // Key Vx pressed, so SKP_VX should increment the program counter
    EXPECT_EQ(incrementedPcValue, registers.pc);
}

TEST_F(InstructionTest, SKP_VX_VxNotPressed_DoesNotSkipInstruction) {
    const uint16_t x = 0x0;
    const Opcode opcode = 0xE09E | (x << 8);

    registers.v[x] = 0xB;
    keypad[registers.v[x]] = false; // Set key Vx as not pressed
    const uint16_t initialPcValue = registers.pc;

    instructions::SKP_VX(opcode, registers, keypad);

    // Key Vx not pressed, so SKP_VX should NOT increment the program counter
    EXPECT_EQ(initialPcValue, registers.pc);
}

TEST_F(InstructionTest, SKNP_VX_VxPressed_DoesNotSkipInstruction) {
    const uint16_t x = 0x0;
    const Opcode opcode = 0xE0A1 | (x << 8);

    registers.v[x] = 0xC;
    keypad[registers.v[x]] = true; // Set key Vx as pressed
    const uint16_t initialPcValue = registers.pc;

    instructions::SKNP_VX(opcode, registers, keypad);

    // Key Vx pressed, so SKNP_VX should NOT increment the program counter
    EXPECT_EQ(initialPcValue, registers.pc);
}

TEST_F(InstructionTest, SKNP_VX_VxNotPressed_SkipsInstruction) {
    const uint16_t x = 0x0;
    const Opcode opcode = 0xE0A1 | (x << 8);

    registers.v[x] = 0xD;
    keypad[registers.v[x]] = false; // Set key Vx as not pressed
    const uint16_t incrementedPcValue = registers.pc + 2;

    instructions::SKNP_VX(opcode, registers, keypad);

    // Key Vx not pressed, so SKNP_VX should increment the program counter
    EXPECT_EQ(incrementedPcValue, registers.pc);
}

TEST_F(InstructionTest, LD_VX_K_NoKeyPressed_DecrementsProgramCounter) {
    const uint16_t x = 0x0;
    const Opcode opcode = 0xF00A | (x << 8);

    registers.pc = 0x002;
    const uint16_t decrementedPcValue = registers.pc - 2;

    instructions::LD_VX_K(opcode, registers, keypad);

    // No key press detected, so LD_VX_K should decrement the program counter
    EXPECT_EQ(decrementedPcValue, registers.pc);
}

TEST_F(InstructionTest, LD_VX_K_KeyPressed_SetVxToPressedKey) {
    const uint16_t x = 0x0;
    const Opcode opcode = 0xF00A | (x << 8);

    const uint8_t initialPcValue = registers.pc;
    const uint8_t key = 0xE;
    keypad[key] = true; // Set key E as pressed

    instructions::LD_VX_K(opcode, registers, keypad);

    // Key press detected, so LD_VX_K should NOT decrement the program counter
    EXPECT_EQ(initialPcValue, registers.pc);

    // LD_VX_K should set Vx to the value of the pressed key
    EXPECT_EQ(key, registers.v[x]);
}