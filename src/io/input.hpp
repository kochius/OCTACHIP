#pragma once

#include <functional>
#include <SDL2/SDL.h>
#include <unordered_map>

namespace CHIP8 {

class Input {
public:
    Input();
    bool processInput(std::function<void(int, bool)> callback);
private:
    std::unordered_map<SDL_Keycode, uint8_t> keyMap;
};

}