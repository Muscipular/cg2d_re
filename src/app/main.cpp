#include "core/Log.hpp"
#include "platform/Event.hpp"
#include "platform/Ime.hpp"
#include "platform/Input.hpp"
#include "platform/Timer.hpp"
#include "platform/Window.hpp"
#include "render/FontCache.hpp"
#include "render/RenderQueue.hpp"
#include "render/SdlRendererBackend.hpp"
#include "render/TextureAtlas.hpp"
#include "render/TextureCache.hpp"
#include "ui/ChatBox.hpp"
#include "ui/TextLayout.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <array>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace {

constexpr int WindowWidth = 1280;
constexpr int WindowHeight = 720;
constexpr int TestTextureSize = 64;
constexpr std::uint32_t StatsLogFrameInterval = 120;
constexpr std::string_view DemoFontKey = "demo-font";
constexpr std::string_view DemoText = "你好 CrossGate";
constexpr std::string_view DemoLayoutText = "你好 CrossGate\nText layout";
constexpr std::string_view DemoInputText = "你好 ChatBox";
constexpr std::array<std::string_view, 3> DemoFontPaths = {
    "C:/Windows/Fonts/msyh.ttc",
    "C:/Windows/Fonts/simhei.ttf",
    "C:/Windows/Fonts/simsun.ttc",
};

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

[[nodiscard]] const char* ToCandidateDisplayModeName(cg::ui::CandidateDisplayMode mode) noexcept
{
    switch (mode) {
    case cg::ui::CandidateDisplayMode::System:
        return "system";
    case cg::ui::CandidateDisplayMode::Custom:
        return "custom";
    default:
        return "unknown";
    }
}

[[nodiscard]] const char* ToCandidateDirectionName(bool horizontal) noexcept
{
    return horizontal ? "horizontal" : "vertical";
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
    if (!renderer.SetLogicalPresentation(
            {WindowWidth, WindowHeight, cg::render::LogicalPresentationMode::IntegerScale})) {
        cg::core::LogSdlError("render", "SDL_SetRenderLogicalPresentation");
        renderer.Shutdown();
        window.Destroy();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    cg::render::TextureCache textureCache(renderer);
    const auto testPixels = CreateTestTexturePixels();
    const cg::render::TextureHandle testTexture =
        textureCache.LoadRgba8("m2-test-texture", TestTextureSize, TestTextureSize, testPixels.data(), TestTextureSize * 4);
    if (!testTexture.IsValid()) {
        cg::core::LogSdlError("render", "TextureCache::LoadRgba8");
        renderer.Shutdown();
        window.Destroy();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    cg::render::TextureAtlas testAtlas;
    testAtlas.SetTexture(testTexture);
    if (!testAtlas.AddRegion(
            "m2-test-region",
            {0.0F, 0.0F, static_cast<float>(TestTextureSize), static_cast<float>(TestTextureSize)})) {
        cg::core::Log(cg::core::LogLevel::Error, "render", "TextureAtlas::AddRegion failed");
        textureCache.Clear();
        renderer.Shutdown();
        window.Destroy();
        TTF_Quit();
        SDL_Quit();
        return 1;
    }

    cg::render::FontCache fontCache(renderer);
    cg::render::TextTexture demoTextTexture;
    std::string_view loadedFontPath;
    for (const std::string_view fontPath : DemoFontPaths) {
        if (fontCache.LoadFont(DemoFontKey, fontPath, 28.0F)) {
            loadedFontPath = fontPath;
            break;
        }
    }
    if (!loadedFontPath.empty()) {
        demoTextTexture = fontCache.RenderText(DemoFontKey, DemoText, {0.95F, 0.95F, 0.90F, 1.0F});
        if (demoTextTexture.texture.IsValid()) {
            const cg::ui::TextLayoutResult demoLayout =
                cg::ui::LayoutText(fontCache, DemoFontKey, DemoLayoutText, 180);
            cg::core::Log(cg::core::LogLevel::Info,
                "render",
                "FontCache demo loaded: " + std::string{loadedFontPath});
            cg::core::Log(cg::core::LogLevel::Info,
                "ui",
                "TextLayout demo: lines=" + std::to_string(demoLayout.lines.size()) +
                    ", size=" + std::to_string(demoLayout.width) +
                    "x" + std::to_string(demoLayout.height));
        } else {
            cg::core::Log(cg::core::LogLevel::Warning, "render", "FontCache demo text render failed");
        }
    } else {
        cg::core::Log(cg::core::LogLevel::Warning, "render", "FontCache demo font not found");
    }

    cg::platform::ImeController imeController(window);
    if (!imeController.BeginTextInput()) {
        cg::core::LogSdlError("ime", "SDL_StartTextInput");
    }
    cg::ui::ChatBox demoChatBox;
    cg::ui::InputBox& demoInputBox = demoChatBox.Input();
    demoInputBox.SetBounds({64.0F, 344.0F, 360.0F, 36.0F});
    demoInputBox.SetCandidateDisplayMode(cg::ui::CandidateDisplayMode::System);
    demoInputBox.SetText(DemoInputText);
    demoInputBox.SetFocused(true);
    const cg::render::RectF inputBoxBounds = demoInputBox.GetBounds();
    if (!imeController.SetInputArea(
            inputBoxBounds.x,
            inputBoxBounds.y,
            inputBoxBounds.width,
            inputBoxBounds.height,
            0.0F)) {
        cg::core::LogSdlError("ime", "SDL_SetTextInputArea");
    }

    cg::platform::EventPump eventPump;
    cg::platform::InputState inputState;
    cg::platform::FrameTimer frameTimer;
    cg::render::RenderQueue renderQueue;
    std::uint32_t framesSinceStatsLog = 0;
    bool running = true;
    while (running) {
        const double deltaSeconds = frameTimer.Tick();
        (void)deltaSeconds;
        inputState.BeginFrame();

        cg::platform::Event event;
        while (eventPump.Poll(event)) {
            inputState.HandleEvent(event);
            const std::size_t messageCountBefore = demoChatBox.GetMessages().size();
            const bool chatBoxConsumed = demoChatBox.HandleEvent(event);

            if (event.type == cg::platform::EventType::Quit) {
                running = false;
            }

            if (event.type == cg::platform::EventType::WindowCloseRequested &&
                event.windowId == window.GetId()) {
                running = false;
            }

            if (demoChatBox.GetMessages().size() != messageCountBefore) {
                const auto& messages = demoChatBox.GetMessages();
                cg::core::Log(cg::core::LogLevel::Info,
                    "ui",
                    "ChatBox submitted: " + messages.back().text);
            }

            if (chatBoxConsumed && event.type == cg::platform::EventType::TextEditing) {
                cg::core::Log(cg::core::LogLevel::Info,
                    "ui",
                    "InputBox composition: text=" + std::string{demoInputBox.GetCompositionText()} +
                        ", cursor=" + std::to_string(demoInputBox.GetCompositionCursor()) +
                        ", selection=" + std::to_string(demoInputBox.GetCompositionSelectionLength()) +
                        ", display=" + demoInputBox.GetDisplayText());
            }

            if (chatBoxConsumed && event.type == cg::platform::EventType::TextEditingCandidates) {
                cg::core::Log(cg::core::LogLevel::Info,
                    "ui",
                    "InputBox candidates: count=" + std::to_string(demoInputBox.GetCandidates().size()) +
                        ", selected=" + std::to_string(demoInputBox.GetSelectedCandidate()) +
                        ", direction=" +
                        std::string{ToCandidateDirectionName(demoInputBox.AreCandidatesHorizontal())} +
                        ", mode=" +
                        std::string{ToCandidateDisplayModeName(demoInputBox.GetCandidateDisplayMode())});
            }
        }

        renderer.ResetStats();
        if (!renderer.BeginFrame({0.07F, 0.08F, 0.09F, 1.0F})) {
            cg::core::LogSdlError("render", "BeginFrame");
            break;
        }

        renderQueue.Clear();
        const auto testSprite = testAtlas.MakeSprite("m2-test-region", {64.0F, 64.0F, 192.0F, 192.0F});
        if (!testSprite.has_value()) {
            cg::core::Log(cg::core::LogLevel::Error, "render", "TextureAtlas::MakeSprite failed");
            break;
        }
        renderQueue.PushSprite(*testSprite);
        if (demoTextTexture.texture.IsValid()) {
            renderQueue.PushSprite({
                demoTextTexture.texture,
                {0.0F, 0.0F, static_cast<float>(demoTextTexture.width), static_cast<float>(demoTextTexture.height)},
                {64.0F, 280.0F, static_cast<float>(demoTextTexture.width), static_cast<float>(demoTextTexture.height)},
                {1.0F, 1.0F, 1.0F, 1.0F},
                1,
                0,
                1,
            });
        }
        if (!renderQueue.Flush(renderer)) {
            cg::core::LogSdlError("render", "RenderQueue::Flush");
            break;
        }

        if (!renderer.EndFrame()) {
            cg::core::LogSdlError("render", "EndFrame");
            break;
        }

        ++framesSinceStatsLog;
        if (framesSinceStatsLog >= StatsLogFrameInterval) {
            const auto stats = renderer.GetStats();
            cg::core::Log(cg::core::LogLevel::Info,
                "render",
                "Render stats: drawCalls=" + std::to_string(stats.drawCalls) +
                    ", sprites=" + std::to_string(stats.sprites) +
                    ", textureSwitches=" + std::to_string(stats.textureSwitches));
            const auto visibleMessages = demoChatBox.GetVisibleMessages(5);
            cg::core::Log(cg::core::LogLevel::Info,
                "ui",
                "ChatBox demo: history=" + std::to_string(demoChatBox.GetMessages().size()) +
                    ", visible=" + std::to_string(visibleMessages.size()) +
                    ", scroll=" + std::to_string(demoChatBox.GetScrollOffset()) +
                    ", focused=" + std::to_string(demoInputBox.IsFocused() ? 1 : 0) +
                    ", cursor=" + std::to_string(demoInputBox.GetCursorByteOffset()) +
                    ", text=" + std::string{demoInputBox.GetText()} +
                    ", composition=" + std::string{demoInputBox.GetCompositionText()} +
                    ", compositionCursor=" + std::to_string(demoInputBox.GetCompositionCursor()) +
                    ", compositionSelection=" +
                        std::to_string(demoInputBox.GetCompositionSelectionLength()) +
                    ", display=" + demoInputBox.GetDisplayText() +
                    ", candidateMode=" +
                    std::string{ToCandidateDisplayModeName(demoInputBox.GetCandidateDisplayMode())} +
                    ", candidates=" + std::to_string(demoInputBox.GetCandidates().size()) +
                    ", selectedCandidate=" + std::to_string(demoInputBox.GetSelectedCandidate()) +
                    ", candidateDirection=" +
                    std::string{ToCandidateDirectionName(demoInputBox.AreCandidatesHorizontal())});
            framesSinceStatsLog = 0;
        }

        inputState.EndFrame();
        frameTimer.DelayToCap(60.0);
    }

    if (!imeController.EndTextInput()) {
        cg::core::LogSdlError("ime", "SDL_StopTextInput");
    }

    fontCache.Clear();
    textureCache.Clear();
    renderer.Shutdown();
    window.Destroy();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
