#pragma once

#include <cstdint>

namespace CHIP8 {

class Opcode {
public:
    Opcode(uint16_t opcode);
    uint8_t x() const;
    uint8_t y() const;
    uint8_t nibble() const;
    uint8_t byte() const;
    uint16_t address() const;
    uint8_t prefix() const;
private:
    uint16_t opcode;
};

}