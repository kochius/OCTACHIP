#pragma once

#include "src/core/random.hpp"

namespace CHIP8 {

class MockRandom : public Random {
public:
    MockRandom() : Random() {}
    uint8_t generateNumber() override {
        return 0x80;
    }
};

}