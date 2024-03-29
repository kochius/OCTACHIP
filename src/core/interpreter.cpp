#include <algorithm>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <random>
#include <sstream>
#include <stdexcept>
#include <string>

#include "core/instructions.hpp"
#include "core/interpreter.hpp"
#include "core/opcode.hpp"

using namespace OCTACHIP;

Interpreter::Interpreter() :
    memory{},
    registers{},
    stack{},
    frame{},
    keypad{},
    prevKeypadState{},
    random{},
    loadStoreQuirk{true},
    shiftQuirk{true},
    wrapQuirk{false} {
    const std::array<uint8_t, FONT_SET_SIZE> fontSet = {
        0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
        0x20, 0x60, 0x20, 0x20, 0x70, // 1
        0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
        0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
        0x90, 0x90, 0xF0, 0x10, 0x10, // 4
        0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
        0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
        0xF0, 0x10, 0x20, 0x40, 0x40, // 7
        0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
        0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
        0xF0, 0x90, 0xF0, 0x90, 0x90, // A
        0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
        0xF0, 0x80, 0x80, 0x80, 0xF0, // C
        0xE0, 0x90, 0x90, 0x90, 0xE0, // D
        0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
        0xF0, 0x80, 0xF0, 0x80, 0x80  // F
    };
    std::copy(std::begin(fontSet), std::end(fontSet), std::begin(this->memory) + 
        FONT_START_ADDRESS);
    registers.pc = PROG_START_ADDRESS;
}

void Interpreter::reset() {
    std::fill(std::begin(memory), std::begin(memory) + FONT_START_ADDRESS, 
        uint8_t{0});
    std::fill(std::begin(memory) + FONT_START_ADDRESS + FONT_SET_SIZE, 
        std::end(memory), uint8_t{0});

    registers.v.fill(0);
    registers.pc = PROG_START_ADDRESS;
    registers.i = 0;
    registers.sp = 0;
    registers.delayTimer = 0;
    registers.soundTimer = 0;

    stack.fill(0);
    frame.fill(false);
    keypad.fill(false);
    prevKeypadState.fill(false);

    loadStoreQuirk = true;
    shiftQuirk = true;
    wrapQuirk = false;
}

void Interpreter::loadRom(const std::filesystem::path& romPath) {
    if (!std::filesystem::exists(romPath)) {
        throw std::runtime_error("File not found: " + romPath.string());
    }

    std::ifstream romFile{romPath, std::ios_base::in | std::ios_base::binary};

    if (!romFile) {
        throw std::runtime_error("Failed to open file: " + romPath.string());
    }

    const uintmax_t romSize = std::filesystem::file_size(romPath);
    const uintmax_t maxRomSize = MEMORY_SIZE - PROG_START_ADDRESS;

    if (romSize > maxRomSize) {
        throw std::length_error("File exceeds maximum ROM size: " + 
            romPath.string() +  " (current size: " + std::to_string(romSize) +
            " bytes, maximum size: " + std::to_string(maxRomSize) + " bytes)");
    }

    romFile.seekg(0, std::ios_base::beg);
    romFile.read(reinterpret_cast<char*>(memory.data() + PROG_START_ADDRESS), 
        romSize);
    
    if (!romFile) {
        throw std::runtime_error("Failed to read file: " + romPath.string());
    }
}

void Interpreter::updateTimers() {
    if (registers.delayTimer > 0) {
        registers.delayTimer--;
    }
    if (registers.soundTimer > 0) {
        registers.soundTimer--;
    }
}

void Interpreter::setKey(const int key, const bool isPressed) {
    keypad[key] = isPressed;
}

void Interpreter::setLoadStoreQuirk(const bool isEnabled) {
    loadStoreQuirk = isEnabled;
}

void Interpreter::setShiftQuirk(const bool isEnabled) {
    shiftQuirk = isEnabled;
}

void Interpreter::setWrapQuirk(const bool isEnabled) {
    wrapQuirk = isEnabled;
}

void Interpreter::tick() {
    const Opcode opcode = memory[registers.pc] << 8 | memory[registers.pc + 1];

    registers.pc += 2;

    switch (opcode.prefix()) {
        case 0x0: 
            switch (opcode.byte()) {
                case 0xE0: return instructions::CLS(frame);
                case 0xEE: return instructions::RET(registers, stack);
                default: return instructions::ILLEGAL_OPCODE(opcode);
            }
        case 0x1: return instructions::JP_ADDR(opcode, registers);
        case 0x2: return instructions::CALL_ADDR(opcode, registers, stack);
        case 0x3: return instructions::SE_VX_BYTE(opcode, registers);
        case 0x4: return instructions::SNE_VX_BYTE(opcode, registers);
        case 0x5: return instructions::SE_VX_VY(opcode, registers);
        case 0x6: return instructions::LD_VX_BYTE(opcode, registers);
        case 0x7: return instructions::ADD_VX_BYTE(opcode, registers);
        case 0x8:
            switch (opcode.nibble()) {
                case 0x0: return instructions::LD_VX_VY(opcode, registers);
                case 0x1: return instructions::OR_VX_VY(opcode, registers);
                case 0x2: return instructions::AND_VX_VY(opcode, registers);
                case 0x3: return instructions::XOR_VX_VY(opcode, registers);
                case 0x4: return instructions::ADD_VX_VY(opcode, registers);
                case 0x5: return instructions::SUB_VX_VY(opcode, registers);
                case 0x6: return instructions::SHR_VX_VY(opcode, registers, 
                    shiftQuirk);
                case 0x7: return instructions::SUBN_VX_VY(opcode, registers);
                case 0xE: return instructions::SHL_VX_VY(opcode, registers,
                    shiftQuirk);
                default: return instructions::ILLEGAL_OPCODE(opcode);
            }
        case 0x9: return instructions::SNE_VX_VY(opcode, registers);
        case 0xA: return instructions::LD_I_ADDR(opcode, registers);
        case 0xB: return instructions::JP_V0_ADDR(opcode, registers);
        case 0xC: return instructions::RND_VX_BYTE(opcode, registers, random);
        case 0xD: return instructions::DRW_VX_VY_NIBBLE(opcode, memory, 
            registers, frame, wrapQuirk);
        case 0xE:
            switch(opcode.byte()) {
                case 0x9E: return instructions::SKP_VX(opcode, registers, 
                    keypad);
                case 0xA1: return instructions::SKNP_VX(opcode, registers, 
                    keypad);
                default: return instructions::ILLEGAL_OPCODE(opcode);
            }
        case 0xF:
            switch(opcode.byte()) {
                case 0x07: return instructions::LD_VX_DT(opcode, registers);
                case 0x0A: return instructions::LD_VX_K(opcode, registers, 
                    keypad, prevKeypadState);
                case 0x15: return instructions::LD_DT_VX(opcode, registers);
                case 0x18: return instructions::LD_ST_VX(opcode, registers);
                case 0x1E: return instructions::ADD_I_VX(opcode, registers);
                case 0x29: return instructions::LD_F_VX(opcode, registers);
                case 0x33: return instructions::LD_B_VX(opcode, memory, 
                    registers);
                case 0x55: return instructions::LD_I_VX(opcode, memory, 
                    registers, loadStoreQuirk);
                case 0x65: return instructions::LD_VX_I(opcode, memory, 
                    registers, loadStoreQuirk);
                default: return instructions::ILLEGAL_OPCODE(opcode);
            }
        default: return instructions::ILLEGAL_OPCODE(opcode);
    }
}

std::string hexFormat(const int value, const int length) {
    std::stringstream stream;
    stream << std::uppercase << std::setfill('0') << std::setw(length)
        << std::hex << value;
    return stream.str();
}

std::string Interpreter::disassembleOpcode(const int address) const {
    const Opcode opcode = memory[address] << 8 | memory[address + 1];
    std::stringstream instruction;
    switch (opcode.prefix()) {
        case 0x0: 
            switch (opcode.byte()) {
                case 0xE0:
                    instruction << "CLS";
                    break;
                case 0xEE:
                    instruction << "RET";
                    break;
                default:
                    instruction << "-";
            }
            break;
        case 0x1:
            instruction << "JP 0x" << hexFormat(opcode.address(), 4);
            break;
        case 0x2:
            instruction << "CALL 0x" << hexFormat(opcode.address(), 4);
            break;
        case 0x3:
            instruction << "SE V" << hexFormat(opcode.x(), 1) << ", 0x" 
                << hexFormat(opcode.byte(), 2);
            break;
        case 0x4:
            instruction << "SNE V" << hexFormat(opcode.x(), 1) << ", 0x"
                << hexFormat(opcode.byte(), 2);
            break;
        case 0x5:
            instruction << "SE V" << hexFormat(opcode.x(), 1) << ", V" 
                << hexFormat(opcode.y(), 1);
            break;
        case 0x6:
            instruction << "LD V" << hexFormat(opcode.x(), 1) << ", 0x"
                << hexFormat(opcode.byte(), 2);
            break;
        case 0x7:
            instruction << "ADD V" << hexFormat(opcode.x(), 1) << ", 0x"
                << hexFormat(opcode.byte(), 2);
            break;
        case 0x8:
            switch (opcode.nibble()) {
                case 0x0:
                    instruction << "LD V" << hexFormat(opcode.x(), 1) << ", V" 
                        << hexFormat(opcode.y(), 1);
                    break;
                case 0x1:
                    instruction << "OR V" << hexFormat(opcode.x(), 1) << ", V" 
                        << hexFormat(opcode.y(), 1);
                    break;
                case 0x2:
                    instruction << "AND V" << hexFormat(opcode.x(), 1) << ", V" 
                        << hexFormat(opcode.y(), 1);
                    break;
                case 0x3:
                    instruction << "XOR V" << hexFormat(opcode.x(), 1) << ", V" 
                        << hexFormat(opcode.y(), 1);
                    break;
                case 0x4:
                    instruction << "ADD V" << hexFormat(opcode.x(), 1) << ", V" 
                        << hexFormat(opcode.y(), 1);
                    break;
                case 0x5:
                    instruction << "SUB V" << hexFormat(opcode.x(), 1) << ", V" 
                        << hexFormat(opcode.y(), 1);
                    break;
                case 0x6:
                    instruction << "SHR V" << hexFormat(opcode.x(), 1) << ", V" 
                        << hexFormat(opcode.y(), 1);
                    break;
                case 0x7:
                    instruction << "SUBN V" << hexFormat(opcode.x(), 1) << ", V" 
                        << hexFormat(opcode.y(), 1);
                    break;
                case 0xE:
                    instruction << "SHL V" << hexFormat(opcode.x(), 1) << ", V" 
                        << hexFormat(opcode.y(), 1);
                    break;
                default:
                    instruction << "-";
            }
            break;
        case 0x9:
            instruction << "SNE V" << hexFormat(opcode.x(), 1) << ", V" 
                << hexFormat(opcode.y(), 1);
            break;
        case 0xA:
            instruction << "LD I, 0x" << hexFormat(opcode.address(), 4);
            break;
        case 0xB:
            instruction << "JP V0, 0x" << hexFormat(opcode.address(), 4);
            break;
        case 0xC:
            instruction << "RND V" << hexFormat(opcode.x(), 1) << ", 0x" 
                << hexFormat(opcode.byte(), 2);
            break;
        case 0xD:
            instruction << "DRW V" << hexFormat(opcode.x(), 1) << ", V"
                << hexFormat(opcode.y(), 1) << ", 0x" 
                << hexFormat(opcode.nibble(), 1);
            break;
        case 0xE:
            switch(opcode.byte()) {
                case 0x9E:
                    instruction << "SKP V" << hexFormat(opcode.x(), 1);
                    break;
                case 0xA1:
                    instruction << "SKNP V" << hexFormat(opcode.x(), 1);
                    break;
                default:
                    instruction << "-";
            }
            break;
        case 0xF:
            switch(opcode.byte()) {
                case 0x07:
                    instruction << "LD V" << hexFormat(opcode.x(), 1) << ", DT";
                    break;
                case 0x0A:
                    instruction << "LD V" << hexFormat(opcode.x(), 1) << ", K";
                    break;
                case 0x15:
                    instruction << "LD DT, V" << hexFormat(opcode.x(), 1);
                    break;
                case 0x18:
                    instruction << "LD ST, V" << hexFormat(opcode.x(), 1);
                    break;
                case 0x1E:
                    instruction << "ADD I, V" << hexFormat(opcode.x(), 1);
                    break;
                case 0x29:
                    instruction << "LD F, V" << hexFormat(opcode.x(), 1);
                    break;
                case 0x33: 
                    instruction << "LD B, V" << hexFormat(opcode.x(), 1);
                    break;
                case 0x55:
                    instruction << "LD [I], V" << hexFormat(opcode.x(), 1);
                    break;
                case 0x65: 
                    instruction << "LD V" << hexFormat(opcode.x(), 1) 
                        << ", [I]";
                    break;
                default:
                    instruction << "-";
            }
            break;
        default:
            instruction << "-";
    }
    return instruction.str();
}

std::string Interpreter::getDisassembledInstructions() const {
    std::stringstream stream;

    for (int i = PROG_START_ADDRESS; i < MEMORY_SIZE; i++) {
        const std::string instruction = disassembleOpcode(i);
        stream << "0x" << hexFormat(i, 4) << ": " << instruction << "\n";
    }

    return stream.str();
}

uint8_t Interpreter::getRegisterValue(const int index) const {
    if (index < 0 || index >= Registers::V_REG_COUNT) {
        throw std::out_of_range("Invalid register index: " + 
            std::to_string(index));
    }
    return registers.v[index];
}

uint16_t Interpreter::getProgramCounterValue() const {
    return registers.pc;
}

uint16_t Interpreter::getIndexRegisterValue() const {
    return registers.i;
}

uint8_t Interpreter::getStackPointerValue() const {
    return registers.sp;
}

uint8_t Interpreter::getDelayTimerValue() const {
    return registers.delayTimer;
}

uint8_t Interpreter::getSoundTimerValue() const {
    return registers.soundTimer;
}

uint16_t Interpreter::getStackValue(const int index) const {
    if (index < 0 || index >= STACK_SIZE) {
        throw std::out_of_range("Invalid stack index: " + 
            std::to_string(index));
    }
    return stack[index];
}

const Frame& Interpreter::getFrame() const {
    return frame;
}