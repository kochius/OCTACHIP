#include <stdexcept>

#include "io/renderer.hpp"

using namespace CHIP8;

Renderer::Renderer(const int width, const int height, 
    const std::string_view& title) :
        window{nullptr},
        renderer{nullptr},
        baseWidth{width},
        baseHeight{height} {
    using namespace std::string_literals;

    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        throw std::runtime_error("Failed to initialize SDL video subsystem: "s 
            + std::string(SDL_GetError()));
    }

    SDL_CreateWindowAndRenderer(width * renderScale, height * renderScale, 0, 
        &window, &renderer);
    if (window == nullptr || renderer == nullptr) {
        throw std::runtime_error("Failed to create SDL window and renderer: "s 
            + std::string(SDL_GetError()));
    }

    SDL_SetWindowTitle(window, title.data());
}

Renderer::~Renderer() { 
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    SDL_Quit();
}

void Renderer::drawFrame(const Frame& frame) {
    clearRenderer();

    for (int row = 0; row < baseHeight; row++) {
        for (int col = 0; col < baseWidth; col++) {
            if (frame[col + (row * baseWidth)]) {
                drawPixel(col, row);
            }
        }
    }

    SDL_RenderPresent(renderer);
}

void Renderer::drawPixel(int col, int row) {
    SDL_SetRenderDrawColor(renderer, 0xFF, 0xFF, 0xFF, 0xFF);
    SDL_Rect pixelBlock{col * renderScale, row * renderScale, renderScale, 
        renderScale};
    SDL_RenderDrawRect(renderer, &pixelBlock);
    SDL_RenderFillRect(renderer, &pixelBlock);
}

void Renderer::clearRenderer() {
    SDL_SetRenderDrawColor(renderer, 0, 0, 0, 0);
    SDL_RenderClear(renderer);
}