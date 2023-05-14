#include "input.hpp"

#include <SDL2/SDL.h>

using namespace CHIP8;

bool Input::processInput() {
    bool shouldRun = true;
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch(event.type) {
            case SDL_QUIT:
                shouldRun = false;
                break;
        }
    }
    return shouldRun;
}