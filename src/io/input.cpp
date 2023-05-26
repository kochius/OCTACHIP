#include "io/input.hpp"

using namespace CHIP8;

Input::Input() {
    keyMap[SDLK_x] = 0;
    keyMap[SDLK_1] = 1;
    keyMap[SDLK_2] = 2;
    keyMap[SDLK_3] = 3;
    keyMap[SDLK_q] = 4;
    keyMap[SDLK_w] = 5;
    keyMap[SDLK_e] = 6;
    keyMap[SDLK_a] = 7;
    keyMap[SDLK_s] = 8;
    keyMap[SDLK_d] = 9;
    keyMap[SDLK_z] = 0xA;
    keyMap[SDLK_c] = 0xB;
    keyMap[SDLK_4] = 0xC;
    keyMap[SDLK_r] = 0xD;
    keyMap[SDLK_f] = 0xE;
    keyMap[SDLK_v] = 0xF;
}

bool Input::processInput(std::function<void(int, bool)> keyEventHandler) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        if (event.type == SDL_QUIT) {
            return false;
        }
        SDL_Keycode key = event.key.keysym.sym;
        if (event.type == SDL_KEYDOWN) {
            if (key == SDLK_ESCAPE) {
                return false;
            }
            if (keyMap.find(key) != keyMap.end()) {
                keyEventHandler(keyMap[key], true);
            }
        }
        if (event.type == SDL_KEYUP) {
            if (keyMap.find(key) != keyMap.end()) {
                keyEventHandler(keyMap[key], false);
            }
        }
    }
    return true;
}