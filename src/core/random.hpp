#pragma once

#include <random>

namespace CHIP8 {

class Random {
public:
    Random(unsigned int seed, int min, int max);
    int generateNumber();
private:
    std::mt19937 engine;
    std::uniform_int_distribution<int> distribution;
};

}