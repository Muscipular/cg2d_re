#include "world/WorldRenderer.hpp"

#include "render/IRenderer.hpp"
#include "render/TextureCache.hpp"
#include "resource/ResourceTypes.hpp"
#include "world/EntityData.hpp"
#include "world/EntitySpriteBuilder.hpp"
#include "world/EntityStore.hpp"
#include "world/IdaRenderRules.hpp"
#include "world/MapObjectTextureCache.hpp"
#include "world/SceneGroundAtlasCache.hpp"

#include <algorithm>
#include <array>
#include <climits>
#include <cstddef>
#include <cstdint>
#include <cstring>
#include <iostream>
#include <optional>
#include <type_traits>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

template <typename T>
concept HasObjectIndex = requires(T value) {
    value.objectIndex;
};

struct TextureUpload {
    cg::render::TextureHandle texture;
    cg::render::TextureDesc desc;
    int pitch = 0;
    std::vector<std::uint8_t> pixels;
};

class FakeRenderer final : public cg::render::IRenderer {
public:
    [[nodiscard]] bool BeginFrame(cg::render::ClearColor) override
    {
        return true;
    }

    [[nodiscard]] bool EndFrame() override
    {
        return true;
    }

    [[nodiscard]] bool SetLogicalPresentation(const cg::render::LogicalPresentation&) override
    {
        return true;
    }

    void ResetStats() noexcept override {}

    [[nodiscard]] cg::render::RenderStats GetStats() const noexcept override
    {
        return {};
    }

    [[nodiscard]] cg::render::TextureHandle CreateTexture(const cg::render::TextureDesc& desc) override
    {
        const cg::render::TextureHandle texture{nextTextureId_++, 1};
        liveTextures_.emplace(texture.id, desc);
        created.push_back(desc);
        return texture;
    }

    [[nodiscard]] bool UpdateTexture(cg::render::TextureHandle texture, const void* pixels, int pitch) override
    {
        const auto iter = liveTextures_.find(texture.id);
        if (iter == liveTextures_.end() || pixels == nullptr || pitch <= 0) {
            return false;
        }

        TextureUpload upload;
        upload.texture = texture;
        upload.desc = iter->second;
        upload.pitch = pitch;
        upload.pixels.resize(static_cast<std::size_t>(pitch) * static_cast<std::size_t>(upload.desc.height));
        std::memcpy(upload.pixels.data(), pixels, upload.pixels.size());
        uploads.push_back(std::move(upload));
        return true;
    }

    void DestroyTexture(cg::render::TextureHandle texture) noexcept override
    {
        destroyed.push_back(texture);
        liveTextures_.erase(texture.id);
    }

    [[nodiscard]] bool DrawTexture(cg::render::TextureHandle, const cg::render::RectF&, const cg::render::RectF&) override
    {
        return true;
    }

    [[nodiscard]] bool DrawSprites(const cg::render::SpriteCmd*, std::size_t) override
    {
        ++drawSpritesCalls;
        return true;
    }

    std::size_t drawSpritesCalls = 0;
    std::vector<cg::render::TextureDesc> created;
    std::vector<TextureUpload> uploads;
    std::vector<cg::render::TextureHandle> destroyed;

private:
    std::uint32_t nextTextureId_ = 1;
    std::unordered_map<std::uint32_t, cg::render::TextureDesc> liveTextures_;
};

class FakeGroundTileImageProvider final : public cg::world::IGroundTileImageProvider {
public:
    [[nodiscard]] std::optional<cg::resource::DecodedImage> DecodeTile(std::uint32_t imageNo) override
    {
        requests.push_back(imageNo);
        const auto iter = images.find(imageNo);
        if (iter == images.end()) {
            return std::nullopt;
        }

        return iter->second;
    }

    std::unordered_map<std::uint32_t, cg::resource::DecodedImage> images;
    std::vector<std::uint32_t> requests;
};

class FakeObjectImageProvider final : public cg::world::IMapObjectImageProvider {
public:
    [[nodiscard]] std::optional<cg::resource::DecodedImage> DecodeObject(std::uint32_t imageNo) override
    {
        requests.push_back(imageNo);
        const auto iter = images.find(imageNo);
        if (iter == images.end()) {
            return std::nullopt;
        }

        return iter->second;
    }

    std::unordered_map<std::uint32_t, cg::resource::DecodedImage> images;
    std::vector<std::uint32_t> requests;
};

class FakeEntityGraphicResolver final : public cg::world::IEntitySpriteGraphicResolver {
public:
    [[nodiscard]] std::optional<cg::world::EntitySpriteGraphic> ResolveEntityGraphic(std::int32_t graphicSeqNo) override
    {
        requests.push_back(graphicSeqNo);
        const auto iter = graphics.find(graphicSeqNo);
        if (iter == graphics.end()) {
            return std::nullopt;
        }

        return iter->second;
    }

    std::unordered_map<std::int32_t, cg::world::EntitySpriteGraphic> graphics;
    std::vector<std::int32_t> requests;
};

struct WorldRendererFixture {
    FakeRenderer renderer;
    cg::render::TextureCache textureCache{renderer};
    FakeGroundTileImageProvider groundProvider;
    FakeObjectImageProvider objectProvider;
    cg::world::SceneGroundAtlasCache groundCache{textureCache, groundProvider};
    cg::world::MapObjectTextureCache objectCache{textureCache, objectProvider};
    cg::world::WorldRenderer worldRenderer{groundCache, objectCache};
};

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
}

template <typename T>
int ExpectEquals(const T& actual, const T& expected, const char* message)
{
    return Expect(actual == expected, message);
}

[[nodiscard]] bool RectEquals(const cg::render::RectF& actual, const cg::render::RectF& expected)
{
    return actual.x == expected.x
        && actual.y == expected.y
        && actual.width == expected.width
        && actual.height == expected.height;
}

[[nodiscard]] std::array<std::uint8_t, 4> MakePixel(std::uint8_t seed, std::size_t index)
{
    const auto value = static_cast<std::uint8_t>(seed + static_cast<std::uint8_t>(index));
    return {value, static_cast<std::uint8_t>(value + 1U), static_cast<std::uint8_t>(value + 2U), 255U};
}

[[nodiscard]] cg::resource::DecodedImage MakeImage(
    int width,
    int height,
    std::uint8_t seed,
    int offsetX = 0,
    int offsetY = 0)
{
    cg::resource::DecodedImage image;
    image.width = width;
    image.height = height;
    image.offsetX = offsetX;
    image.offsetY = offsetY;
    image.format = cg::render::PixelFormat::Rgba8;
    image.pixels.reserve(static_cast<std::size_t>(width) * static_cast<std::size_t>(height) * 4U);
    for (std::size_t index = 0; index < static_cast<std::size_t>(width) * static_cast<std::size_t>(height); ++index) {
        const std::array<std::uint8_t, 4> pixel = MakePixel(seed, index);
        image.pixels.insert(image.pixels.end(), pixel.begin(), pixel.end());
    }
    return image;
}

[[nodiscard]] cg::resource::MapData MakeMap(
    std::uint16_t width,
    std::uint16_t height,
    std::vector<std::uint16_t> tileData)
{
    cg::resource::MapData map;
    map.mapId = 10;
    map.serverNo = 2;
    map.floor = 3;
    map.width = width;
    map.height = height;
    map.tileData = std::move(tileData);
    return map;
}

[[nodiscard]] cg::resource::AnimeFrame MakeEntityFrame(std::int32_t graphicSeqNo, std::int16_t x, std::int16_t y)
{
    cg::resource::AnimeFrame frame;
    frame.graphicSeqNo = graphicSeqNo;
    frame.x = x;
    frame.y = y;
    return frame;
}

[[nodiscard]] cg::resource::AnimeAction MakeEntityAction(
    std::int16_t action,
    cg::world::Direction direction,
    std::int32_t duration,
    int frameIndex,
    std::int32_t frameCount)
{
    cg::resource::AnimeAction result;
    result.header.action = static_cast<cg::resource::CG_ActionType>(action);
    result.header.dir = static_cast<cg::resource::CG_Direction>(static_cast<int>(direction));
    result.header.duration = duration;
    result.header.frameCount = frameCount;
    result.frameIndex = frameIndex;
    return result;
}

[[nodiscard]] cg::resource::AnimationClip MakeEntityWalkClip()
{
    cg::resource::AnimationClip clip;
    clip.actions.push_back(MakeEntityAction(
        static_cast<std::int16_t>(cg::resource::CG_ACTIONTYPE_WALK),
        cg::world::Direction::East,
        200,
        0,
        2));
    clip.frames.push_back(MakeEntityFrame(101, 0, 0));
    clip.frames.push_back(MakeEntityFrame(102, 1, 2));
    return clip;
}

[[nodiscard]] cg::world::MapTileAtlasCacheKey MakeKey(std::uint64_t mapGeneration)
{
    return cg::world::MapTileAtlasCacheKey{
        10,
        2,
        3,
        cg::world::MapTileAtlasAllLayerMask,
        mapGeneration,
        1,
    };
}

[[nodiscard]] cg::world::WorldRenderCamera MakeCamera()
{
    return cg::world::WorldRenderCamera{
        cg::world::TileCoord{0, 0},
        cg::world::ScreenPixelCoord{0, 0},
        cg::render::RectF{30.0F, -30.0F, 10.0F, 10.0F},
    };
}

[[nodiscard]] bool SpriteLess(const cg::render::SpriteCmd& lhs, const cg::render::SpriteCmd& rhs) noexcept
{
    if (lhs.layer != rhs.layer) {
        return lhs.layer < rhs.layer;
    }
    if (lhs.sortY != rhs.sortY) {
        return lhs.sortY < rhs.sortY;
    }
    return lhs.order < rhs.order;
}

int TestBuildFrameOnlyBuildsFrame()
{
    static_assert(std::is_constructible_v<
        cg::world::WorldRenderer,
        cg::world::SceneGroundAtlasCache&,
        cg::world::MapObjectTextureCache&>);
    static_assert(!std::is_constructible_v<cg::world::WorldRenderer, cg::render::IRenderer&>);

    int failures = 0;
    WorldRendererFixture fixture;
    cg::world::WorldRenderFrame frame;
    frame.sprites.push_back(cg::render::SpriteCmd{});

    fixture.worldRenderer.BuildFrame(cg::world::WorldRenderInput{}, frame);

    failures += Expect(frame.sprites.empty(), "BuildFrame 应只输出本帧 sprites 并清空旧内容");
    failures += ExpectEquals(fixture.renderer.drawSpritesCalls, static_cast<std::size_t>(0), "BuildFrame 不应调用 DrawSprites");
    return failures;
}

int TestGroundAtlasCameraCullingKeepsOriginalOrder()
{
    int failures = 0;
    WorldRendererFixture fixture;
    fixture.groundProvider.images.emplace(1000U, MakeImage(8, 8, 10U));
    fixture.groundProvider.images.emplace(1001U, MakeImage(8, 8, 20U));

    const cg::resource::MapData map = MakeMap(2, 1, {1000U, 1001U});
    cg::world::WorldRenderFrame frame;
    cg::world::WorldRenderInput input;
    input.map = &map;
    input.groundKey = MakeKey(1U);
    input.camera = MakeCamera();
    input.orderBase.ground = 10U;

    fixture.worldRenderer.BuildFrame(input, frame);

    failures += ExpectEquals(frame.sprites.size(), static_cast<std::size_t>(1), "ground camera culling 应只保留屏内 entry");
    if (frame.sprites.empty()) {
        return failures;
    }

    const cg::render::SpriteCmd& sprite = frame.sprites[0];
    failures += Expect(
        RectEquals(sprite.destination, cg::render::RectF{32.0F, -24.0F, 8.0F, 8.0F}),
        "ground destination 来自未剔除原 entry");
    failures += ExpectEquals(sprite.layer, static_cast<std::uint32_t>(cg::world::TileRenderLayer::Normal), "ground layer");
    failures += ExpectEquals(
        sprite.order,
        static_cast<std::uint32_t>(UINT_MAX) - 11U,
        "前置屏外 ground entry 不应压缩后续 order");
    return failures;
}

int TestMapObjectsUseTextureCacheLayerMapAndOriginalOrder()
{
    int failures = 0;
    WorldRendererFixture fixture;
    fixture.objectProvider.images.emplace(1U, MakeImage(8, 8, 30U));
    fixture.objectProvider.images.emplace(2U, MakeImage(8, 8, 40U));

    const std::array<cg::world::WorldMapObjectSprite, 3> mapObjects{
        cg::world::WorldMapObjectSprite{
            cg::world::MapObject{100, cg::world::TileCoord{1, 0}, 404U},
            1,
            3,
        },
        cg::world::WorldMapObjectSprite{
            cg::world::MapObject{101, cg::world::TileCoord{0, 0}, 1U},
            1,
            3,
        },
        cg::world::WorldMapObjectSprite{
            cg::world::MapObject{102, cg::world::TileCoord{1, 0}, 2U},
            1000,
            3,
        },
    };

    cg::world::WorldRenderFrame frame;
    cg::world::WorldRenderInput input;
    input.mapObjects = mapObjects;
    input.camera = MakeCamera();
    input.nowMs = 200U;
    input.layers.highMapNoFlag2Three = 77U;
    input.orderBase.mapObject = 20U;

    fixture.worldRenderer.BuildFrame(input, frame);

    failures += ExpectEquals(frame.sprites.size(), static_cast<std::size_t>(1), "object 失败和屏外项应跳过");
    failures += ExpectEquals(fixture.objectProvider.requests.size(), static_cast<std::size_t>(3), "object texture cache 应按输入请求纹理");
    failures += ExpectEquals(mapObjects[2].object.objectIndex, 102, "objectIndex 只保留在 MapObject 输入");
    if (frame.sprites.empty()) {
        return failures;
    }

    const cg::render::SpriteCmd& sprite = frame.sprites[0];
    failures += Expect(sprite.texture.IsValid(), "成功 object 应使用 cache 返回 texture");
    failures += ExpectEquals(sprite.layer, 77U, "object layer 应来自显式 layer map");
    failures += ExpectEquals(
        sprite.order,
        static_cast<std::uint32_t>(UINT_MAX) - 22U,
        "失败和屏外 object 不应压缩后续 order");
    failures += Expect(RectEquals(sprite.destination, cg::render::RectF{32.0F, -24.0F, 8.0F, 8.0F}), "object destination");
    return failures;
}

int TestEntitiesBuildSpriteAndSkipInvalidInputs()
{
    int failures = 0;
    WorldRendererFixture fixture;
    const std::array<cg::world::WorldEntitySprite, 4> entities{
        cg::world::WorldEntitySprite{
            cg::world::EntityId{1U, 1U},
            cg::render::TextureHandle{},
            cg::render::RectF{1.0F, 0.0F, 8.0F, 8.0F},
            32.0F,
            -24.0F,
            0.0F,
            0.0F,
            8.0F,
            8.0F,
            9U,
            1,
        },
        cg::world::WorldEntitySprite{
            cg::world::EntityId{2U, 1U},
            cg::render::TextureHandle{80U, 1U},
            cg::render::RectF{2.0F, 0.0F, 8.0F, 8.0F},
            32.0F,
            -24.0F,
            0.0F,
            0.0F,
            0.0F,
            8.0F,
            9U,
            2,
        },
        cg::world::WorldEntitySprite{
            cg::world::EntityId{3U, 1U},
            cg::render::TextureHandle{81U, 1U},
            cg::render::RectF{3.0F, 0.0F, 8.0F, 8.0F},
            0.0F,
            0.0F,
            0.0F,
            0.0F,
            8.0F,
            8.0F,
            9U,
            3,
        },
        cg::world::WorldEntitySprite{
            cg::world::EntityId{4U, 1U},
            cg::render::TextureHandle{82U, 1U},
            cg::render::RectF{4.0F, 5.0F, 8.0F, 8.0F},
            32.0F,
            -24.0F,
            1.0F,
            2.0F,
            8.0F,
            8.0F,
            9U,
            44,
        },
    };

    cg::world::WorldRenderFrame frame;
    cg::world::WorldRenderInput input;
    input.entities = entities;
    input.camera = MakeCamera();
    input.orderBase.entity = 30U;

    fixture.worldRenderer.BuildFrame(input, frame);

    failures += ExpectEquals(frame.sprites.size(), static_cast<std::size_t>(1), "entity 应跳过无效 texture/尺寸/屏外项");
    if (frame.sprites.empty()) {
        return failures;
    }

    const cg::render::SpriteCmd& sprite = frame.sprites[0];
    failures += ExpectEquals(sprite.texture, cg::render::TextureHandle{82U, 1U}, "entity texture");
    failures += Expect(RectEquals(sprite.source, cg::render::RectF{4.0F, 5.0F, 8.0F, 8.0F}), "entity source");
    failures += Expect(RectEquals(sprite.destination, cg::render::RectF{33.0F, -22.0F, 8.0F, 8.0F}), "entity destination");
    failures += ExpectEquals(sprite.layer, 9U, "entity layer");
    failures += ExpectEquals(sprite.sortY, 44, "entity sortY");
    failures += ExpectEquals(
        sprite.order,
        static_cast<std::uint32_t>(UINT_MAX) - 33U,
        "无效 entity 不应压缩后续 order");
    return failures;
}

int TestMixedFrameHasStableSortableKeys()
{
    int failures = 0;
    WorldRendererFixture fixture;
    fixture.groundProvider.images.emplace(1000U, MakeImage(8, 8, 10U));
    fixture.groundProvider.images.emplace(1001U, MakeImage(8, 8, 20U));
    fixture.objectProvider.images.emplace(2U, MakeImage(8, 8, 40U));

    const cg::resource::MapData map = MakeMap(2, 1, {1000U, 1001U});
    const std::array<cg::world::WorldMapObjectSprite, 1> mapObjects{
        cg::world::WorldMapObjectSprite{
            cg::world::MapObject{200, cg::world::TileCoord{1, 0}, 2U},
            1000,
            3,
        },
    };
    const std::array<cg::world::WorldEntitySprite, 1> entities{
        cg::world::WorldEntitySprite{
            cg::world::EntityId{9U, 1U},
            cg::render::TextureHandle{90U, 1U},
            cg::render::RectF{90.0F, 0.0F, 8.0F, 8.0F},
            32.0F,
            -24.0F,
            0.0F,
            0.0F,
            8.0F,
            8.0F,
            9U,
            32,
        },
    };

    cg::world::WorldRenderFrame frame;
    cg::world::WorldRenderInput input;
    input.map = &map;
    input.groundKey = MakeKey(2U);
    input.mapObjects = mapObjects;
    input.entities = entities;
    input.camera = MakeCamera();
    input.nowMs = 300U;
    input.layers.highMapNoFlag2Three = 7U;
    input.orderBase.ground = 10U;
    input.orderBase.mapObject = 20U;
    input.orderBase.entity = 30U;

    fixture.worldRenderer.BuildFrame(input, frame);

    failures += ExpectEquals(frame.sprites.size(), static_cast<std::size_t>(3), "混合 frame 应输出 ground/object/entity");
    if (frame.sprites.size() != 3U) {
        return failures;
    }

    failures += ExpectEquals(frame.sprites[0].layer, 5U, "raw 输出先追加 ground");
    failures += ExpectEquals(frame.sprites[1].layer, 7U, "raw 输出再追加 object");
    failures += ExpectEquals(frame.sprites[2].layer, 9U, "raw 输出最后追加 entity");

    std::vector<cg::render::SpriteCmd> sorted = frame.sprites;
    std::stable_sort(sorted.begin(), sorted.end(), SpriteLess);
    failures += ExpectEquals(sorted[0].layer, 5U, "排序后 ground layer 最早");
    failures += ExpectEquals(sorted[1].layer, 7U, "排序后 object layer 居中");
    failures += ExpectEquals(sorted[2].layer, 9U, "排序后 entity layer 最晚");
    failures += ExpectEquals(sorted[0].order, static_cast<std::uint32_t>(UINT_MAX) - 11U, "ground order");
    failures += ExpectEquals(sorted[1].order, static_cast<std::uint32_t>(UINT_MAX) - 20U, "object order");
    failures += ExpectEquals(sorted[2].order, static_cast<std::uint32_t>(UINT_MAX) - 30U, "entity order");
    return failures;
}

int TestMovingEntityAnimationFeedsWorldRenderer()
{
    int failures = 0;
    WorldRendererFixture fixture;
    cg::world::EntityStore store;
    const cg::world::EntityId id = store.Create();
    const cg::world::Movement movement = cg::world::MakePixelMovement(
        {0, 0},
        {128, 0},
        1000,
        400,
        cg::world::Direction::East);
    cg::world::AnimationState animation{};
    const cg::resource::AnimationClip clip = MakeEntityWalkClip();
    FakeEntityGraphicResolver resolver;
    resolver.graphics.emplace(
        102,
        cg::world::EntitySpriteGraphic{
            cg::render::TextureHandle{91U, 1U},
            cg::render::RectF{11.0F, 12.0F, 8.0F, 8.0F},
            8,
            8,
            0,
            0,
        });

    cg::world::EntitySpriteBuildInput buildInput;
    buildInput.id = id;
    buildInput.movement = &movement;
    buildInput.animation = &animation;
    buildInput.clip = &clip;
    buildInput.graphicResolver = &resolver;
    buildInput.movingAction = static_cast<std::int16_t>(cg::resource::CG_ACTIONTYPE_WALK);
    buildInput.originTile = cg::world::TileCoord{0, 0};
    buildInput.originScreen = cg::world::ScreenPixelCoord{0, 0};
    buildInput.nowTimeMs = 1200;
    buildInput.deltaMs = 100;
    buildInput.layer = 9U;
    buildInput.sortY = 44;

    const std::optional<cg::world::WorldEntitySprite> built = cg::world::BuildEntitySprite(buildInput);
    failures += Expect(built.has_value(), "移动+动画应生成 WorldEntitySprite");
    failures += Expect(animation.action == cg::resource::CG_ACTIONTYPE_WALK, "移动中 action 为 walk");
    failures += Expect(animation.direction == cg::world::Direction::East, "动画方向跟随移动方向");
    failures += Expect(animation.frameIndex == 1U, "移动 delta 推进到第二帧");
    failures += Expect(!resolver.requests.empty() && resolver.requests[0] == 102, "renderer 前已解析推进后的图档");
    if (!built.has_value()) {
        return failures;
    }

    const std::array<cg::world::WorldEntitySprite, 1> entities{*built};
    cg::world::WorldRenderFrame frame;
    cg::world::WorldRenderInput input;
    input.entities = entities;
    input.camera = MakeCamera();
    input.orderBase.entity = 30U;

    fixture.worldRenderer.BuildFrame(input, frame);

    failures += ExpectEquals(frame.sprites.size(), static_cast<std::size_t>(1), "WorldRenderer 应接收 helper 生成的 entity sprite");
    if (frame.sprites.empty()) {
        return failures;
    }

    const cg::render::SpriteCmd& sprite = frame.sprites[0];
    failures += ExpectEquals(sprite.texture, cg::render::TextureHandle{91U, 1U}, "闭环 texture");
    failures += Expect(RectEquals(sprite.source, cg::render::RectF{11.0F, 12.0F, 8.0F, 8.0F}), "闭环 source");
    failures += Expect(RectEquals(sprite.destination, cg::render::RectF{33.0F, -22.0F, 8.0F, 8.0F}), "闭环 destination");
    failures += ExpectEquals(sprite.layer, 9U, "闭环 layer");
    failures += ExpectEquals(sprite.sortY, 44, "闭环 sortY");
    failures += ExpectEquals(sprite.order, static_cast<std::uint32_t>(UINT_MAX) - 30U, "闭环 order");
    return failures;
}

int TestObjectIndexScope()
{
    int failures = 0;
    failures += Expect(HasObjectIndex<cg::world::MapObject>, "MapObject 保留 objectIndex");
    failures += Expect(!HasObjectIndex<cg::world::WorldEntitySprite>, "WorldEntitySprite 不新增 objectIndex");
    failures += Expect(!HasObjectIndex<cg::world::EntityData>, "EntityData 不新增 objectIndex");
    failures += Expect(!HasObjectIndex<cg::render::SpriteCmd>, "SpriteCmd 不新增 objectIndex");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestBuildFrameOnlyBuildsFrame();
    failures += TestGroundAtlasCameraCullingKeepsOriginalOrder();
    failures += TestMapObjectsUseTextureCacheLayerMapAndOriginalOrder();
    failures += TestEntitiesBuildSpriteAndSkipInvalidInputs();
    failures += TestMixedFrameHasStableSortableKeys();
    failures += TestMovingEntityAnimationFeedsWorldRenderer();
    failures += TestObjectIndexScope();

    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
