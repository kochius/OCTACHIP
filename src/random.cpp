#include "random.hpp"

using namespace CHIP8;

template <typename Seed>
Random::Random(Seed seed, int min, int max) : 
    engine{std::random_device{}()},
    dist{min, max} {}

uint8_t Random::generateNumber() {
    return dist(engine);
}