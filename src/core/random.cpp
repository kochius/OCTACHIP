#include <stdexcept>

#include "random.hpp"

using namespace CHIP8;

Random::Random(unsigned int seed, uint8_t min, uint8_t max) : 
    engine{seed},
    distribution{min, max} {
    if (min > max) {
        throw std::invalid_argument("min must be less than or equal to max");
    }
}

uint8_t Random::generateNumber() {
    return distribution(engine);
}