#pragma once

#include "frame.hpp"
#include "opcode.hpp"
#include "types.hpp"

namespace CHIP8 {

void CLS(Frame& frame) {
    frame.clearFrame();
}

void JP_addr(const Opcode& opcode, Registers& registers) {
    registers.pc = opcode.address();
}

void LD_Vx_byte(const Opcode& opcode, Registers& registers) {
    registers.v[opcode.x()] = opcode.byte();
}

void ADD_Vx_byte(const Opcode& opcode, Registers& registers) {
    registers.v[opcode.x()] += opcode.byte();
}

void LD_I_addr(const Opcode& opcode, Registers& registers) {
    registers.i = opcode.address();
}

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
