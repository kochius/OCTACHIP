#pragma once

#include <random>

namespace CHIP8 {

class Random {
public:
    template <typename Seed>
    Random(Seed seed, int min, int max);
    uint8_t generateNumber();
private:
    std::mt19937 engine;
    std::uniform_int_distribution<uint8_t> dist;
};

}