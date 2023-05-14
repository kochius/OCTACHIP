#include "random.hpp"

using namespace CHIP8;

template <typename Seed>
Random::Random(Seed seed, int min, int max) : 
    engine{seed},
    dist{min, max} {}

int Random::generateNumber() {
    return dist(engine);
}