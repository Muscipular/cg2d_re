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
#include "resource/MapData.hpp"
#include "resource/ResourceTypes.hpp"
#include "text/TextEncoding.hpp"
#include "ui/ChatBox.hpp"
#include "ui/TextLayout.hpp"
#include "world/EntitySpriteBuilder.hpp"
#include "world/MapObjectTextureCache.hpp"
#include "world/SceneGroundAtlasCache.hpp"
#include "world/WorldRenderer.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <array>
#include <cstdint>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <vector>

namespace {

constexpr int WindowWidth = 1280;
constexpr int WindowHeight = 720;
constexpr int TestTextureSize = 64;
constexpr int DemoTileWidth = 64;
constexpr int DemoTileHeight = 48;
constexpr std::uint64_t DemoWalkLegMs = 2000;
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

cg::resource::DecodedImage CreateSolidImage(
    int width,
    int height,
    std::uint8_t red,
    std::uint8_t green,
    std::uint8_t blue,
    int offsetX,
    int offsetY)
{
    cg::resource::DecodedImage image;
    image.width = width;
    image.height = height;
    image.offsetX = offsetX;
    image.offsetY = offsetY;
    image.format = cg::render::PixelFormat::Rgba8;
    image.pixels.resize(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4U);
    for (std::size_t index = 0; index < static_cast<std::size_t>(width) * static_cast<std::size_t>(height); ++index) {
        const std::size_t offset = index * 4U;
        image.pixels[offset + 0U] = red;
        image.pixels[offset + 1U] = green;
        image.pixels[offset + 2U] = blue;
        image.pixels[offset + 3U] = 0xFFU;
    }

    return image;
}

cg::resource::MapData CreateDemoMapData()
{
    cg::resource::MapData map;
    map.mapId = 1U;
    map.serverNo = 1U;
    map.floor = 1U;
    map.width = 8U;
    map.height = 8U;
    map.tileData.reserve(map.CellCount());
    for (std::size_t index = 0; index < map.CellCount(); ++index) {
        const std::uint16_t x = static_cast<std::uint16_t>(index % map.width);
        const std::uint16_t y = static_cast<std::uint16_t>(index / map.width);
        if ((x + y) % 5U == 0U) {
            map.tileData.push_back(5000U);
        } else if ((x + y) % 2U == 0U) {
            map.tileData.push_back(1000U);
        } else {
            map.tileData.push_back(1001U);
        }
    }

    return map;
}

cg::resource::AnimeFrame CreateDemoAnimeFrame(std::int32_t graphicSeqNo, std::int16_t x, std::int16_t y)
{
    cg::resource::AnimeFrame frame;
    frame.graphicSeqNo = graphicSeqNo;
    frame.x = x;
    frame.y = y;
    return frame;
}

cg::resource::AnimeAction CreateDemoAnimeAction(
    cg::world::Direction direction,
    int frameIndex,
    std::int32_t frameCount)
{
    cg::resource::AnimeAction action;
    action.header.action = cg::resource::CG_ACTIONTYPE_WALK;
    action.header.dir = static_cast<cg::resource::CG_Direction>(static_cast<int>(direction));
    action.header.duration = 480;
    action.header.frameCount = frameCount;
    action.frameIndex = frameIndex;
    return action;
}

cg::resource::AnimationClip CreateDemoWalkClip()
{
    cg::resource::AnimationClip clip;
    clip.actions.push_back(CreateDemoAnimeAction(cg::world::Direction::East, 0, 4));
    clip.actions.push_back(CreateDemoAnimeAction(cg::world::Direction::West, 4, 4));
    clip.frames.push_back(CreateDemoAnimeFrame(1, 0, 0));
    clip.frames.push_back(CreateDemoAnimeFrame(1, 2, -1));
    clip.frames.push_back(CreateDemoAnimeFrame(1, 0, 0));
    clip.frames.push_back(CreateDemoAnimeFrame(1, -2, -1));
    clip.frames.push_back(CreateDemoAnimeFrame(1, 0, 0));
    clip.frames.push_back(CreateDemoAnimeFrame(1, -2, -1));
    clip.frames.push_back(CreateDemoAnimeFrame(1, 0, 0));
    clip.frames.push_back(CreateDemoAnimeFrame(1, 2, -1));
    return clip;
}

class DemoGroundTileImageProvider final : public cg::world::IGroundTileImageProvider {
public:
    DemoGroundTileImageProvider()
    {
        images_.emplace(1000U, CreateSolidImage(DemoTileWidth, DemoTileHeight, 0x2DU, 0x74U, 0x54U, -32, -24));
        images_.emplace(1001U, CreateSolidImage(DemoTileWidth, DemoTileHeight, 0x34U, 0x84U, 0x64U, -32, -24));
        images_.emplace(5000U, CreateSolidImage(DemoTileWidth, DemoTileHeight, 0x78U, 0x67U, 0x3BU, -32, -24));
    }

    [[nodiscard]] std::optional<cg::resource::DecodedImage> DecodeTile(std::uint32_t imageNo) override
    {
        const auto iter = images_.find(imageNo);
        if (iter == images_.end()) {
            return std::nullopt;
        }

        return iter->second;
    }

private:
    std::unordered_map<std::uint32_t, cg::resource::DecodedImage> images_;
};

class DemoMapObjectImageProvider final : public cg::world::IMapObjectImageProvider {
public:
    DemoMapObjectImageProvider()
    {
        images_.emplace(2000U, CreateSolidImage(48, 72, 0x64U, 0x70U, 0xB0U, -24, -66));
    }

    [[nodiscard]] std::optional<cg::resource::DecodedImage> DecodeObject(std::uint32_t imageNo) override
    {
        const auto iter = images_.find(imageNo);
        if (iter == images_.end()) {
            return std::nullopt;
        }

        return iter->second;
    }

private:
    std::unordered_map<std::uint32_t, cg::resource::DecodedImage> images_;
};

class DemoEntityGraphicResolver final : public cg::world::IEntitySpriteGraphicResolver {
public:
    explicit DemoEntityGraphicResolver(cg::render::TextureHandle texture) noexcept
        : texture_(texture)
    {
    }

    [[nodiscard]] std::optional<cg::world::EntitySpriteGraphic> ResolveEntityGraphic(std::int32_t graphicSeqNo) override
    {
        if (graphicSeqNo != 1) {
            return std::nullopt;
        }

        return cg::world::EntitySpriteGraphic{
            texture_,
            {0.0F, 0.0F, static_cast<float>(TestTextureSize), static_cast<float>(TestTextureSize)},
            TestTextureSize,
            TestTextureSize,
            -32,
            -60,
        };
    }

private:
    cg::render::TextureHandle texture_;
};

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

    DemoGroundTileImageProvider demoGroundProvider;
    DemoMapObjectImageProvider demoObjectProvider;
    cg::world::SceneGroundAtlasCache demoGroundAtlasCache(textureCache, demoGroundProvider);
    cg::world::MapObjectTextureCache demoMapObjectTextureCache(textureCache, demoObjectProvider);
    cg::world::WorldRenderer demoWorldRenderer(demoGroundAtlasCache, demoMapObjectTextureCache);
    const cg::resource::MapData demoMap = CreateDemoMapData();
    const cg::world::MapTileAtlasCacheKey demoGroundKey{
        static_cast<int>(demoMap.mapId),
        static_cast<int>(demoMap.serverNo),
        static_cast<int>(demoMap.floor),
        cg::world::MapTileAtlasAllLayerMask,
        1U,
        1U,
    };
    const std::array<cg::world::WorldMapObjectSprite, 1> demoMapObjects{
        cg::world::WorldMapObjectSprite{
            cg::world::MapObject{1, cg::world::TileCoord{4, 3}, 2000U},
            2000,
            3,
        },
    };
    const cg::resource::AnimationClip demoWalkClip = CreateDemoWalkClip();
    DemoEntityGraphicResolver demoEntityResolver(testTexture);
    cg::world::AnimationState demoEntityAnimation{};
    cg::world::WorldRenderFrame demoWorldFrame;
    std::uint64_t demoWorldTimeMs = 0U;

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

    const cg::text::EncodingResult gbkBytes =
        cg::text::Utf8ToLegacy(DemoText, cg::text::LegacyEncoding::Gbk);
    const cg::text::EncodingResult gbkRoundtrip =
        gbkBytes.ok ? cg::text::LegacyToUtf8(gbkBytes.text, cg::text::LegacyEncoding::Gbk) :
                      cg::text::EncodingResult{};
    const bool gbkRoundtripOk = gbkRoundtrip.ok && std::string_view{gbkRoundtrip.text} == DemoText;
    cg::core::Log(cg::core::LogLevel::Info,
        "text",
        "Encoding demo: gbkOk=" + std::to_string(gbkBytes.ok ? 1 : 0) +
            ", gbkBytes=" + std::to_string(gbkBytes.text.size()) +
            ", roundtripOk=" + std::to_string(gbkRoundtripOk ? 1 : 0));

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
        const std::uint64_t deltaMs =
            deltaSeconds > 0.0 ? static_cast<std::uint64_t>(deltaSeconds * 1000.0) : 0U;
        demoWorldTimeMs += deltaMs;
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
        const std::uint64_t walkPhaseMs = demoWorldTimeMs % (DemoWalkLegMs * 2U);
        const bool walkingEast = walkPhaseMs < DemoWalkLegMs;
        const std::uint64_t walkNowMs = walkingEast ? walkPhaseMs : walkPhaseMs - DemoWalkLegMs;
        const cg::world::Movement demoEntityMovement = cg::world::MakePixelMovement(
            walkingEast ? cg::world::WorldPixelCoord{-128, 0} : cg::world::WorldPixelCoord{128, 0},
            walkingEast ? cg::world::WorldPixelCoord{128, 0} : cg::world::WorldPixelCoord{-128, 0},
            0U,
            DemoWalkLegMs,
            walkingEast ? cg::world::Direction::East : cg::world::Direction::West);

        cg::world::EntitySpriteBuildInput demoEntityInput;
        demoEntityInput.id = cg::world::EntityId{1U, 1U};
        demoEntityInput.movement = &demoEntityMovement;
        demoEntityInput.animation = &demoEntityAnimation;
        demoEntityInput.clip = &demoWalkClip;
        demoEntityInput.graphicResolver = &demoEntityResolver;
        demoEntityInput.movingAction = static_cast<std::int16_t>(cg::resource::CG_ACTIONTYPE_WALK);
        demoEntityInput.originTile = cg::world::TileCoord{0, 0};
        demoEntityInput.originScreen = cg::world::ScreenPixelCoord{640, 300};
        demoEntityInput.nowTimeMs = walkNowMs;
        demoEntityInput.deltaMs = deltaMs;
        demoEntityInput.layer = 9U;
        demoEntityInput.sortY = 300;

        std::array<cg::world::WorldEntitySprite, 1> demoEntities{};
        const std::optional<cg::world::WorldEntitySprite> demoEntitySprite =
            cg::world::BuildEntitySprite(demoEntityInput);

        cg::world::WorldRenderInput demoWorldInput;
        demoWorldInput.map = &demoMap;
        demoWorldInput.groundKey = demoGroundKey;
        demoWorldInput.mapObjects = demoMapObjects;
        demoWorldInput.camera = cg::world::WorldRenderCamera{
            cg::world::TileCoord{0, 0},
            cg::world::ScreenPixelCoord{640, 300},
            cg::render::RectF{0.0F, 0.0F, static_cast<float>(WindowWidth), static_cast<float>(WindowHeight)},
        };
        demoWorldInput.nowMs = demoWorldTimeMs;
        demoWorldInput.layers = cg::world::WorldRenderLayerMap{
            6U,
            7U,
            7U,
            7U,
        };
        demoWorldInput.orderBase = cg::world::WorldRenderOrderBase{
            0U,
            1000U,
            2000U,
        };
        if (demoEntitySprite.has_value()) {
            demoEntities[0] = *demoEntitySprite;
            demoWorldInput.entities = demoEntities;
        }
        demoWorldRenderer.BuildFrame(demoWorldInput, demoWorldFrame);
        demoMapObjectTextureCache.PruneIdle(demoWorldTimeMs);

        const auto testSprite = testAtlas.MakeSprite("m2-test-region", {64.0F, 64.0F, 192.0F, 192.0F});
        if (!testSprite.has_value()) {
            cg::core::Log(cg::core::LogLevel::Error, "render", "TextureAtlas::MakeSprite failed");
            break;
        }
        renderQueue.PushSprite(*testSprite);
        for (const cg::render::SpriteCmd& worldSprite : demoWorldFrame.sprites) {
            renderQueue.PushSprite(worldSprite);
        }
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
