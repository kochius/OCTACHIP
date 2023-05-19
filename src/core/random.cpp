#include <stdexcept>

#include "random.hpp"

using namespace CHIP8;

Random::Random(unsigned int seed, int min, int max) : 
    engine{seed},
    distribution{min, max} {
    if (min > max) {
        throw std::invalid_argument("min must be less than or equal to max");
    }
}

int Random::generateNumber() {
    return distribution(engine);
}