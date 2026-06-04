#include "core/Log.hpp"
#include "app/SceneClientLoopBridge.hpp"
#include "game/GameStateMachine.hpp"
#include "platform/Console.hpp"
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
#include "resource/BinSetConfigLoader.hpp"
#include "resource/MapData.hpp"
#include "resource/MapLoader.hpp"
#include "resource/ResourceManager.hpp"
#include "resource/ResourceTypes.hpp"
#include "scene/SceneManager.hpp"
#include "scene/WorldScene.hpp"
#include "text/TextEncoding.hpp"
#include "ui/ChatBox.hpp"
#include "ui/GraphicalUiPresenter.hpp"
#include "ui/SdlTextUiPresenter.hpp"
#include "ui/TextLayout.hpp"
#include "ui/UiTextRenderer.hpp"
#include "world/EntitySpriteBuilder.hpp"
#include "world/IdaRenderRules.hpp"
#include "world/MapObjectTextureCache.hpp"
#include "world/SceneGroundAtlasCache.hpp"
#include "world/WorldRenderer.hpp"

#include <SDL3/SDL.h>
#include <SDL3_image/SDL_image.h>
#include <SDL3_ttf/SDL_ttf.h>

#include <algorithm>
#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <limits>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <unordered_map>
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

constexpr int WindowWidth = 1280;
constexpr int WindowHeight = 720;
constexpr int TestTextureSize = 64;
constexpr int DemoTileWidth = 64;
constexpr int DemoTileHeight = 48;
constexpr std::uint64_t DemoWalkLegMs = 2000;
constexpr std::uint32_t StatsLogFrameInterval = 120;
constexpr std::uint32_t TestMapId = 0U;
constexpr std::uint16_t TestServerNo = 0U;
constexpr std::uint16_t TestFloor = 1000U;
constexpr int TestAnimeNo = 110300;
constexpr std::size_t DefaultMapObjectTextureCacheCapacity = 100U;
constexpr std::string_view DefaultResourceRoot = "H:/Games/cgmsv_21.2a/CrossGate7.1.0";
constexpr std::string_view DemoFontKey = "demo-font";
constexpr std::string_view DemoText = "你好 CrossGate";
constexpr std::string_view DemoLayoutText = "你好 CrossGate\nText layout";
constexpr std::string_view DemoInputText = "你好 ChatBox";
constexpr std::array<std::string_view, 3> DemoFontPaths = {
    "C:/Windows/Fonts/msyh.ttc",
    "C:/Windows/Fonts/simhei.ttf",
    "C:/Windows/Fonts/simsun.ttc",
};

struct WorldSceneData {
    cg::resource::MapData map;
    std::vector<cg::world::WorldMapObjectSprite> mapObjects;
    cg::resource::AnimationClip walkClip;
    bool realResources = false;
};

class ClientRootScene final : public cg::scene::IScene {
public:
    explicit ClientRootScene(cg::scene::RootSceneId id) noexcept
        : id_(id)
    {
    }

    [[nodiscard]] cg::scene::RootSceneId Id() const noexcept override
    {
        return id_;
    }

    void Update(float) override
    {
    }

    bool HandleInput(const cg::scene::SceneInputEvent&) override
    {
        return false;
    }

    void Render(cg::render::RenderQueue&) const override
    {
    }

private:
    cg::scene::RootSceneId id_;
};

class MainHudSubScene final : public cg::scene::ISubScene {
public:
    [[nodiscard]] cg::scene::SubSceneId Id() const noexcept override
    {
        return cg::scene::SubSceneId::MainHud;
    }

    [[nodiscard]] cg::scene::SubScenePolicy Policy() const noexcept override
    {
        cg::scene::SubScenePolicy policy;
        policy.transparentRender = true;
        return policy;
    }

    void Update(float) override
    {
    }

    bool HandleInput(const cg::scene::SceneInputEvent&) override
    {
        return false;
    }

    void Render(cg::render::RenderQueue&) const override
    {
    }
};

class OverlaySubScene final : public cg::scene::ISubScene {
public:
    [[nodiscard]] cg::scene::SubSceneId Id() const noexcept override
    {
        return cg::scene::SubSceneId::LoadingOverlay;
    }

    [[nodiscard]] cg::scene::SubScenePolicy Policy() const noexcept override
    {
        cg::scene::SubScenePolicy policy;
        policy.transparentRender = true;
        return policy;
    }

    void Update(float) override
    {
    }

    bool HandleInput(const cg::scene::SceneInputEvent&) override
    {
        return false;
    }

    void Render(cg::render::RenderQueue&) const override
    {
    }
};

void RegisterClientRootScenes(
    cg::scene::SceneManager& sceneManager,
    cg::render::TextureHandle demoTexture,
    cg::scene::WorldScene::RenderProvider worldRenderProvider)
{
    sceneManager.RegisterRootSceneFactory(cg::scene::RootSceneId::Boot, []() {
        return std::make_unique<ClientRootScene>(cg::scene::RootSceneId::Boot);
    });
    sceneManager.RegisterRootSceneFactory(cg::scene::RootSceneId::Login, []() {
        return std::make_unique<ClientRootScene>(cg::scene::RootSceneId::Login);
    });
    sceneManager.RegisterRootSceneFactory(cg::scene::RootSceneId::CharacterSelect, []() {
        return std::make_unique<ClientRootScene>(cg::scene::RootSceneId::CharacterSelect);
    });
    sceneManager.RegisterRootSceneFactory(cg::scene::RootSceneId::Loading, []() {
        return std::make_unique<ClientRootScene>(cg::scene::RootSceneId::Loading);
    });
    sceneManager.RegisterRootSceneFactory(cg::scene::RootSceneId::World, [demoTexture, worldRenderProvider]() {
        auto scene = std::make_unique<cg::scene::WorldScene>();
        (void)scene->CreateDemoEntity();
        if (worldRenderProvider) {
            scene->SetRenderProvider(worldRenderProvider);
        } else {
            const std::array<cg::render::SpriteCmd, 1> sprites{{
                cg::render::SpriteCmd{
                    demoTexture,
                    {0.0F, 0.0F, static_cast<float>(TestTextureSize), static_cast<float>(TestTextureSize)},
                    {16.0F, 16.0F, 32.0F, 32.0F},
                    {1.0F, 1.0F, 1.0F, 1.0F},
                    1U,
                    0,
                    0U,
                },
            }};
            scene->SetRenderSprites(sprites);
        }
        return scene;
    });
    sceneManager.RegisterRootSceneFactory(cg::scene::RootSceneId::Battle, []() {
        return std::make_unique<ClientRootScene>(cg::scene::RootSceneId::Battle);
    });
    sceneManager.RegisterRootSceneFactory(cg::scene::RootSceneId::Disconnected, []() {
        return std::make_unique<ClientRootScene>(cg::scene::RootSceneId::Disconnected);
    });
    sceneManager.RegisterRootSceneFactory(cg::scene::RootSceneId::Shutdown, []() {
        return std::make_unique<ClientRootScene>(cg::scene::RootSceneId::Shutdown);
    });
}

void ReplaceClientSubScene(cg::scene::SceneManager& sceneManager, cg::game::GameSubSceneType subScene)
{
    if (subScene == cg::game::GameSubSceneType::MainHud) {
        sceneManager.ReplaceCurrentSubScene(std::make_unique<MainHudSubScene>());
    } else if (subScene == cg::game::GameSubSceneType::Overlay) {
        sceneManager.ReplaceCurrentSubScene(std::make_unique<OverlaySubScene>());
    }
}

[[nodiscard]] std::optional<cg::ui::UiInputEvent> ToUiInputEvent(const cg::platform::Event& event)
{
    if (event.type == cg::platform::EventType::TextInput && !event.text.empty()) {
        return cg::ui::UiInputEvent{cg::ui::UiInputEventType::TextInput, cg::ui::UiKey::None, event.text};
    }

    if (event.type != cg::platform::EventType::KeyDown || event.repeat) {
        return std::nullopt;
    }

    cg::ui::UiKey key = cg::ui::UiKey::None;
    switch (event.key) {
    case SDLK_UP:
        key = cg::ui::UiKey::Up;
        break;
    case SDLK_DOWN:
        key = cg::ui::UiKey::Down;
        break;
    case SDLK_LEFT:
        key = cg::ui::UiKey::Left;
        break;
    case SDLK_RIGHT:
        key = cg::ui::UiKey::Right;
        break;
    case SDLK_RETURN:
    case SDLK_KP_ENTER:
        key = cg::ui::UiKey::Enter;
        break;
    case SDLK_ESCAPE:
        key = cg::ui::UiKey::Escape;
        break;
    case SDLK_BACKSPACE:
        key = cg::ui::UiKey::Backspace;
        break;
    default:
        return std::nullopt;
    }

    return cg::ui::UiInputEvent{cg::ui::UiInputEventType::Key, key, {}};
}

[[nodiscard]] std::string PathToUtf8(const std::filesystem::path& path)
{
    const std::u8string value = path.u8string();
    std::string text;
    text.reserve(value.size());
    for (const char8_t ch : value) {
        text.push_back(static_cast<char>(ch));
    }
    return text;
}

[[nodiscard]] std::filesystem::path PathFromUtf8(std::string_view text)
{
    std::u8string value;
    value.reserve(text.size());
    for (const char ch : text) {
        value.push_back(static_cast<char8_t>(ch));
    }
    return std::filesystem::path(value);
}

[[nodiscard]] std::filesystem::path ResolveResourceRoot(int argc, char* argv[])
{
    if (argc >= 2 && argv[1] != nullptr && std::string_view{argv[1]} != "--help") {
        return PathFromUtf8(argv[1]);
    }

    return PathFromUtf8(DefaultResourceRoot);
}

void LogResourceError(std::string_view prefix, const cg::resource::ResourceError& error)
{
    std::string message{prefix};
    if (!error.message.empty()) {
        message += ": " + error.message;
    }
    if (!error.path.empty()) {
        message += ", path=" + PathToUtf8(error.path);
    }
    if (error.resourceId != 0) {
        message += ", resourceId=" + std::to_string(error.resourceId);
    }
    cg::core::Log(cg::core::LogLevel::Warning, "resource", message);
}

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

[[nodiscard]] std::vector<cg::world::WorldMapObjectSprite> CreateDemoMapObjects()
{
    return std::vector<cg::world::WorldMapObjectSprite>{
        cg::world::WorldMapObjectSprite{
            cg::world::MapObject{1, cg::world::TileCoord{4, 3}, 2000U},
            2000,
            3,
        },
    };
}

[[nodiscard]] WorldSceneData CreateFallbackWorldScene()
{
    WorldSceneData scene;
    scene.map = CreateDemoMapData();
    scene.mapObjects = CreateDemoMapObjects();
    scene.walkClip = CreateDemoWalkClip();
    return scene;
}

void AppendWorldMapNo(std::uint32_t mapNo, std::unordered_set<int>& seen, std::vector<int>& mapNos)
{
    if (mapNo <= 999U || mapNo > static_cast<std::uint32_t>(std::numeric_limits<int>::max())) {
        return;
    }

    const int value = static_cast<int>(mapNo);
    if (seen.insert(value).second) {
        mapNos.push_back(value);
    }
}

[[nodiscard]] std::vector<int> CollectWorldMapNos(const cg::resource::MapData& map)
{
    std::unordered_set<int> seen;
    std::vector<int> mapNos;
    const std::size_t cellCount = map.CellCount();
    const std::size_t tileCount = std::min(cellCount, map.tileData.size());
    const std::size_t objCount = std::min(cellCount, map.objData.size());
    mapNos.reserve(std::min<std::size_t>(tileCount + objCount, 4096U));

    for (std::size_t index = 0; index < tileCount; ++index) {
        AppendWorldMapNo(cg::world::DecodeMapCellForWorld(map.tileData[index]), seen, mapNos);
    }
    for (std::size_t index = 0; index < objCount; ++index) {
        AppendWorldMapNo(cg::world::DecodeMapCellForWorld(map.objData[index]), seen, mapNos);
    }
    return mapNos;
}

[[nodiscard]] std::vector<cg::world::WorldMapObjectSprite> BuildWorldMapObjects(
    const cg::resource::MapData& map,
    const cg::resource::ResourceManager& resources)
{
    std::vector<cg::world::WorldMapObjectSprite> objects;
    const std::size_t cellCount = map.CellCount();
    const std::size_t objCount = std::min(cellCount, map.objData.size());
    objects.reserve(std::min<std::size_t>(objCount, 4096U));

    for (std::size_t index = 0; index < objCount; ++index) {
        const std::uint32_t imageNo = cg::world::DecodeMapCellForWorld(map.objData[index]);
        if (imageNo > static_cast<std::uint32_t>(std::numeric_limits<int>::max())) {
            continue;
        }

        const std::optional<cg::resource::GraphicRef> graphic = resources.FindGraphicByMapNo(static_cast<int>(imageNo));
        if (!graphic.has_value()) {
            continue;
        }

        const int flag2 = cg::resource::GraphicFlag2(graphic->info);
        if (!cg::world::ClassifyMapObjectRenderGroup(static_cast<int>(imageNo), flag2).has_value()) {
            continue;
        }

        objects.push_back(cg::world::WorldMapObjectSprite{
            cg::world::MapObject{
                static_cast<int>(index),
                cg::world::TileCoord{
                    static_cast<std::int32_t>(index % map.width),
                    static_cast<std::int32_t>(index / map.width),
                },
                imageNo,
            },
            static_cast<int>(imageNo),
            flag2,
        });
    }

    return objects;
}

[[nodiscard]] std::size_t CountUniqueMapObjectImageNos(
    const std::vector<cg::world::WorldMapObjectSprite>& mapObjects)
{
    std::unordered_set<std::uint32_t> imageNos;
    imageNos.reserve(mapObjects.size());
    for (const cg::world::WorldMapObjectSprite& object : mapObjects) {
        imageNos.insert(object.object.imageNo);
    }
    return imageNos.size();
}

[[nodiscard]] std::size_t MakeMapObjectTextureCacheCapacity(
    const std::vector<cg::world::WorldMapObjectSprite>& mapObjects)
{
    return std::max(DefaultMapObjectTextureCacheCapacity, CountUniqueMapObjectImageNos(mapObjects));
}

[[nodiscard]] bool TryLoadTestWorldScene(
    const std::filesystem::path& resourceRoot,
    cg::resource::ResourceManager& resources,
    WorldSceneData& scene)
{
    const std::filesystem::path ymlPath = resourceRoot / "cgmsv.yml";
    const std::filesystem::path palettePath = resourceRoot / "3.x" / "bin" / "pal" / "palet_00.cgp";
    const cg::resource::BinSetConfigLoadResult config = cg::resource::LoadBinSetConfig(ymlPath);
    if (!config.IsOk()) {
        LogResourceError("cgmsv.yml 加载失败", config.error);
        return false;
    }

    const std::filesystem::path mapPath =
        cg::resource::BuildMapFloorPath(resourceRoot, TestMapId, TestServerNo, TestFloor);
    const cg::resource::MapLoadResult mapResult =
        cg::resource::LoadMapData(mapPath, TestMapId, TestServerNo, TestFloor);
    if (!mapResult.IsOk()) {
        LogResourceError("测试地图加载失败", mapResult.error);
        return false;
    }
    const std::vector<int> worldMapNos = CollectWorldMapNos(mapResult.map);

    cg::resource::ResourceLoadOptions loadOptions;
    loadOptions.buildAutoMapColorCache = false;
    loadOptions.buildGraphicGlobalIndex = false;
    loadOptions.hiddenPaletteTablePath = resourceRoot / "data" / "CGLoadSpr.dbf";
    if (!resources.LoadBinSets(config.binSets, palettePath, loadOptions)) {
        LogResourceError("binSet 加载失败", resources.LastError());
        return false;
    }

    std::optional<cg::resource::AnimationClip> walkClip = resources.LoadAnimation(TestAnimeNo);
    if (!walkClip.has_value()) {
        LogResourceError("测试 Anime 加载失败", resources.LastError());
        return false;
    }

    scene.map = mapResult.map;
    scene.mapObjects = BuildWorldMapObjects(scene.map, resources);
    scene.walkClip = std::move(*walkClip);
    scene.realResources = true;
    cg::core::Log(
        cg::core::LogLevel::Info,
        "world",
        "Loaded test world: root=" + PathToUtf8(resourceRoot) +
            ", mapId=" + std::to_string(TestMapId) +
            ", serverNo=" + std::to_string(TestServerNo) +
            ", floor=" + std::to_string(TestFloor) +
            ", size=" + std::to_string(scene.map.width) + "x" + std::to_string(scene.map.height) +
            ", mapNos=" + std::to_string(worldMapNos.size()) +
            ", objects=" + std::to_string(scene.mapObjects.size()) +
            ", animeNo=" + std::to_string(TestAnimeNo));
    return true;
}

class DemoGroundTileImageProvider final : public cg::world::IGroundTileImageProvider {
public:
    explicit DemoGroundTileImageProvider(cg::resource::ResourceManager* resources = nullptr)
        : resources_(resources)
    {
        images_.emplace(1000U, CreateSolidImage(DemoTileWidth, DemoTileHeight, 0x2DU, 0x74U, 0x54U, -32, -24));
        images_.emplace(1001U, CreateSolidImage(DemoTileWidth, DemoTileHeight, 0x34U, 0x84U, 0x64U, -32, -24));
        images_.emplace(5000U, CreateSolidImage(DemoTileWidth, DemoTileHeight, 0x78U, 0x67U, 0x3BU, -32, -24));
    }

    [[nodiscard]] std::optional<cg::resource::DecodedImage> DecodeTile(std::uint32_t imageNo) override
    {
        if (resources_ != nullptr && imageNo <= static_cast<std::uint32_t>(std::numeric_limits<int>::max())) {
            return resources_->DecodeGraphicByMapNo(static_cast<int>(imageNo));
        }

        const auto iter = images_.find(imageNo);
        if (iter == images_.end()) {
            return std::nullopt;
        }

        return iter->second;
    }

private:
    cg::resource::ResourceManager* resources_ = nullptr;
    std::unordered_map<std::uint32_t, cg::resource::DecodedImage> images_;
};

class DemoMapObjectImageProvider final : public cg::world::IMapObjectImageProvider {
public:
    explicit DemoMapObjectImageProvider(cg::resource::ResourceManager* resources = nullptr)
        : resources_(resources)
    {
        images_.emplace(2000U, CreateSolidImage(48, 72, 0x64U, 0x70U, 0xB0U, -24, -66));
    }

    [[nodiscard]] std::optional<cg::resource::DecodedImage> DecodeObject(std::uint32_t imageNo) override
    {
        if (resources_ != nullptr && imageNo <= static_cast<std::uint32_t>(std::numeric_limits<int>::max())) {
            return resources_->DecodeGraphicByMapNo(static_cast<int>(imageNo));
        }

        const auto iter = images_.find(imageNo);
        if (iter == images_.end()) {
            return std::nullopt;
        }

        return iter->second;
    }

private:
    cg::resource::ResourceManager* resources_ = nullptr;
    std::unordered_map<std::uint32_t, cg::resource::DecodedImage> images_;
};

class DemoEntityGraphicResolver final : public cg::world::IEntitySpriteGraphicResolver {
public:
    DemoEntityGraphicResolver(
        cg::render::TextureHandle texture,
        cg::render::TextureCache& textureCache,
        cg::resource::ResourceManager* resources,
        const cg::resource::AnimationClip* clip) noexcept
        : texture_(texture)
        , textureCache_(textureCache)
        , resources_(resources)
        , clip_(clip)
    {
    }

    [[nodiscard]] std::optional<cg::world::EntitySpriteGraphic> ResolveEntityGraphic(
        std::int32_t graphicSeqNo,
        std::int16_t paletteIndex) override
    {
        if (resources_ != nullptr && clip_ != nullptr) {
            const std::optional<cg::resource::GraphicRef> graphic =
                resources_->FindGraphicBySeq(clip_->binSetIndex, graphicSeqNo);
            if (!graphic.has_value()) {
                return std::nullopt;
            }

            std::optional<cg::resource::Palette> palette;
            if (paletteIndex > 0) {
                palette = resources_->LoadHiddenPaletteByPalIndex(paletteIndex);
            }
            std::optional<cg::resource::DecodedImage> image =
                palette.has_value() ? resources_->DecodeGraphic(*graphic, *palette) : resources_->DecodeGraphic(*graphic);
            if (!image.has_value()
                || image->format != cg::render::PixelFormat::Rgba8
                || image->width <= 0
                || image->height <= 0
                || image->pixels.empty()) {
                return std::nullopt;
            }

            const std::string key =
                "entity-graphic:binSet=" + std::to_string(graphic->binSetIndex) +
                ";seqNo=" + std::to_string(graphic->seqNo) +
                ";pal=" + std::to_string(palette.has_value() ? paletteIndex : 0);
            const cg::render::TextureHandle texture =
                textureCache_.LoadRgba8(key, image->width, image->height, image->pixels.data(), image->width * 4);
            if (!texture.IsValid()) {
                return std::nullopt;
            }

            return cg::world::EntitySpriteGraphic{
                texture,
                {0.0F, 0.0F, static_cast<float>(image->width), static_cast<float>(image->height)},
                image->width,
                image->height,
                image->offsetX,
                image->offsetY,
            };
        }

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
    cg::render::TextureCache& textureCache_;
    cg::resource::ResourceManager* resources_ = nullptr;
    const cg::resource::AnimationClip* clip_ = nullptr;
};

class DemoWorldRenderDriver final {
public:
    DemoWorldRenderDriver(
        WorldSceneData& scene,
        cg::world::WorldRenderer& worldRenderer,
        cg::world::MapObjectTextureCache& mapObjectTextureCache,
        DemoEntityGraphicResolver& entityResolver,
        cg::world::MapTileAtlasCacheKey groundKey) noexcept
        : scene_(scene)
        , worldRenderer_(worldRenderer)
        , mapObjectTextureCache_(mapObjectTextureCache)
        , entityResolver_(entityResolver)
        , groundKey_(groundKey)
    {
    }

    void BuildFrame(float deltaSeconds, std::vector<cg::render::SpriteCmd>& out)
    {
        const std::uint64_t deltaMs = deltaSeconds > 0.0F ? static_cast<std::uint64_t>(deltaSeconds * 1000.0F) : 0U;
        timeMs_ += deltaMs;

        const std::uint64_t walkPhaseMs = timeMs_ % (DemoWalkLegMs * 2U);
        const bool walkingEast = walkPhaseMs < DemoWalkLegMs;
        const std::uint64_t walkNowMs = walkingEast ? walkPhaseMs : walkPhaseMs - DemoWalkLegMs;
        const cg::world::TileCoord originTile{
            static_cast<std::int32_t>(scene_.map.width / 2U),
            static_cast<std::int32_t>(scene_.map.height / 2U),
        };
        const cg::world::WorldPixelCoord originWorld = cg::world::TileToWorldPixel(originTile);
        const cg::world::Movement movement = cg::world::MakePixelMovement(
            walkingEast ? cg::world::WorldPixelCoord{originWorld.x - 128, originWorld.y} :
                          cg::world::WorldPixelCoord{originWorld.x + 128, originWorld.y},
            walkingEast ? cg::world::WorldPixelCoord{originWorld.x + 128, originWorld.y} :
                          cg::world::WorldPixelCoord{originWorld.x - 128, originWorld.y},
            0U,
            DemoWalkLegMs,
            walkingEast ? cg::world::Direction::East : cg::world::Direction::West);

        cg::world::EntitySpriteBuildInput entityInput;
        entityInput.id = cg::world::EntityId{1U, 1U};
        entityInput.movement = &movement;
        entityInput.animation = &entityAnimation_;
        entityInput.clip = &scene_.walkClip;
        entityInput.graphicResolver = &entityResolver_;
        entityInput.movingAction = static_cast<std::int16_t>(cg::resource::CG_ACTIONTYPE_WALK);
        entityInput.originTile = originTile;
        entityInput.originScreen = cg::world::ScreenPixelCoord{640, 300};
        entityInput.nowTimeMs = walkNowMs;
        entityInput.deltaMs = deltaMs;
        entityInput.layer = 9U;
        entityInput.sortY = 300;

        std::array<cg::world::WorldEntitySprite, 1> entities{};
        const std::optional<cg::world::WorldEntitySprite> entitySprite = cg::world::BuildEntitySprite(entityInput);

        cg::world::WorldRenderInput input;
        input.map = &scene_.map;
        input.groundKey = groundKey_;
        input.mapObjects = scene_.mapObjects;
        input.camera = cg::world::WorldRenderCamera{
            originTile,
            cg::world::ScreenPixelCoord{640, 300},
            cg::render::RectF{0.0F, 0.0F, static_cast<float>(WindowWidth), static_cast<float>(WindowHeight)},
        };
        input.nowMs = timeMs_;
        input.layers = cg::world::WorldRenderLayerMap{6U, 7U, 7U, 7U};
        input.orderBase = cg::world::WorldRenderOrderBase{0U, 1000U, 2000U};
        if (entitySprite.has_value()) {
            entities[0] = *entitySprite;
            input.entities = entities;
        }

        worldRenderer_.BuildFrame(input, frame_);
        mapObjectTextureCache_.PruneIdle(timeMs_);
        out = frame_.sprites;
    }

private:
    WorldSceneData& scene_;
    cg::world::WorldRenderer& worldRenderer_;
    cg::world::MapObjectTextureCache& mapObjectTextureCache_;
    DemoEntityGraphicResolver& entityResolver_;
    cg::world::MapTileAtlasCacheKey groundKey_{};
    cg::world::AnimationState entityAnimation_{};
    cg::world::WorldRenderFrame frame_;
    std::uint64_t timeMs_ = 0U;
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
    cg::platform::ConfigureConsoleUtf8();

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

    const std::filesystem::path resourceRoot = ResolveResourceRoot(argc, argv);
    cg::resource::ResourceManager testResources;
    WorldSceneData worldScene = CreateFallbackWorldScene();
    const bool realWorldLoaded = TryLoadTestWorldScene(resourceRoot, testResources, worldScene);
    if (!realWorldLoaded) {
        cg::core::Log(
            cg::core::LogLevel::Warning,
            "world",
            "Use fallback in-memory world. Pass resource root as argv[1] or keep default root valid: " +
                PathToUtf8(resourceRoot));
    }

    DemoGroundTileImageProvider demoGroundProvider(realWorldLoaded ? &testResources : nullptr);
    DemoMapObjectImageProvider demoObjectProvider(realWorldLoaded ? &testResources : nullptr);
    cg::world::SceneGroundAtlasCache demoGroundAtlasCache(textureCache, demoGroundProvider);
    cg::world::MapObjectTextureCache demoMapObjectTextureCache(
        textureCache,
        demoObjectProvider,
        MakeMapObjectTextureCacheCapacity(worldScene.mapObjects));
    cg::world::WorldRenderer demoWorldRenderer(demoGroundAtlasCache, demoMapObjectTextureCache);
    const cg::world::MapTileAtlasCacheKey demoGroundKey{
        static_cast<int>(worldScene.map.mapId),
        static_cast<int>(worldScene.map.serverNo),
        static_cast<int>(worldScene.map.floor),
        cg::world::MapTileAtlasAllLayerMask,
        1U,
        1U,
    };
    DemoEntityGraphicResolver demoEntityResolver(
        testTexture,
        textureCache,
        realWorldLoaded ? &testResources : nullptr,
        &worldScene.walkClip);
    DemoWorldRenderDriver demoWorldDriver(
        worldScene,
        demoWorldRenderer,
        demoMapObjectTextureCache,
        demoEntityResolver,
        demoGroundKey);

    cg::game::GameStateMachine clientStateMachine;
    cg::scene::SceneManager clientSceneManager;
    cg::ui::SdlTextUiPresenter textPresenter;
    cg::ui::GraphicalUiPresenter graphicalPresenter;
    cg::ui::UiTextRenderer uiTextRenderer;
    RegisterClientRootScenes(clientSceneManager, testTexture, [&demoWorldDriver](float deltaSeconds, std::vector<cg::render::SpriteCmd>& out) {
        demoWorldDriver.BuildFrame(deltaSeconds, out);
    });
    (void)clientSceneManager.Start(cg::scene::RootSceneId::Boot);

    cg::ui::UiViewModel clientModel;
    clientModel.screen = cg::ui::UiScreenType::Login;
    clientModel.title = "Scene";
    textPresenter.SetViewModel(clientModel);
    graphicalPresenter.SetViewModel(clientModel);

    const std::array<cg::game::GameEvent, 10> seedEvents{{
        {cg::game::GameEventType::AppStarted, {}, 0},
        {cg::game::GameEventType::LoginSubmitted, "demo-login", 0},
        {cg::game::GameEventType::LoginAccepted, {}, 0},
        {cg::game::GameEventType::CharacterListReceived, {}, 0},
        {cg::game::GameEventType::CharacterSelected, "demo-character", 0},
        {cg::game::GameEventType::MapReady, "demo-map", 0},
        {cg::game::GameEventType::ObjectListReceived, "demo-objects", 0},
        {cg::game::GameEventType::BattleStarted, {}, 0},
        {cg::game::GameEventType::BattleEnded, {}, 0},
        {cg::game::GameEventType::ChatMessageReceived, "Scene loop ready", 0},
    }};
    for (const cg::game::GameEvent& seedEvent : seedEvents) {
        (void)cg::app::DriveGameEvent(
            clientStateMachine,
            clientSceneManager,
            textPresenter,
            seedEvent,
            ReplaceClientSubScene);
    }
    graphicalPresenter.SetViewModel(textPresenter.ViewModel());

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
        inputState.BeginFrame();

        cg::platform::Event event;
        while (eventPump.Poll(event)) {
            inputState.HandleEvent(event);
            const std::size_t messageCountBefore = demoChatBox.GetMessages().size();
            const bool chatBoxConsumed = demoChatBox.HandleEvent(event);

            if (event.type == cg::platform::EventType::Quit) {
                (void)cg::app::DriveGameEvent(
                    clientStateMachine,
                    clientSceneManager,
                    textPresenter,
                    {cg::game::GameEventType::QuitRequested, {}, 0},
                    ReplaceClientSubScene);
                running = false;
            }

            if (event.type == cg::platform::EventType::WindowCloseRequested &&
                event.windowId == window.GetId()) {
                (void)cg::app::DriveGameEvent(
                    clientStateMachine,
                    clientSceneManager,
                    textPresenter,
                    {cg::game::GameEventType::QuitRequested, {}, 0},
                    ReplaceClientSubScene);
                running = false;
            }

            const std::optional<cg::ui::UiInputEvent> uiInput = ToUiInputEvent(event);
            if (uiInput.has_value()) {
                (void)cg::app::DriveUiInput(
                    clientStateMachine,
                    clientSceneManager,
                    textPresenter,
                    *uiInput,
                    ReplaceClientSubScene);
                if (clientStateMachine.State().type == cg::game::GameStateType::Shutdown) {
                    running = false;
                }
            }
            graphicalPresenter.SetViewModel(textPresenter.ViewModel());

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
        clientSceneManager.Update(static_cast<float>(deltaSeconds));
        clientSceneManager.Render(renderQueue);

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
        if (!loadedFontPath.empty()) {
            const std::vector<cg::ui::UiTextDraw> tuiDraws = textPresenter.BuildTextDraws();
            const cg::ui::GraphicalUiLayout graphicalLayout{
                760.0F,
                32.0F,
                28.0F,
                120U,
                0U,
                {1.0F, 0.92F, 0.68F, 1.0F},
                {0.78F, 0.88F, 1.0F, 1.0F},
            };
            const std::vector<cg::ui::UiTextDraw> graphicalDraws = graphicalPresenter.BuildTextDraws(graphicalLayout);
            (void)uiTextRenderer.PushTextDraws(fontCache, DemoFontKey, tuiDraws, renderQueue);
            (void)uiTextRenderer.PushTextDraws(fontCache, DemoFontKey, graphicalDraws, renderQueue);
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

    uiTextRenderer.Clear();
    fontCache.Clear();
    textureCache.Clear();
    renderer.Shutdown();
    window.Destroy();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
