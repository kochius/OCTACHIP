#include "frame.hpp"

using namespace CHIP8;

void Frame::clearFrame() {
    pixels.fill(false);
}

bool& Frame::operator[](const int index) {
    return pixels.at(index);
}

const bool& Frame::operator[](const int index) const {
    return pixels.at(index);
}