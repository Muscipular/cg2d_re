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

#include <cstdint>
#include <vector>
#include <string>

namespace {

constexpr int WindowWidth = 1280;
constexpr int WindowHeight = 720;
constexpr int TestTextureSize = 64;

std::vector<std::uint8_t> CreateTestTexturePixels()
{
    std::vector<std::uint8_t> pixels(TestTextureSize * TestTextureSize * 4);
    for (int y = 0; y < TestTextureSize; ++y) {
        for (int x = 0; x < TestTextureSize; ++x) {
            const bool bright = ((x / 8) + (y / 8)) % 2 == 0;
            const std::size_t offset = static_cast<std::size_t>((y * TestTextureSize + x) * 4);
            pixels[offset + 0] = bright ? 0x4CU : 0xF7U;
            pixels[offset + 1] = bright ? 0xC9U : 0x25U;
            pixels[offset + 2] = bright ? 0xF0U : 0x85U;
            pixels[offset + 3] = 0xFFU;
        }
    }

    return pixels;
}

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
        renderer.Shutdown();
        window.Destroy();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    const auto testPixels = CreateTestTexturePixels();
    const cg::render::TextureHandle testTexture =
        renderer.CreateTexture({TestTextureSize, TestTextureSize, cg::render::PixelFormat::Rgba8});
    if (!testTexture.IsValid() ||
        !renderer.UpdateTexture(testTexture, testPixels.data(), TestTextureSize * static_cast<int>(sizeof(std::uint32_t)))) {
        cg::core::LogSdlError("render", "CreateTexture/UpdateTexture");
        renderer.Shutdown();
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

        if (!renderer.DrawTexture(testTexture,
                {0.0F, 0.0F, static_cast<float>(TestTextureSize), static_cast<float>(TestTextureSize)},
                {64.0F, 64.0F, 192.0F, 192.0F})) {
            cg::core::LogSdlError("render", "DrawTexture");
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

    renderer.DestroyTexture(testTexture);
    renderer.Shutdown();
    window.Destroy();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
