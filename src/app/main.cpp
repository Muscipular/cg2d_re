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
#include "render/TextureCache.hpp"
#include "resource/BinSetConfigLoader.hpp"
#include "resource/MapData.hpp"
#include "resource/MapLoader.hpp"
#include "resource/ResourceManager.hpp"
#include "resource/ResourceTypes.hpp"
#include "scene/SceneManager.hpp"
#include "scene/WorldScene.hpp"
#include "ui/GraphicalUiPresenter.hpp"
#include "ui/SdlTextUiPresenter.hpp"
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
#include <unordered_set>
#include <utility>
#include <vector>

namespace {

constexpr int WindowWidth = 1280;
constexpr int WindowHeight = 720;
constexpr std::uint64_t WalkLegDurationMs = 2000;
constexpr std::uint32_t StatsLogFrameInterval = 120;
constexpr std::uint32_t InitialMapId = 0U;
constexpr std::uint16_t InitialServerNo = 0U;
constexpr std::uint16_t InitialFloor = 1000U;
constexpr int InitialPlayerAnimeNo = 110300;
constexpr std::size_t DefaultMapObjectTextureCacheCapacity = 100U;
constexpr std::string_view ClientUiFontKey = "client-ui-font";
constexpr std::array<std::string_view, 3> DefaultUiFontPaths = {
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
    sceneManager.RegisterRootSceneFactory(cg::scene::RootSceneId::World, [worldRenderProvider]() {
        auto scene = std::make_unique<cg::scene::WorldScene>();
        if (worldRenderProvider) {
            (void)scene->CreateEntity();
            scene->SetRenderProvider(worldRenderProvider);
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

[[nodiscard]] std::optional<std::filesystem::path> ResolveResourceRoot(int argc, char* argv[])
{
    if (argc >= 2 && argv[1] != nullptr && std::string_view{argv[1]} != "--help") {
        return PathFromUtf8(argv[1]);
    }

    return std::nullopt;
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

[[nodiscard]] bool TryLoadInitialWorldScene(
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
        cg::resource::BuildMapFloorPath(resourceRoot, InitialMapId, InitialServerNo, InitialFloor);
    const cg::resource::MapLoadResult mapResult =
        cg::resource::LoadMapData(mapPath, InitialMapId, InitialServerNo, InitialFloor);
    if (!mapResult.IsOk()) {
        LogResourceError("初始地图加载失败", mapResult.error);
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

    std::optional<cg::resource::AnimationClip> walkClip = resources.LoadAnimation(InitialPlayerAnimeNo);
    if (!walkClip.has_value()) {
        LogResourceError("初始角色动画加载失败", resources.LastError());
        return false;
    }

    scene.map = mapResult.map;
    scene.mapObjects = BuildWorldMapObjects(scene.map, resources);
    scene.walkClip = std::move(*walkClip);
    scene.realResources = true;
    cg::core::Log(
        cg::core::LogLevel::Info,
        "world",
        "Loaded world resources: root=" + PathToUtf8(resourceRoot) +
            ", mapId=" + std::to_string(InitialMapId) +
            ", serverNo=" + std::to_string(InitialServerNo) +
            ", floor=" + std::to_string(InitialFloor) +
            ", size=" + std::to_string(scene.map.width) + "x" + std::to_string(scene.map.height) +
            ", mapNos=" + std::to_string(worldMapNos.size()) +
            ", objects=" + std::to_string(scene.mapObjects.size()) +
            ", animeNo=" + std::to_string(InitialPlayerAnimeNo));
    return true;
}

class ResourceGroundTileImageProvider final : public cg::world::IGroundTileImageProvider {
public:
    explicit ResourceGroundTileImageProvider(cg::resource::ResourceManager& resources) noexcept
        : resources_(resources)
    {
    }

    [[nodiscard]] std::optional<cg::resource::DecodedImage> DecodeTile(std::uint32_t imageNo) override
    {
        if (imageNo > static_cast<std::uint32_t>(std::numeric_limits<int>::max())) {
            return std::nullopt;
        }

        return resources_.DecodeGraphicByMapNo(static_cast<int>(imageNo));
    }

private:
    cg::resource::ResourceManager& resources_;
};

class ResourceMapObjectImageProvider final : public cg::world::IMapObjectImageProvider {
public:
    explicit ResourceMapObjectImageProvider(cg::resource::ResourceManager& resources) noexcept
        : resources_(resources)
    {
    }

    [[nodiscard]] std::optional<cg::resource::DecodedImage> DecodeObject(std::uint32_t imageNo) override
    {
        if (imageNo > static_cast<std::uint32_t>(std::numeric_limits<int>::max())) {
            return std::nullopt;
        }

        return resources_.DecodeGraphicByMapNo(static_cast<int>(imageNo));
    }

private:
    cg::resource::ResourceManager& resources_;
};

class EntityGraphicResolver final : public cg::world::IEntitySpriteGraphicResolver {
public:
    EntityGraphicResolver(
        cg::render::TextureCache& textureCache,
        cg::resource::ResourceManager& resources,
        const cg::resource::AnimationClip* clip) noexcept
        : textureCache_(textureCache)
        , resources_(resources)
        , clip_(clip)
    {
    }

    [[nodiscard]] std::optional<cg::world::EntitySpriteGraphic> ResolveEntityGraphic(
        std::int32_t graphicSeqNo,
        std::int16_t paletteIndex) override
    {
        if (clip_ == nullptr) {
            return std::nullopt;
        }

        const std::optional<cg::resource::GraphicRef> graphic =
            resources_.FindGraphicBySeq(clip_->binSetIndex, graphicSeqNo);
        if (!graphic.has_value()) {
            return std::nullopt;
        }

        std::optional<cg::resource::Palette> palette;
        if (paletteIndex > 0) {
            palette = resources_.LoadHiddenPaletteByPalIndex(paletteIndex, clip_->sourceAnimeNo);
        }
        std::optional<cg::resource::DecodedImage> image =
            palette.has_value() ? resources_.DecodeGraphic(*graphic, *palette) : resources_.DecodeGraphic(*graphic);
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

private:
    cg::render::TextureCache& textureCache_;
    cg::resource::ResourceManager& resources_;
    const cg::resource::AnimationClip* clip_ = nullptr;
};

class WorldRenderDriver final {
public:
    WorldRenderDriver(
        WorldSceneData& scene,
        cg::world::WorldRenderer& worldRenderer,
        cg::world::MapObjectTextureCache& mapObjectTextureCache,
        EntityGraphicResolver& entityResolver,
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

        const std::uint64_t walkPhaseMs = timeMs_ % (WalkLegDurationMs * 2U);
        const bool walkingEast = walkPhaseMs < WalkLegDurationMs;
        const std::uint64_t walkNowMs = walkingEast ? walkPhaseMs : walkPhaseMs - WalkLegDurationMs;
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
            WalkLegDurationMs,
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
    EntityGraphicResolver& entityResolver_;
    cg::world::MapTileAtlasCacheKey groundKey_{};
    cg::world::AnimationState entityAnimation_{};
    cg::world::WorldRenderFrame frame_;
    std::uint64_t timeMs_ = 0U;
};

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
    if (!window.Create("cg-client", WindowWidth, WindowHeight, SDL_WINDOW_RESIZABLE)) {
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

    std::unique_ptr<cg::resource::ResourceManager> clientResources;
    std::unique_ptr<WorldSceneData> loadedWorldScene;
    std::unique_ptr<ResourceGroundTileImageProvider> groundProvider;
    std::unique_ptr<ResourceMapObjectImageProvider> objectProvider;
    std::unique_ptr<cg::world::SceneGroundAtlasCache> groundAtlasCache;
    std::unique_ptr<cg::world::MapObjectTextureCache> mapObjectTextureCache;
    std::unique_ptr<cg::world::WorldRenderer> worldRenderer;
    std::unique_ptr<EntityGraphicResolver> entityResolver;
    std::unique_ptr<WorldRenderDriver> worldDriver;
    cg::scene::WorldScene::RenderProvider worldRenderProvider;

    const std::optional<std::filesystem::path> resourceRoot = ResolveResourceRoot(argc, argv);
    if (resourceRoot.has_value()) {
        clientResources = std::make_unique<cg::resource::ResourceManager>();
        loadedWorldScene = std::make_unique<WorldSceneData>();
        if (TryLoadInitialWorldScene(*resourceRoot, *clientResources, *loadedWorldScene)) {
            groundProvider = std::make_unique<ResourceGroundTileImageProvider>(*clientResources);
            objectProvider = std::make_unique<ResourceMapObjectImageProvider>(*clientResources);
            groundAtlasCache = std::make_unique<cg::world::SceneGroundAtlasCache>(textureCache, *groundProvider);
            mapObjectTextureCache = std::make_unique<cg::world::MapObjectTextureCache>(
                textureCache,
                *objectProvider,
                MakeMapObjectTextureCacheCapacity(loadedWorldScene->mapObjects));
            worldRenderer = std::make_unique<cg::world::WorldRenderer>(*groundAtlasCache, *mapObjectTextureCache);
            const cg::world::MapTileAtlasCacheKey groundKey{
                static_cast<int>(loadedWorldScene->map.mapId),
                static_cast<int>(loadedWorldScene->map.serverNo),
                static_cast<int>(loadedWorldScene->map.floor),
                cg::world::MapTileAtlasAllLayerMask,
                1U,
                1U,
            };
            entityResolver =
                std::make_unique<EntityGraphicResolver>(textureCache, *clientResources, &loadedWorldScene->walkClip);
            worldDriver = std::make_unique<WorldRenderDriver>(
                *loadedWorldScene,
                *worldRenderer,
                *mapObjectTextureCache,
                *entityResolver,
                groundKey);
            worldRenderProvider = [driver = worldDriver.get()](
                                      float deltaSeconds,
                                      std::vector<cg::render::SpriteCmd>& out) {
                driver->BuildFrame(deltaSeconds, out);
            };
        } else {
            loadedWorldScene.reset();
            clientResources.reset();
            cg::core::Log(
                cg::core::LogLevel::Warning,
                "world",
                "World resources unavailable: root=" + PathToUtf8(*resourceRoot));
        }
    } else {
        cg::core::Log(cg::core::LogLevel::Info, "world", "No resource root provided; world resources not loaded");
    }

    cg::game::GameStateMachine clientStateMachine;
    cg::scene::SceneManager clientSceneManager;
    cg::ui::SdlTextUiPresenter textPresenter;
    cg::ui::GraphicalUiPresenter graphicalPresenter;
    cg::ui::UiTextRenderer uiTextRenderer;
    RegisterClientRootScenes(clientSceneManager, worldRenderProvider);
    (void)clientSceneManager.Start(cg::scene::RootSceneId::Boot);

    cg::ui::UiViewModel clientModel;
    clientModel.screen = cg::ui::UiScreenType::Login;
    clientModel.title = "Scene";
    textPresenter.SetViewModel(clientModel);
    graphicalPresenter.SetViewModel(clientModel);

    const std::array<cg::game::GameEvent, 1> seedEvents{{
        {cg::game::GameEventType::AppStarted, {}, 0},
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
    std::string_view loadedFontPath;
    for (const std::string_view fontPath : DefaultUiFontPaths) {
        if (fontCache.LoadFont(ClientUiFontKey, fontPath, 28.0F)) {
            loadedFontPath = fontPath;
            break;
        }
    }
    if (!loadedFontPath.empty()) {
        cg::core::Log(cg::core::LogLevel::Info, "render", "UI font loaded: " + std::string{loadedFontPath});
    } else {
        cg::core::Log(cg::core::LogLevel::Warning, "render", "UI font not found");
    }

    cg::platform::ImeController imeController(window);
    if (!imeController.BeginTextInput()) {
        cg::core::LogSdlError("ime", "SDL_StartTextInput");
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
        }

        renderer.ResetStats();
        if (!renderer.BeginFrame({0.07F, 0.08F, 0.09F, 1.0F})) {
            cg::core::LogSdlError("render", "BeginFrame");
            break;
        }

        renderQueue.Clear();
        clientSceneManager.Update(static_cast<float>(deltaSeconds));
        clientSceneManager.Render(renderQueue);

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
            (void)uiTextRenderer.PushTextDraws(fontCache, ClientUiFontKey, tuiDraws, renderQueue);
            (void)uiTextRenderer.PushTextDraws(fontCache, ClientUiFontKey, graphicalDraws, renderQueue);
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
