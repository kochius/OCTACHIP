#pragma once

#include "frame.hpp"
#include "opcode.hpp"
#include "random.hpp"
#include "types.hpp"

namespace CHIP8::instructions {

// 00E0 - Clear the display.
void CLS(Frame& frame);

// 00EE - Return from a subroutine.
void RET(Registers& registers, Stack& stack);

// 1nnn - Jump to location nnn.
void JP_addr(const Opcode& opcode, Registers& registers);

// 2nnn - Call subroutine at nnn.
void CALL_addr(const Opcode& opcode, Registers& registers, Stack& stack);

// 3xkk - Skip next instruction if Vx = kk.
void SE_Vx_byte(const Opcode& opcode, Registers& registers);

// 4xkk - Skip next instruction if Vx != kk.
void SNE_Vx_byte(const Opcode& opcode, Registers& registers);

// 5xy0 - Skip next instruction if Vx = Vy.
void SE_Vx_Vy(const Opcode& opcode, Registers& registers);

// 6xkk - Set Vx = kk.
void LD_Vx_byte(const Opcode& opcode, Registers& registers);

// 7xkk - Set Vx = Vx + kk.
void ADD_Vx_byte(const Opcode& opcode, Registers& registers);

// 8xy0 - Set Vx = Vy.
void LD_Vx_Vy(const Opcode& opcode, Registers& registers);

// 8xy1 - Set Vx = Vx OR Vy.
void OR_Vx_Vy(const Opcode& opcode, Registers& registers);

// 8xy2 - Set Vx = Vx AND Vy.
void AND_Vx_Vy(const Opcode& opcode, Registers& registers);

// 8xy3 - Set Vx = Vx XOR Vy.
void XOR_Vx_Vy(const Opcode& opcode, Registers& registers);

// 8xy4 - Set Vx = Vx + Vy, set VF = carry.
void ADD_Vx_Vy(const Opcode& opcode, Registers& registers);

// 8xy5 - Set Vx = Vx - Vy, set VF = NOT borrow.
void SUB_Vx_Vy(const Opcode& opcode, Registers& registers);

// 8xy6 - Set Vx = Vx SHR 1.
void SHR_Vx_Vy(const Opcode& opcode, Registers& registers);

// 8xy7 - Set Vx = Vy - Vx, set VF = NOT borrow.
void SUBN_Vx_Vy(const Opcode& opcode, Registers& registers);

// 8xyE - Set Vx = Vx SHL 1.
void SHL_Vx_Vy(const Opcode& opcode, Registers& registers);

// 9xy0 - Skip next instruction if Vx != Vy.
void SNE_Vx_Vy(const Opcode& opcode, Registers& registers);

// Annn - Set I = nnn.
void LD_I_addr(const Opcode& opcode, Registers& registers);

// Bnnn - Jump to location nnn + V0.
void JP_V0_addr(const Opcode& opcode, Registers& registers);

// Cxkk - Set Vx = random byte AND kk.
void RND_Vx_byte(const Opcode& opcode, Registers& registers, Random& random);

// Dxyn - Display n-byte sprite starting at memory location I at (Vx, Vy), set 
//        VF = collision.
void DRW_Vx_Vy_nibble(const Opcode& opcode, const Memory& memory, 
    Registers& registers, Frame& frame);

// Ex9E - Skip next instruction if key with the value of Vx is pressed.
void SKP_Vx(const Opcode& opcode, Registers& registers, const Keypad& keypad);

// ExA1 - Skip next instruction if key with the value of Vx is not pressed.
void SKNP_Vx(const Opcode& opcode, Registers& registers, const Keypad& keypad);

// Fx07 - Set Vx = delay timer value.
void LD_Vx_DT(const Opcode& opcode, Registers& registers);

// Fx0A - Wait for a key press, store the value of the key in Vx.
void LD_Vx_K(const Opcode& opcode, Registers& registers, const Keypad& keypad);

// Fx15 - Set delay timer = Vx.
void LD_DT_Vx(const Opcode& opcode, Registers& registers);

// Fx18 - Set sound timer = Vx.
void LD_ST_Vx(const Opcode& opcode, Registers& registers);

// Fx1E - Set I = I + Vx.
void ADD_I_Vx(const Opcode& opcode, Registers& registers);

// Fx29 - Set I = location of sprite for digit Vx.
void LD_F_Vx(const Opcode& opcode, Registers& registers, 
    const int fontStartaddress, const int fontCharSize);

// Fx33 - Store BCD representation of Vx in memory locations I, I+1, and I+2.
void LD_B_Vx(const Opcode& opcode, Memory& memory, Registers& registers);

// Fx55 - Store registers V0 through Vx in memory starting at location I.
void LD_I_Vx(const Opcode& opcode, Memory& memory, Registers& registers);

// Fx65 - Read registers V0 through Vx from memory starting at location I.
void LD_Vx_I(const Opcode& opcode, Memory& memory, Registers& registers);

}