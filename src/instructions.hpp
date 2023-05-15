#pragma once

#include "frame.hpp"
#include "opcode.hpp"
#include "random.hpp"
#include "types.hpp"

namespace CHIP8 {

/**
 * 00E0 - Clear the display.
 */
void CLS(Frame& frame) {
    frame.clearFrame();
}

/**
 * 00EE - Return from a subroutine.
 * 
 * The interpreter sets the program counter to the address at the top of the 
 * stack, then subtracts 1 from the stack pointer.
 */
void RET(Registers& registers, Stack& stack) {
    registers.pc = stack[--registers.sp];
}

/**
 * 1nnn - Jump to location nnn.
 * 
 * The interpreter sets the program counter to nnn.
 */
void JP_addr(const Opcode& opcode, Registers& registers) {
    registers.pc = opcode.address();
}

/**
 * 2nnn - Call subroutine at nnn.
 * 
 * The interpreter increments the stack pointer, then puts the current PC on 
 * the top of the stack. The PC is then set to nnn.
 */
void CALL_addr(const Opcode& opcode, Registers& registers, Stack& stack) {
    stack[registers.sp++] = registers.pc;
    registers.pc = opcode.address();
}

/**
 * 3xkk - Skip next instruction if Vx = kk.
 * 
 * The interpreter compares register Vx to kk, and if they are equal, increments 
 * the program counter by 2.
 */
void SE_Vx_byte(const Opcode& opcode, Registers& registers) {
    if (registers.v[opcode.x()] == opcode.byte()) {
        registers.pc += 2;
    }
}

/**
 * 4xkk - Skip next instruction if Vx != kk.
 * 
 * The interpreter compares register Vx to kk, and if they are not equal, 
 * increments the program counter by 2.
 */
void SNE_Vx_byte(const Opcode& opcode, Registers& registers) {
    if (registers.v[opcode.x()] != opcode.byte()) {
        registers.pc += 2;
    }
}

/**
 * 5xy0 - Skip next instruction if Vx = Vy.
 * 
 * The interpreter compares register Vx to register Vy, and if they are equal, 
 * increments the program counter by 2.
 */
void SE_Vx_Vy(const Opcode& opcode, Registers& registers) {
    if (registers.v[opcode.x()] == registers.v[opcode.y()]) {
        registers.pc += 2;
    }
}

/**
 * 6xkk - Set Vx = kk.
 * 
 * The interpreter puts the value kk into register Vx.
 */
void LD_Vx_byte(const Opcode& opcode, Registers& registers) {
    registers.v[opcode.x()] = opcode.byte();
}

/**
 * 7xkk - Set Vx = Vx + kk.
 * 
 * Adds the value kk to the value of register Vx, then stores the result in Vx.
 */
void ADD_Vx_byte(const Opcode& opcode, Registers& registers) {
    registers.v[opcode.x()] += opcode.byte();
}

/**
 * 8xy0 - Set Vx = Vy.
 * 
 * Stores the value of register Vy in register Vx.
 */
void LD_Vx_Vy(const Opcode& opcode, Registers& registers) {
    registers.v[opcode.x()] = registers.v[opcode.y()];
}

/**
 * 8xy1 - Set Vx = Vx OR Vy.
 * 
 * Performs a bitwise OR on the values of Vx and Vy, then stores the result in 
 * Vx. A bitwise OR compares the corrseponding bits from two values, and if 
 * either bit is 1, then the same bit in the result is also 1. Otherwise, it is 
 * 0.
 */
void OR_Vx_Vy(const Opcode& opcode, Registers& registers) {
    registers.v[opcode.x()] |= registers.v[opcode.y()];
}

/**
 * 8xy2 - Set Vx = Vx AND Vy.
 * 
 * Performs a bitwise AND on the values of Vx and Vy, then stores the result in 
 * Vx. A bitwise AND compares the corrseponding bits from two values, and if 
 * both bits are 1, then the same bit in the result is also 1. Otherwise, it is 
 * 0.
 */
void AND_Vx_Vy(const Opcode& opcode, Registers& registers) {
    registers.v[opcode.x()] &= registers.v[opcode.y()];
}

/**
 * 8xy3 - Set Vx = Vx XOR Vy.
 * 
 * Performs a bitwise exclusive OR on the values of Vx and Vy, then stores the 
 * result in Vx. An exclusive OR compares the corrseponding bits from two 
 * values, and if the bits are not both the same, then the corresponding bit in 
 * the result is set to 1. Otherwise, it is 0.
 */
void XOR_Vx_Vy(const Opcode& opcode, Registers& registers) {
    registers.v[opcode.x()] ^= registers.v[opcode.y()];
}

/**
 * 8xy4 - Set Vx = Vx + Vy, set VF = carry.
 * 
 * The values of Vx and Vy are added together. If the result is greater than 8 
 * bits (i.e., > 255,) VF is set to 1, otherwise 0. Only the lowest 8 bits of 
 * the result are kept, and stored in Vx.
 */
void ADD_Vx_Vy(const Opcode& opcode, Registers& registers) {
    uint16_t sum = registers.v[opcode.x()] + registers.v[opcode.y()];
    registers.v[opcode.x()] = sum & 0xFF;
    registers.v[0x0F] = sum > 0xFF ? 1 : 0;
}

/**
 * 8xy5 - Set Vx = Vx - Vy, set VF = NOT borrow.
 * 
 * If Vx >= Vy, then VF is set to 1, otherwise 0. Then Vy is subtracted from Vx, 
 * and the results stored in Vx.
 */
void SUB_Vx_Vy(const Opcode& opcode, Registers& registers) {
    bool borrow = registers.v[opcode.x()] < registers.v[opcode.y()];
    registers.v[opcode.x()] -= registers.v[opcode.y()];
    registers.v[0x0F] = !borrow? 1 : 0;
}

/**
 * 8xy6 - Set Vx = Vx SHR 1.
 * 
 * If the least-significant bit of Vx is 1, then VF is set to 1, otherwise 0. 
 * Then Vx is divided by 2.
 * 
 * TODO: Implement configurable quirks for this instruction
 */
void SHR_Vx_Vy(const Opcode& opcode, Registers& registers) {
    registers.v[0x0F] = registers.v[opcode.x()] & 0x01;
    registers.v[opcode.x()] >>= 1;
}

/**
 * 8xy7 - Set Vx = Vy - Vx, set VF = NOT borrow.
 * 
 * If Vy >= Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, 
 * and the results stored in Vx.
 */
void SUBN_Vx_Vy(const Opcode& opcode, Registers& registers) {
    bool borrow = registers.v[opcode.y()] < registers.v[opcode.x()];
    registers.v[opcode.x()] = registers.v[opcode.y()] - registers.v[opcode.x()];
    registers.v[0x0F] =  !borrow? 1 : 0;
}

/**
 * 8xyE - Set Vx = Vx SHL 1.
 * 
 * If the most-significant bit of Vx is 1, then VF is set to 1, otherwise to 0. 
 * Then Vx is multiplied by 2.
 * 
 * TODO: Implement configurable quirks for this instruction
 */
void SHL_Vx_Vy(const Opcode& opcode, Registers& registers) {
    registers.v[0x0F] = registers.v[opcode.x()] >> 7;
    registers.v[opcode.x()] <<= 1;
}

/**
 * 9xy0 - Skip next instruction if Vx != Vy.
 * 
 * The values of Vx and Vy are compared, and if they are not equal, the program 
 * counter is increased by 2.
 */
void SNE_Vx_Vy(const Opcode& opcode, Registers& registers) {
    if (registers.v[opcode.x()] != registers.v[opcode.y()]) {
        registers.pc += 2;
    }
}

/**
 * Annn - Set I = nnn.
 * 
 * The value of register I is set to nnn.
 */
void LD_I_addr(const Opcode& opcode, Registers& registers) {
    registers.i = opcode.address();
}

/**
 * Bnnn - Jump to location nnn + V0.
 * 
 * The program counter is set to nnn plus the value of V0.
 * 
 * TODO: Implement configurable quirks for this instruction
 */
void JP_V0_addr(const Opcode& opcode, Registers& registers) {
    registers.pc = opcode.address() + registers.v[0];
}

/**
 * Cxkk - Set Vx = random byte AND kk.
 * 
 * The interpreter generates a random number from 0 to 255, which is then ANDed 
 * with the value kk. The results are stored in Vx.
 */
void RND_Vx_byte(const Opcode& opcode, Registers& registers, Random& random) {
    registers.v[opcode.x()] = random.generateNumber() & opcode.byte();
}

/**
 * Dxyn - Display n-byte sprite starting at memory location I at (Vx, Vy), set 
 *        VF = collision.
 * 
 * The interpreter reads n bytes from memory, starting at the address stored in 
 * I. These bytes are then displayed as sprites on screen at coordinates 
 * (Vx, Vy). Sprites are XORed onto the existing screen. If this causes any 
 * pixels to be erased, VF is set to 1, otherwise it is set to 0. If the sprite 
 * is positioned so part of it is outside the coordinates of the display, it 
 * wraps around to the opposite side of the screen.
 */
void DRW_Vx_Vy_nibble(const Opcode& opcode, const Memory& memory, 
    Registers& registers, Frame& frame) {
    int xPos = registers.v[opcode.x()] % Frame::WIDTH;
    int yPos = registers.v[opcode.y()] % Frame::HEIGHT;
    registers.v[0x0F] = 0;
    int height = opcode.nibble();
    for (int row = 0; row < height; row++) {
        uint8_t spriteRow = memory[registers.i + row];
        for (int col = 0; col < 8; col++) {
            if (xPos + col >= 0 && xPos + col <= Frame::WIDTH &&
                yPos + row >= 0 && yPos + row <= Frame::HEIGHT &&
                spriteRow & (0b10000000 >> col)) {
                int pixel = ((yPos + row) * Frame::WIDTH) + (xPos + col);
                if (frame[pixel]) {
                    registers.v[0x0F] = 1;
                }
                frame[pixel] ^= 1;
            }
        }
    }
}

}
