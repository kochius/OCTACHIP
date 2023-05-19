#include <fstream>
#include <iostream>
#include <iterator>
#include <random>
#include <stdexcept>
#include <string>

#include "instructions.hpp"
#include "interpreter.hpp"
#include "opcode.hpp"

using namespace CHIP8;

Interpreter::Interpreter() :
    random{std::random_device{}(), 0, 255} {
    constexpr std::array<uint8_t, FONT_SET_SIZE> fontSet = {
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
    std::copy(std::begin(fontSet), std::end(fontSet), 
        std::begin(this->memory) + FONT_START_ADDRESS);
    registers.pc = PROG_START_ADDRESS;
}

void Interpreter::loadRom(const std::filesystem::path& romPath) {
    using namespace std::string_literals;

    if (!std::filesystem::exists(romPath)) {
        throw std::runtime_error("File not found: "s + romPath.string());
    }
    
    std::ifstream romFile{romPath, std::ios_base::in | std::ios_base::binary};
    if (!romFile) {
        throw std::runtime_error("Unable to open file: "s + romPath.string());
    }

    const uintmax_t romSize = std::filesystem::file_size(romPath);
    constexpr unsigned int MAX_ROM_SIZE = MEMORY_SIZE - PROG_START_ADDRESS;
    if (romSize > MAX_ROM_SIZE) {
        const uintmax_t overflow = romSize - MAX_ROM_SIZE;
        throw std::length_error("File exceeds maximum ROM size by "s + 
            std::to_string(overflow) + " bytes: "s + romPath.string() + 
            " (actual size: " + std::to_string(romSize) + ")");
    }

    romFile.seekg(0, std::ios_base::beg);
    if (!romFile.read(reinterpret_cast<char*>(this->memory.data() + 
        PROG_START_ADDRESS), static_cast<std::streamsize>(romSize))) {
        throw std::runtime_error("Unable to read file: "s + romPath.string());
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

bool Interpreter::soundTimerOn() const {
    return (registers.soundTimer > 0);
}

void Interpreter::setKey(int key, bool isPressed) {
    keypad[key] = isPressed;
}

const Frame& Interpreter::getFrame() const {
    return frame;
}

void Interpreter::tick() {
    using namespace std::string_literals;
    Opcode opcode = memory[registers.pc] << 8 | memory[registers.pc + 1];

    registers.pc += 2;

    switch (opcode.prefix()) {
        case 0x00: 
            switch (opcode.byte()) {
                case 0xE0: return instructions::CLS(frame);
                case 0xEE: return instructions::RET(registers, stack);
                default:
                    throw std::runtime_error("Invalid opcode: "s + 
                        std::to_string(opcode.full()));
            }
        case 0x01: return instructions::JP_addr(opcode, registers);
        case 0x02: return instructions::CALL_addr(opcode, registers, stack);
        case 0x03: return instructions::SE_Vx_byte(opcode, registers);
        case 0x04: return instructions::SNE_Vx_byte(opcode, registers);
        case 0x05: return instructions::SE_Vx_Vy(opcode, registers);
        case 0x06: return instructions::LD_Vx_byte(opcode, registers);
        case 0x07: return instructions::ADD_Vx_byte(opcode, registers);
        case 0x08:
            switch (opcode.nibble()) {
                case 0x00: return instructions::LD_Vx_Vy(opcode, registers);
                case 0x01: return instructions::OR_Vx_Vy(opcode, registers);
                case 0x02: return instructions::AND_Vx_Vy(opcode, registers);
                case 0x03: return instructions::XOR_Vx_Vy(opcode, registers);
                case 0x04: return instructions::ADD_Vx_Vy(opcode, registers);
                case 0x05: return instructions::SUB_Vx_Vy(opcode, registers);
                case 0x06: return instructions::SHR_Vx_Vy(opcode, registers);
                case 0x07: return instructions::SUBN_Vx_Vy(opcode, registers);
                case 0x0E: return instructions::SHL_Vx_Vy(opcode, registers);
                default:
                    throw std::runtime_error("Invalid opcode: "s + 
                        std::to_string(opcode.full()));
            }
        case 0x09: return instructions::SNE_Vx_Vy(opcode, registers);
        case 0x0A: return instructions::LD_I_addr(opcode, registers);
        case 0x0B: return instructions::JP_V0_addr(opcode, registers);
        case 0x0C: return instructions::RND_Vx_byte(opcode, registers, random);
        case 0x0D: return instructions::DRW_Vx_Vy_nibble(opcode, memory, registers, frame);
        case 0x0E:
            switch(opcode.byte()) {
                case 0x9E: return instructions::SKP_Vx(opcode, registers, keypad);
                case 0xA1: return instructions::SKNP_Vx(opcode, registers, keypad);
                default:
                    throw std::runtime_error("Invalid opcode: "s + 
                        std::to_string(opcode.full()));
            }
        case 0x0F:
            switch(opcode.byte()) {
                case 0x07: return instructions::LD_Vx_DT(opcode, registers);
                case 0x0A: return instructions::LD_Vx_K(opcode, registers, keypad);
                case 0x15: return instructions::LD_DT_Vx(opcode, registers);
                case 0x18: return instructions::LD_ST_Vx(opcode, registers);
                case 0x1E: return instructions::ADD_I_Vx(opcode, registers);
                case 0x29: return instructions::LD_F_Vx(opcode, registers, FONT_START_ADDRESS, 
                    FONT_CHAR_SIZE);
                case 0x33: return instructions::LD_B_Vx(opcode, memory, registers);
                case 0x55: return instructions::LD_I_Vx(opcode, memory, registers);
                case 0x65: return instructions::LD_Vx_I(opcode, memory, registers);
                default:
                    throw std::runtime_error("Invalid opcode: "s + 
                        std::to_string(opcode.full()));
            }
        default:
            throw std::runtime_error("Invalid opcode: "s + 
                std::to_string(opcode.full()));
    }
}