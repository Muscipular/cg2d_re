#include "core/Log.hpp"
#include "platform/Window.hpp"

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

    cg::platform::Window window;
    if (!window.Create("cg-client M1.1", WindowWidth, WindowHeight, SDL_WINDOW_RESIZABLE)) {
        cg::core::LogSdlError("app", "SDL_CreateWindow");
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
                event.window.windowID == window.GetId()) {
                running = false;
            }
        }

        SDL_Delay(16);
    }

    window.Destroy();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
