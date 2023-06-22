#include <limits>

#include "core/random.hpp"

using namespace OCTACHIP;

Random::Random() :
    engine{std::random_device{}()},
    distribution{std::numeric_limits<uint8_t>::min(),
        std::numeric_limits<uint8_t>::max()} {}

uint8_t Random::generateNumber() {
    return static_cast<uint8_t>(distribution(engine));
}