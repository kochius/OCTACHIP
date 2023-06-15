#pragma once

#include <climits>
#include <random>
#include <type_traits>

namespace OCTACHIP {

template <typename T>
class Random {
public:
    Random(unsigned int seed, T min, T max) : 
        engine{seed}, minValue{min}, maxValue{max} {}
    virtual ~Random() = default;
    virtual T generateNumber()  {
        if constexpr (std::is_integral<T>::value) {
            std::uniform_int_distribution<T> distribution{minValue, maxValue};
            return distribution(engine);
        }
        else {
            std::uniform_real_distribution<T> distribution{minValue, maxValue};
            return distribution(engine);
        }
    }
private:
    std::mt19937 engine;
    T minValue;
    T maxValue;
};

}