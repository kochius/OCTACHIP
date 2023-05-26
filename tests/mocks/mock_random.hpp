#pragma once

#include "core/random.hpp"

namespace CHIP8 {

template <typename T>
class MockRandom : public Random<T> {
public:
    MockRandom() :
        Random<T>(0, T{}, T{}) {}
    T generateNumber() override {
        return 0x37;
    }
};

}