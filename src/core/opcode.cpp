#include "core/opcode.hpp"

using namespace OCTACHIP;

Opcode::Opcode(const uint16_t instructionCode) : opcode{instructionCode} {}

uint8_t Opcode::x() const {
    return (opcode & 0x0F00) >> 8;
}

uint8_t Opcode::y() const {
    return (opcode & 0x00F0) >> 4;
}

uint8_t Opcode::nibble() const {
    return opcode & 0x000F;
}

uint8_t Opcode::byte() const {
    return opcode & 0x00FF;
}

uint16_t Opcode::address() const {
    return opcode & 0x0FFF;
}

uint8_t Opcode::prefix() const {
    return opcode >> 12;
}

uint16_t Opcode::full() const {
    return opcode;
}