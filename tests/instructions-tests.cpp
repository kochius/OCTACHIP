#include <cstdint>
#include <gtest/gtest.h>

#include "fixtures.hpp"
#include "mocks.hpp"
#include "src/core/frame.hpp"
#include "src/core/instructions.hpp"
#include "src/core/opcode.hpp"
#include "src/core/types.hpp"

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

TEST_F(VxVyInstructionTest, SNE_Vx_Vy_VxEqualToVy_DoesNotSkipNextInstruction) {
    registers.v[x] = registers.v[y] = 0xFF;
    const uint16_t oldPcValue = registers.pc;

    instructions::SNE_Vx_Vy(opcode, registers);

    // SNE_Vx_Vy should NOT increment pc by 2 because Vx == Vy
    EXPECT_EQ(oldPcValue, registers.pc);
}

TEST_F(VxVyInstructionTest, SNE_Vx_Vy_VxNotEqualToVy_SkipsNextInstruction) {
    registers.v[x] = 0x01;
    registers.v[y] = 0xFF;
    const uint16_t newPcValue = registers.pc + 2;

    instructions::SNE_Vx_Vy(opcode, registers);

    // SNE_Vx_Vy should increment pc by 2 because Vx != Vy
    EXPECT_EQ(newPcValue, registers.pc);
}

TEST(InstructionTest, LD_I_addr_SetIEqualToAddress) {
    const uint16_t address = 0x0FFF;
    Opcode opcode = 0xA000 | address;
    Registers registers;

    instructions::LD_I_addr(opcode, registers);

    // LD_I_addr should set i equal to the specified address
    EXPECT_EQ(address, registers.i);
}

TEST(InstructionTest, JP_V0_addr_SetPcEqualToV0PlusAddress) {
    const uint16_t address = 0x0500;
    Opcode opcode = 0xB000 | address;
    Registers registers;

    registers.v[0] = 0x8;
    const uint16_t newPcValue = registers.v[0] + address;

    instructions::JP_V0_addr(opcode, registers);

    // JP_V0_addr should set pc equal to the specified address plus the offset
    // value in V0
    EXPECT_EQ(newPcValue, registers.pc);
}

TEST(InstructionTest, RND_Vx_byte_SetVxEqualToRndAndByte) {
    const uint16_t x = 0x8;
    const uint16_t byte = 0x10;
    Opcode opcode = 0xC000 | (x << 8) | byte;
    Registers registers;

    MockRandom random;
    const uint8_t result = random.generateNumber() & byte;

    instructions::RND_Vx_byte(opcode, registers, random);

    // RND_Vx_byte should AND a random number with the specified byte and store
    // the result in Vx
    EXPECT_EQ(result, registers.v[x]);
}

TEST(InstructionTest, DRW_Vx_Vy_nibble_FirstDraw_DrawsSpriteOntoScreen) {
    // create the sprite data
    constexpr int SPRITE_HEIGHT = 6;
    constexpr std::array<uint8_t, SPRITE_HEIGHT> sprite = {
        0b00000000,
        0b11111111,
        0b10000000,
        0b00000001,
        0b11001100,
        0b01100110
    };

    // load the sprite data into memory
    Memory memory;
    Registers registers;
    registers.i = 0x200;
    for (int offset = 0; offset < SPRITE_HEIGHT; offset++) {
        memory[registers.i + offset] = sprite[offset];
    }

    // set the starting location of the sprite data
    constexpr uint16_t x = 0x0;
    constexpr uint16_t y = 0x1;
    registers.v[x] = 32;
    registers.v[y] = 16;

    // construct the opcode and execute the instruction
    Opcode opcode = (x << 8) | (y << 4) | SPRITE_HEIGHT;
    Frame frame;
    instructions::DRW_Vx_Vy_nibble(opcode, memory, registers, frame);

    // DRW_Vx_Vy should draw the sprite to the screen
    for (int row = 0; row < SPRITE_HEIGHT; row++) {
        for (int col = 0; col < 8; col++) {
            bool expectedPixel = sprite[row] & (0b10000000 >> col);
            bool actualPixel = frame[(registers.v[y] + row) * Frame::WIDTH + 
                (registers.v[x] + col)];
            EXPECT_EQ(expectedPixel, actualPixel);
        }
    }
    // The VF flag should be cleared since there was nothing else on the screen
    EXPECT_EQ(0, registers.v[0xF]);
}

TEST(InstructionTest, DRW_Vx_Vy_nibble_RepeatDraw_ClearsSpriteFromScreen) {
    // create the sprite data
    constexpr int SPRITE_HEIGHT = 6;
    constexpr std::array<uint8_t, SPRITE_HEIGHT> sprite = {
        0b00000000,
        0b11111111,
        0b10000000,
        0b00000001,
        0b11001100,
        0b01100110
    };

    // load the sprite data into memory
    Memory memory;
    Registers registers;
    registers.i = 0x200;
    for (int offset = 0; offset < SPRITE_HEIGHT; offset++) {
        memory[registers.i + offset] = sprite[offset];
    }

    // set the starting location of the sprite data
    constexpr uint16_t x = 0x0;
    constexpr uint16_t y = 0x1;
    registers.v[x] = 32;
    registers.v[y] = 16;

    // construct the opcode and execute the instruction twice
    Opcode opcode = (x << 8) | (y << 4) | SPRITE_HEIGHT;
    Frame frame;
    instructions::DRW_Vx_Vy_nibble(opcode, memory, registers, frame);
    instructions::DRW_Vx_Vy_nibble(opcode, memory, registers, frame);

    // Drawing the sprite twice should toggle the pixels off, so the screen
    // should be cleared
    for (size_t i = 0; i < Frame::WIDTH * Frame::HEIGHT; i++) {
        EXPECT_EQ(false, frame[i]);
    }
    // Toggled the pixels off, so the VF flag register should be set
    EXPECT_EQ(1, registers.v[0xF]);
}

TEST(InstructionTest, SKP_Vx_VxPressed_SkipsNextInstruction) {
    const uint16_t x = 0x2;
    Opcode opcode = 0xE09E | (x << 8);

    Registers registers;
    registers.v[x] = 0x8;
    const uint16_t newPcValue = registers.pc + 2;

    Keypad keypad;
    keypad[registers.v[x]] = true;

    instructions::SKP_Vx(opcode, registers, keypad);

    EXPECT_EQ(newPcValue, registers.pc);
}

TEST(InstructionTest, SKP_Vx_VxNotPressed_DoesNotSkipNextInstruction) {
    const uint16_t x = 0x2;
    Opcode opcode = 0xE09E | (x << 8);

    Registers registers;
    registers.v[x] = 0x8;
    const uint16_t oldPcValue = registers.pc;

    Keypad keypad;
    keypad[registers.v[x]] = false;

    instructions::SKP_Vx(opcode, registers, keypad);

    EXPECT_EQ(oldPcValue, registers.pc);
}

TEST(InstructionTest, SKNP_Vx_VxPressed_DoesNotSkipNextInstruction) {
    const uint16_t x = 0x2;
    Opcode opcode = 0xE09E | (x << 8);

    Registers registers;
    registers.v[x] = 0x8;
    const uint16_t oldPcValue = registers.pc;

    Keypad keypad;
    keypad[registers.v[x]] = true;

    instructions::SKNP_Vx(opcode, registers, keypad);

    EXPECT_EQ(oldPcValue, registers.pc);
}

TEST(InstructionTest, SKNP_Vx_VxNotPressed_SkipsNextInstruction) {
    const uint16_t x = 0x2;
    Opcode opcode = 0xE09E | (x << 8);

    Registers registers;
    registers.v[x] = 0x8;
    const uint16_t newPcValue = registers.pc + 2;

    Keypad keypad;
    keypad[registers.v[x]] = false;

    instructions::SKNP_Vx(opcode, registers, keypad);

    EXPECT_EQ(newPcValue, registers.pc);
}

TEST(InstructionTest, LD_Vx_DT_SetsVxEqualToDelayTimer) {
    const uint16_t x = 0x8;
    Opcode opcode = 0xF007 | (x << 8);

    Registers registers;
    registers.delayTimer = 0x42;

    instructions::LD_Vx_DT(opcode, registers);

    EXPECT_EQ(registers.delayTimer, registers.v[x]);
}

TEST(InstructionTest, LD_Vx_K_NoKeyPressed_DecrementsProgramCounterByTwo) {
    const uint16_t x = 0x8;
    Opcode opcode = 0xF00A | (x << 8);

    Registers registers;
    registers.pc = 2;

    Keypad keypad{};

    instructions::LD_Vx_K(opcode, registers, keypad);

    EXPECT_EQ(0, registers.pc);
}

TEST(InstructionTest, LD_Vx_K_KeyPressed_SetsVxEqualToKey) {
    const uint16_t x = 0x8;
    Opcode opcode = 0xF00A | (x << 8);

    Registers registers;
    registers.pc = 2;

    Keypad keypad{};
    const int key = 0xA;
    keypad[key] = true;

    instructions::LD_Vx_K(opcode, registers, keypad);

    EXPECT_EQ(key, registers.v[x]);
    EXPECT_EQ(2, registers.pc);
}

TEST(InstructionTest, LD_DT_Vx_SetsDelayTimerEqualToVx) {
    const uint16_t x = 0x8;
    Opcode opcode = 0xF015 | (x << 8);

    Registers registers;
    registers.v[x] = 0x42;

    instructions::LD_DT_Vx(opcode, registers);

    EXPECT_EQ(registers.v[x], registers.delayTimer);
}

TEST(InstructionTest, LD_ST_Vx_SetsSoundTimerEqualToVx) {
    const uint16_t x = 0x8;
    Opcode opcode = 0xF018 | (x << 8);

    Registers registers;
    registers.v[x] = 0x42;

    instructions::LD_ST_Vx(opcode, registers);

    EXPECT_EQ(registers.v[x], registers.soundTimer);
}

TEST(InstructionTest, ADD_I_Vx_SetsIndexRegistersEqualToIndexRegistersPlusVx) {
    const uint16_t x = 0x8;
    Opcode opcode = 0xF01E | (x << 8);

    Registers registers;
    registers.i = 0x32;
    registers.v[x] = 0x29;
    const uint16_t iPlusVx = registers.i + registers.v[x];

    instructions::ADD_I_Vx(opcode, registers);

    EXPECT_EQ(iPlusVx, registers.i);
}

TEST(InstructionTest, LD_F_Vx) {
    const int FONT_START_ADDRESS = 0x00;
    const int FONT_CHAR_SIZE = 0x05;

    const uint16_t x = 0x8;
    Opcode opcode = 0xF029 | (x << 8);

    Registers registers;
    registers.v[x] = 0x06;
    const uint16_t characterLocation = FONT_START_ADDRESS + registers.v[x] * FONT_CHAR_SIZE;

    instructions::LD_F_Vx(opcode, registers, FONT_START_ADDRESS, FONT_CHAR_SIZE);

    EXPECT_EQ(characterLocation, registers.i);
}

TEST(InstructionTest, LD_B_Vx_StoresBcdOfVxInMemory) {
    const uint16_t x = 0x8;
    Opcode opcode = 0xF033 | (x << 8);

    Registers registers;
    registers.v[x] = 62;
    registers.i = 0;

    Memory memory;

    instructions::LD_B_Vx(opcode, memory, registers);

    EXPECT_EQ(0, memory[registers.i]);
    EXPECT_EQ(6, memory[registers.i + 1]);
    EXPECT_EQ(2, memory[registers.i + 2]);
}

TEST(InstructionTest, LD_I_Vx_StoresRegistersV0ToVxInMemoryStartingAtI) {
    const uint16_t x = 5;
    Opcode opcode = 0xF055 | (x << 8);

    std::array<uint8_t, x + 1> data = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};
    Registers registers;
    registers.i = 0;

    for (int i = 0; i <= x; i++) {
        registers.v[i] = data[i];
    }
    
    Memory memory;
    instructions::LD_I_Vx(opcode, memory, registers);

    for (int i = 0; i <= x; i++) {
        EXPECT_EQ(data[i], memory[registers.i + i]);
    }
}

TEST(InstructionTest, LD_Vx_I_StoresMemoryAtIInRegistersV0ToVx) {
    const uint16_t x = 5;
    Opcode opcode = 0xF065 | (x << 8);

    std::array<uint8_t, x + 1> data = {0x11, 0x22, 0x33, 0x44, 0x55, 0x66};

    Registers registers;
    registers.i = 0;

    Memory memory;
    for (int i = 0; i <= x; i++) {
        memory[registers.i + i] = data[i];
    }
    
    instructions::LD_Vx_I(opcode, memory, registers);

    for (int i = 0; i <= x; i++) {
        EXPECT_EQ(data[i], registers.v[i]);
    }
}