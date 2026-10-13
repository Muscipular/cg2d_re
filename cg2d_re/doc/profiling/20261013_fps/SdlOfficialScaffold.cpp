// SDL 3.4.8 官方 renderer/01-clear（公有领域）加有限时长的帧数／提交耗时统计。
// 来源：https://github.com/libsdl-org/SDL/blob/release-3.4.8/examples/renderer/01-clear/clear.c
#define SDL_MAIN_USE_CALLBACKS 1
#include <SDL3/SDL.h>
#include <SDL3/SDL_main.h>
#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <dxgi1_2.h>

static SDL_Window* window = nullptr;
static SDL_Renderer* renderer = nullptr;
static Uint64 started = 0;
static Uint64 measuredFrom = 0;
static Uint64 frames = 0;
static Uint64 presentNs = 0;
static Uint64 foregroundFrames = 0;
static Uint64 totalSeconds = 0;
static HWND nativeWindow = nullptr;

SDL_AppResult SDL_AppInit(void**, int, char**)
{
    SDL_SetAppMetadata("SDL 官方渲染骨架对照", "1.0", "cg.diagnostic.sdl-scaffold");
    if (!SDL_Init(SDL_INIT_VIDEO)
        || !SDL_CreateWindowAndRenderer("SDL 官方渲染骨架对照", 640, 480,
            SDL_WINDOW_RESIZABLE | (SDL_getenv("CG_PROBE_BLIT") ? SDL_WINDOW_TRANSPARENT : 0), &window, &renderer)) {
        SDL_Log("初始化失败：%s", SDL_GetError());
        return SDL_APP_FAILURE;
    }
    SDL_SetRenderLogicalPresentation(renderer, 640, 480, SDL_LOGICAL_PRESENTATION_LETTERBOX);
    int vsync = -2;
    SDL_GetRenderVSync(renderer, &vsync);
    SDL_Log("renderer=%s vsync=%d SDL=%d", SDL_GetRendererName(renderer), vsync, SDL_GetVersion());
    const SDL_DisplayMode* mode = SDL_GetCurrentDisplayMode(SDL_GetDisplayForWindow(window));
    if (mode != nullptr) {
        SDL_Log("显示模式：%dx%d refresh_hz=%.3f", mode->w, mode->h, mode->refresh_rate);
    }
    nativeWindow = static_cast<HWND>(SDL_GetPointerProperty(SDL_GetWindowProperties(window),
        SDL_PROP_WINDOW_WIN32_HWND_POINTER, nullptr));
    auto* swap = static_cast<IDXGISwapChain1*>(SDL_GetPointerProperty(SDL_GetRendererProperties(renderer),
        SDL_PROP_RENDERER_D3D11_SWAPCHAIN_POINTER, nullptr));
    if (swap != nullptr) {
        DXGI_SWAP_CHAIN_DESC1 desc {};
        if (SUCCEEDED(swap->GetDesc1(&desc))) {
            SDL_Log("交换链：effect=%u buffers=%u flags=0x%x", desc.SwapEffect, desc.BufferCount, desc.Flags);
        }
    }
    started = SDL_GetTicksNS();
    return SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppEvent(void*, SDL_Event* event)
{
    return event->type == SDL_EVENT_QUIT ? SDL_APP_SUCCESS : SDL_APP_CONTINUE;
}

SDL_AppResult SDL_AppIterate(void*)
{
    const double now = static_cast<double>(SDL_GetTicks()) / 1000.0;
    const float red = static_cast<float>(0.5 + 0.5 * SDL_sin(now));
    const float green = static_cast<float>(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 2 / 3));
    const float blue = static_cast<float>(0.5 + 0.5 * SDL_sin(now + SDL_PI_D * 4 / 3));
    SDL_SetRenderDrawColorFloat(renderer, red, green, blue, SDL_ALPHA_OPAQUE_FLOAT);
    SDL_RenderClear(renderer);
    const Uint64 before = SDL_GetTicksNS();
    SDL_RenderPresent(renderer);
    const Uint64 after = SDL_GetTicksNS();
    if (measuredFrom == 0) {
        if (after - started >= 2000000000ULL) {
            measuredFrom = after;
        }
    } else {
        ++frames;
        presentNs += after - before;
        foregroundFrames += GetForegroundWindow() == nativeWindow ? 1 : 0;
        if (after - measuredFrom >= 5000000000ULL) {
            const double seconds = static_cast<double>(after - measuredFrom) / 1e9;
            SDL_Log("统计：frames=%llu seconds=%.6f fps=%.3f present_avg_ms=%.3f foreground=%.1f%%",
                static_cast<unsigned long long>(frames), seconds,
                static_cast<double>(frames) / seconds,
                static_cast<double>(presentNs) / static_cast<double>(frames) / 1e6,
                static_cast<double>(foregroundFrames) * 100.0 / static_cast<double>(frames));
            totalSeconds += 5;
            measuredFrom = after;
            frames = 0;
            presentNs = 0;
            foregroundFrames = 0;
            if (totalSeconds >= 45) {
                return SDL_APP_SUCCESS;
            }
        }
    }
    return SDL_APP_CONTINUE;
}

void SDL_AppQuit(void*, SDL_AppResult)
{
    // 官方回调骨架由 SDL 清理窗口和 renderer。
}
