#include "core/Log.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <string>

namespace {

constexpr int WindowWidth = 1280;
constexpr int WindowHeight = 720;

} // namespace

int main(int argc, char* argv[])
{
    (void)argc;
    (void)argv;

    if (!SDL_Init(SDL_INIT_VIDEO)) {
        cg::core::LogSdlError("app", "SDL_Init");
        return 1;
    }

    if (!TTF_Init()) {
        cg::core::LogSdlError("app", "TTF_Init");
        SDL_Quit();
        return 1;
    }

    cg::core::Log(cg::core::LogLevel::Info, "app", "SDL3_image version: " + std::to_string(IMG_Version()));

    SDL_Window* window = SDL_CreateWindow("cg-client M0", WindowWidth, WindowHeight, SDL_WINDOW_RESIZABLE);
    if (window == nullptr) {
        cg::core::LogSdlError("app", "SDL_CreateWindow");
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, nullptr);
    if (renderer == nullptr) {
        cg::core::LogSdlError("app", "SDL_CreateRenderer");
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    bool running = true;
    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                running = false;
            }

            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                event.window.windowID == SDL_GetWindowID(window)) {
                running = false;
            }
        }

        if (!SDL_SetRenderDrawColor(renderer, 18, 20, 24, SDL_ALPHA_OPAQUE)) {
            cg::core::LogSdlError("render", "SDL_SetRenderDrawColor");
            break;
        }

        if (!SDL_RenderClear(renderer)) {
            cg::core::LogSdlError("render", "SDL_RenderClear");
            break;
        }

        if (!SDL_RenderPresent(renderer)) {
            cg::core::LogSdlError("render", "SDL_RenderPresent");
            break;
        }

        SDL_Delay(16);
    }

    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();

    return 0;
}
