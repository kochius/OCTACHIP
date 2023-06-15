#pragma once

#include <cstdint>

namespace OCTACHIP {

class Opcode {
public:
    Opcode(const uint16_t instructionCode);
    uint8_t x() const;
    uint8_t y() const;
    uint8_t nibble() const;
    uint8_t byte() const;
    uint16_t address() const;
    uint8_t prefix() const;
    uint16_t full() const;
private:
    uint16_t opcode;
};

}