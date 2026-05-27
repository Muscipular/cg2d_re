#include "core/Log.hpp"
#include "platform/Event.hpp"
#include "platform/Ime.hpp"
#include "platform/Input.hpp"
#include "platform/Timer.hpp"
#include "platform/Window.hpp"
#include "render/SdlRendererBackend.hpp"

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

    cg::render::SdlRendererBackend renderer;
    if (!renderer.Initialize(window)) {
        cg::core::LogSdlError("render", "SDL_CreateRenderer");
        window.Destroy();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    cg::platform::ImeController imeController(window);
    if (!imeController.BeginTextInput()) {
        cg::core::LogSdlError("ime", "SDL_StartTextInput");
    }
    if (!imeController.SetInputArea(0.0F, 0.0F, 1.0F, 1.0F, 0.0F)) {
        cg::core::LogSdlError("ime", "SDL_SetTextInputArea");
    }

    cg::platform::EventPump eventPump;
    cg::platform::InputState inputState;
    cg::platform::FrameTimer frameTimer;
    bool running = true;
    while (running) {
        const double deltaSeconds = frameTimer.Tick();
        (void)deltaSeconds;
        inputState.BeginFrame();

        cg::platform::Event event;
        while (eventPump.Poll(event)) {
            inputState.HandleEvent(event);

            if (event.type == cg::platform::EventType::Quit) {
                running = false;
            }

            if (event.type == cg::platform::EventType::WindowCloseRequested &&
                event.windowId == window.GetId()) {
                running = false;
            }
        }

        if (!renderer.BeginFrame({0.07F, 0.08F, 0.09F, 1.0F})) {
            cg::core::LogSdlError("render", "BeginFrame");
            break;
        }

        if (!renderer.EndFrame()) {
            cg::core::LogSdlError("render", "EndFrame");
            break;
        }

        inputState.EndFrame();
        frameTimer.DelayToCap(60.0);
    }

    if (!imeController.EndTextInput()) {
        cg::core::LogSdlError("ime", "SDL_StopTextInput");
    }

    window.Destroy();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
