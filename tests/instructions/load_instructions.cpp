#include <cstdint>
#include <gtest/gtest.h>

#include "fixtures/instruction_test.hpp"
#include "core/instructions.hpp"
#include "core/opcode.hpp"
#include "core/types.hpp"

using namespace CHIP8;

TEST_F(InstructionTest, LD_VX_BYTE_SetsVxToByte) {
    const Opcode opcode = 0x6000 | (x << 8) | byte;

    instructions::LD_VX_BYTE(opcode, registers);

    // LD_VX_BYTE should set Vx to the specified byte
    EXPECT_EQ(byte, registers.v[x]);
}

TEST_F(InstructionTest, LD_VX_VY_SetsVxToVy) {
    const Opcode opcode = 0x8000 | (x << 8) | (y << 4);

    registers.v[y] = 0x23;

    instructions::LD_VX_VY(opcode, registers);

    // LD_VX_VY should set Vx to Vy
    EXPECT_EQ(registers.v[y], registers.v[x]);
}

TEST_F(InstructionTest, LD_I_ADDR_SetsIndexRegisterToAddress) {
    const Opcode opcode = 0xA000 | address;

    instructions::LD_I_ADDR(opcode, registers);

    // LD_I_ADDR should set the index register to the specified address
    EXPECT_EQ(address, registers.i);
}

TEST_F(InstructionTest, LD_VX_DT_SetsVxToDelayTimer) {
    const Opcode opcode = 0xF007 | (x << 8);

    registers.delayTimer = 0x42;

    instructions::LD_VX_DT(opcode, registers);

    // LD_VX_DY should set Vx to the delay timer value
    EXPECT_EQ(registers.delayTimer, registers.v[x]);
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

TEST_F(InstructionTest, LD_B_VX_MemoryInRange_WritesVxBcdToMemory) {
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

TEST_F(InstructionTest, LD_B_VX_MemoryOutOfRange_ThrowsException) {
    const Opcode opcode = 0xF033 | (x << 8);

    registers.i = MEMORY_SIZE - 1;

    // LD_B_VX should attempt to write to memory addresses I to I + 2
    // I already points to the last memory address, so LD_B_VX should throw an 
    // exception
    EXPECT_THROW(instructions::LD_B_VX(opcode, memory, registers), 
        std::out_of_range);
}

TEST_F(InstructionTest, LD_I_VX_MemoryInRange_WritesRegistersToMemory) {
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

TEST_F(InstructionTest, LD_I_VX_MemoryOutOfRange_ThrowsException) {
    constexpr uint16_t x = 0xF;
    const Opcode opcode = 0xF055 | (x << 8);

    registers.i = MEMORY_SIZE - 1;
    
    // LD_I_VX should attempt to write to memory addresses I to I + x
    // I already points to the last memory address, so LD_I_VX should throw an 
    // exception
    EXPECT_THROW(instructions::LD_I_VX(opcode, memory, registers), 
        std::out_of_range);
}

TEST_F(InstructionTest, LD_VX_I_MemoryInRange_ReadsMemoryIntoRegisters) {
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

TEST_F(InstructionTest, LD_VX_I_MemoryOutOfRange_ThrowsException) {
    constexpr uint16_t x = 0xF;
    const Opcode opcode = 0xF065 | (x << 8);

    registers.i = MEMORY_SIZE - 1;
    
    // LD_VX_I should attempt to read from memory addresses I to I + x
    // I already points to the last memory address, so LD_VX_I should throw an 
    // exception
    EXPECT_THROW(instructions::LD_VX_I(opcode, memory, registers), 
        std::out_of_range);
}