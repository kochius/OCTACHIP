#pragma once

#include <climits>
#include <random>

namespace CHIP8 {

class Random {
public:
    Random(unsigned int seed = std::random_device{}(), uint8_t min = 0, 
        uint8_t max = 255);
    virtual ~Random() = default;
    virtual uint8_t generateNumber();
private:
    std::mt19937 engine;
    std::uniform_int_distribution<uint8_t> distribution;
};

}