#pragma once

#include <random>

namespace OCTACHIP {

class Random {
public:
    Random();
    virtual ~Random() = default;
    virtual uint8_t generateNumber();
private:
    std::mt19937 engine;
    std::uniform_int_distribution<unsigned int> distribution;
};

}