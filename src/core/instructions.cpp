#include <cstdint>
#include <stdexcept>

#include "instructions.hpp"

using namespace CHIP8;

/**
 * 00E0 - Clear the display.
 */
void instructions::CLS(Frame& frame) {
    frame.clearFrame();
}

/**
 * 00EE - Return from a subroutine.
 * 
 * The interpreter sets the program counter to the address at the top of the 
 * stack, then subtracts 1 from the stack pointer.
 */
void instructions::RET(Registers& registers, Stack& stack) {
    if (registers.sp <= 0) {
        throw std::runtime_error("RET: Attempted to return from a subroutine, "
            "but the stack is empty");
    }
    registers.pc = stack[--registers.sp];
}

/**
 * 1nnn - Jump to location nnn.
 * 
 * The interpreter sets the program counter to nnn.
 */
void instructions::JP_addr(const Opcode& opcode, Registers& registers) {
    registers.pc = opcode.address();
}

/**
 * 2nnn - Call subroutine at nnn.
 * 
 * The interpreter increments the stack pointer, then puts the current PC on 
 * the top of the stack. The PC is then set to nnn.
 */
void instructions::CALL_addr(const Opcode& opcode, Registers& registers, 
    Stack& stack) {
    if (registers.sp >= STACK_SIZE) {
        throw std::runtime_error("CALL_addr: Stack overflow error");
    }
    stack[registers.sp++] = registers.pc;
    registers.pc = opcode.address();
}

/**
 * 3xkk - Skip next instruction if Vx = kk.
 * 
 * The interpreter compares register Vx to kk, and if they are equal, increments 
 * the program counter by 2.
 */
void instructions::SE_Vx_byte(const Opcode& opcode, Registers& registers) {
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
void instructions::SNE_Vx_byte(const Opcode& opcode, Registers& registers) {
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
void instructions::SE_Vx_Vy(const Opcode& opcode, Registers& registers) {
    if (registers.v[opcode.x()] == registers.v[opcode.y()]) {
        registers.pc += 2;
    }
}

/**
 * 6xkk - Set Vx = kk.
 * 
 * The interpreter puts the value kk into register Vx.
 */
void instructions::LD_Vx_byte(const Opcode& opcode, Registers& registers) {
    registers.v[opcode.x()] = opcode.byte();
}

/**
 * 7xkk - Set Vx = Vx + kk.
 * 
 * Adds the value kk to the value of register Vx, then stores the result in Vx.
 */
void instructions::ADD_Vx_byte(const Opcode& opcode, Registers& registers) {
    registers.v[opcode.x()] += opcode.byte();
}

/**
 * 8xy0 - Set Vx = Vy.
 * 
 * Stores the value of register Vy in register Vx.
 */
void instructions::LD_Vx_Vy(const Opcode& opcode, Registers& registers) {
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
void instructions::OR_Vx_Vy(const Opcode& opcode, Registers& registers) {
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
void instructions::AND_Vx_Vy(const Opcode& opcode, Registers& registers) {
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
void instructions::XOR_Vx_Vy(const Opcode& opcode, Registers& registers) {
    registers.v[opcode.x()] ^= registers.v[opcode.y()];
}

/**
 * 8xy4 - Set Vx = Vx + Vy, set VF = carry.
 * 
 * The values of Vx and Vy are added together. If the result is greater than 8 
 * bits (i.e., > 255,) VF is set to 1, otherwise 0. Only the lowest 8 bits of 
 * the result are kept, and stored in Vx.
 */
void instructions::ADD_Vx_Vy(const Opcode& opcode, Registers& registers) {
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
void instructions::SUB_Vx_Vy(const Opcode& opcode, Registers& registers) {
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
void instructions::SHR_Vx_Vy(const Opcode& opcode, Registers& registers) {
    registers.v[0x0F] = registers.v[opcode.x()] & 0x01;
    registers.v[opcode.x()] >>= 1;
}

/**
 * 8xy7 - Set Vx = Vy - Vx, set VF = NOT borrow.
 * 
 * If Vy >= Vx, then VF is set to 1, otherwise 0. Then Vx is subtracted from Vy, 
 * and the results stored in Vx.
 */
void instructions::SUBN_Vx_Vy(const Opcode& opcode, Registers& registers) {
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
void instructions::SHL_Vx_Vy(const Opcode& opcode, Registers& registers) {
    registers.v[0x0F] = registers.v[opcode.x()] >> 7;
    registers.v[opcode.x()] <<= 1;
}

/**
 * 9xy0 - Skip next instruction if Vx != Vy.
 * 
 * The values of Vx and Vy are compared, and if they are not equal, the program 
 * counter is increased by 2.
 */
void instructions::SNE_Vx_Vy(const Opcode& opcode, Registers& registers) {
    if (registers.v[opcode.x()] != registers.v[opcode.y()]) {
        registers.pc += 2;
    }
}

/**
 * Annn - Set I = nnn.
 * 
 * The value of register I is set to nnn.
 */
void instructions::LD_I_addr(const Opcode& opcode, Registers& registers) {
    registers.i = opcode.address();
}

/**
 * Bnnn - Jump to location nnn + V0.
 * 
 * The program counter is set to nnn plus the value of V0.
 * 
 * TODO: Implement configurable quirks for this instruction
 */
void instructions::JP_V0_addr(const Opcode& opcode, Registers& registers) {
    registers.pc = opcode.address() + registers.v[0];
}

/**
 * Cxkk - Set Vx = random byte AND kk.
 * 
 * The interpreter generates a random number from 0 to 255, which is then ANDed 
 * with the value kk. The results are stored in Vx.
 */
void instructions::RND_Vx_byte(const Opcode& opcode, Registers& registers, 
    Random& random) {
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
void instructions::DRW_Vx_Vy_nibble(const Opcode& opcode, const Memory& memory, 
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

/**
 * Ex9E - Skip next instruction if key with the value of Vx is pressed.
 * 
 * Checks the keyboard, and if the key corresponding to the value of Vx is 
 * currently in the down position, PC is increased by 2.
 */
void instructions::SKP_Vx(const Opcode& opcode, Registers& registers, const 
    Keypad& keypad) {
    bool isPressed = keypad[registers.v[opcode.x()]];
    if (isPressed) {
        registers.pc += 2;
    }
}

/**
 * ExA1 - Skip next instruction if key with the value of Vx is not pressed.
 * 
 * Checks the keyboard, and if the key corresponding to the value of Vx is 
 * currently in the up position, PC is increased by 2.
 */
void instructions::SKNP_Vx(const Opcode& opcode, Registers& registers, const 
    Keypad& keypad) {
    bool isPressed = keypad[registers.v[opcode.x()]];
    if (!isPressed) {
        registers.pc += 2;
    }
}

/**
 * Fx07 - Set Vx = delay timer value.
 * 
 * The value of DT is placed into Vx.
 */
void instructions::LD_Vx_DT(const Opcode& opcode, Registers& registers) {
    registers.v[opcode.x()] = registers.delayTimer;
}

/**
 * Fx0A - Wait for a key press, store the value of the key in Vx.
 * 
 * All execution stops until a key is pressed, then the value of that key is 
 * stored in Vx.
 * 
 * TODO: Implement configurable quirks for this instruction
 */
void instructions::LD_Vx_K(const Opcode& opcode, Registers& registers, const 
    Keypad& keypad) {
    for (size_t keyValue = 0; keyValue < keypad.size(); keyValue++) {
        if (keypad[keyValue]) {
            registers.v[opcode.x()] = keyValue;
            return;
        }
    }
    registers.pc -= 2;
}

/**
 * Fx15 - Set delay timer = Vx.
 * 
 * DT is set equal to the value of Vx.
 */
void instructions::LD_DT_Vx(const Opcode& opcode, Registers& registers) {
    registers.delayTimer = registers.v[opcode.x()];
}

/**
 * Fx18 - Set sound timer = Vx.
 * 
 * ST is set equal to the value of Vx.
 */
void instructions::LD_ST_Vx(const Opcode& opcode, Registers& registers) {
    registers.soundTimer = registers.v[opcode.x()];
}

/**
 * Fx1E - Set I = I + Vx.
 * 
 * The values of I and Vx are added, and the results are stored in I.
 * 
 * TODO: Implement configurable quirks for this instruction
 */
void instructions::ADD_I_Vx(const Opcode& opcode, Registers& registers) {
    registers.i += registers.v[opcode.x()];
}

/**
 * Fx29 - Set I = location of sprite for digit Vx.
 * 
 * The value of I is set to the location for the hexadecimal sprite 
 * corresponding to the value of Vx.
 */
void instructions::LD_F_Vx(const Opcode& opcode, Registers& registers, 
    const int fontStartaddress, const int fontCharSize) {
    uint8_t character = registers.v[opcode.x()] & 0x0F;
    registers.i = fontStartaddress + fontCharSize * character;
}

/**
 * Fx33 - Store BCD representation of Vx in memory locations I, I+1, and I+2.
 * 
 * The interpreter takes the decimal value of Vx, and places the hundreds digit 
 * in memory at location in I, the tens digit at location I+1, and the ones 
 * digit at location I+2.
 */
void instructions::LD_B_Vx(const Opcode& opcode, Memory& memory, Registers& 
    registers) {
    uint8_t value = registers.v[opcode.x()];
    for (int j = 2; j >= 0; j--) {
        memory[registers.i + j] = value % 10;
        value /= 10;
    }
}

/**
 * Fx55 - Store registers V0 through Vx in memory starting at location I.
 * 
 * The interpreter copies the values of registers V0 through Vx into memory, 
 * starting at the address in I.
 * 
 * TODO: Implement configurable quirks for this instruction
 */
void instructions::LD_I_Vx(const Opcode& opcode, Memory& memory, Registers& 
    registers) {
    for (int j = 0; j <= opcode.x(); j++) {
        memory[registers.i + j] = registers.v[j];
    }
}

/**
 * Fx65 - Read registers V0 through Vx from memory starting at location I.
 * 
 * The interpreter reads values from memory starting at location I into 
 * registers V0 through Vx.
 */
void instructions::LD_Vx_I(const Opcode& opcode, Memory& memory, Registers& 
    registers) {
    for (int j = 0; j <= opcode.x(); j++) {
        registers.v[j] = memory[registers.i + j];
    }
}
