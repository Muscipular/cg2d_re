#include "world/SyntheticPopulation.hpp"

#include "render/IRenderer.hpp"
#include "render/RenderQueue.hpp"
#include "render/SpriteCmd.hpp"
#include "world/EntityData.hpp"
#include "world/WorldRenderer.hpp"

#include <array>
#include <cmath>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <type_traits>
#include <vector>

namespace {

template <typename T>
concept HasObjectIndex = requires(T value) {
    value.objectIndex;
};

class RecordingRenderer final : public cg::render::IRenderer {
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

    [[nodiscard]] cg::render::TextureHandle CreateTexture(const cg::render::TextureDesc&) override
    {
        return cg::render::TextureHandle{1U, 1U};
    }

    [[nodiscard]] bool UpdateTexture(cg::render::TextureHandle, const void*, int) override
    {
        return true;
    }

    void DestroyTexture(cg::render::TextureHandle) noexcept override {}

    [[nodiscard]] bool DrawTexture(cg::render::TextureHandle, const cg::render::RectF&, const cg::render::RectF&) override
    {
        return true;
    }

    [[nodiscard]] bool DrawSprites(const cg::render::SpriteCmd* sprites, std::size_t count) override
    {
        for (std::size_t index = 0; index < count; ++index) {
            drawnSprites.push_back(sprites[index]);
        }
        return true;
    }

    std::vector<cg::render::SpriteCmd> drawnSprites;
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

[[nodiscard]] bool RectEquals(const cg::render::RectF& lhs, const cg::render::RectF& rhs) noexcept
{
    return lhs.x == rhs.x
        && lhs.y == rhs.y
        && lhs.width == rhs.width
        && lhs.height == rhs.height;
}

[[nodiscard]] bool SpriteEquals(
    const cg::world::WorldEntitySprite& lhs,
    const cg::world::WorldEntitySprite& rhs) noexcept
{
    return lhs.id == rhs.id
        && lhs.texture == rhs.texture
        && RectEquals(lhs.source, rhs.source)
        && lhs.anchorScreenX == rhs.anchorScreenX
        && lhs.anchorScreenY == rhs.anchorScreenY
        && lhs.drawOffsetX == rhs.drawOffsetX
        && lhs.drawOffsetY == rhs.drawOffsetY
        && lhs.width == rhs.width
        && lhs.height == rhs.height
        && lhs.layer == rhs.layer
        && lhs.sortY == rhs.sortY;
}

[[nodiscard]] bool PopulationSpritesEqual(
    const cg::world::SyntheticPopulation& lhs,
    const cg::world::SyntheticPopulation& rhs) noexcept
{
    if (lhs.sprites.size() != rhs.sprites.size()) {
        return false;
    }

    for (std::size_t index = 0; index < lhs.sprites.size(); ++index) {
        if (!SpriteEquals(lhs.sprites[index], rhs.sprites[index])) {
            return false;
        }
    }

    return true;
}

[[nodiscard]] bool HasAnyDifferentKeyField(
    const cg::world::SyntheticPopulation& lhs,
    const cg::world::SyntheticPopulation& rhs) noexcept
{
    const std::size_t count = lhs.sprites.size() < rhs.sprites.size() ? lhs.sprites.size() : rhs.sprites.size();
    for (std::size_t index = 0; index < count; ++index) {
        const cg::world::WorldEntitySprite& left = lhs.sprites[index];
        const cg::world::WorldEntitySprite& right = rhs.sprites[index];
        if (!(left.texture == right.texture)
            || !RectEquals(left.source, right.source)
            || left.anchorScreenX != right.anchorScreenX
            || left.anchorScreenY != right.anchorScreenY
            || left.layer != right.layer
            || left.sortY != right.sortY) {
            return true;
        }
    }

    return lhs.sprites.size() != rhs.sprites.size();
}

[[nodiscard]] cg::world::SyntheticPopulationConfig MakeBaseConfig(
    std::size_t count,
    std::span<const cg::render::TextureHandle> textures,
    std::span<const cg::render::RectF> sources) noexcept
{
    cg::world::SyntheticPopulationConfig config;
    config.seed = 123U;
    config.count = count;
    config.placement = cg::world::SyntheticPopulationPlacement::Screen;
    config.screenRange = cg::render::RectF{-500.0F, -300.0F, 1000.0F, 600.0F};
    config.textures = textures;
    config.sources = sources;
    config.width = 32.0F;
    config.height = 48.0F;
    config.layerRange = cg::world::SyntheticPopulationLayerRange{2U, 5U};
    config.sortYRange = cg::world::SyntheticPopulationSortYRange{-100, 100};
    return config;
}

[[nodiscard]] bool PopulationIsEmpty(const cg::world::SyntheticPopulation& population) noexcept
{
    return population.sprites.empty() && population.store.ActiveCount() == 0U;
}

[[nodiscard]] bool SpriteCommandsSortedByRenderOrder(const std::vector<cg::render::SpriteCmd>& sprites) noexcept
{
    for (std::size_t index = 1U; index < sprites.size(); ++index) {
        const cg::render::SpriteCmd& previous = sprites[index - 1U];
        const cg::render::SpriteCmd& current = sprites[index];
        if (previous.layer > current.layer) {
            return false;
        }
        if (previous.layer == current.layer && previous.sortY > current.sortY) {
            return false;
        }
        if (previous.layer == current.layer && previous.sortY == current.sortY && previous.order > current.order) {
            return false;
        }
    }

    return true;
}

int TestRequestedCounts()
{
    int failures = 0;
    constexpr std::array<std::size_t, 4U> Counts{1000U, 4000U, 8000U, 16000U};
    constexpr std::array<cg::render::TextureHandle, 2U> Textures{
        cg::render::TextureHandle{10U, 1U},
        cg::render::TextureHandle{11U, 1U},
    };
    constexpr std::array<cg::render::RectF, 2U> Sources{
        cg::render::RectF{0.0F, 0.0F, 32.0F, 48.0F},
        cg::render::RectF{32.0F, 0.0F, 32.0F, 48.0F},
    };

    for (const std::size_t count : Counts) {
        const cg::world::SyntheticPopulation population = cg::world::GenerateSyntheticPopulation(
            MakeBaseConfig(count, Textures, Sources));
        failures += ExpectEquals(population.sprites.size(), count, "合成 sprite 数量应匹配 count");
        failures += ExpectEquals(population.store.ActiveCount(), count, "EntityStore active 数量应匹配 count");
    }

    return failures;
}

int TestSeedDeterminism()
{
    int failures = 0;
    constexpr std::array<cg::render::TextureHandle, 3U> Textures{
        cg::render::TextureHandle{20U, 1U},
        cg::render::TextureHandle{21U, 1U},
        cg::render::TextureHandle{22U, 1U},
    };
    constexpr std::array<cg::render::RectF, 3U> Sources{
        cg::render::RectF{0.0F, 0.0F, 16.0F, 16.0F},
        cg::render::RectF{16.0F, 0.0F, 16.0F, 16.0F},
        cg::render::RectF{32.0F, 0.0F, 16.0F, 16.0F},
    };

    cg::world::SyntheticPopulationConfig config = MakeBaseConfig(128U, Textures, Sources);
    const cg::world::SyntheticPopulation first = cg::world::GenerateSyntheticPopulation(config);
    const cg::world::SyntheticPopulation second = cg::world::GenerateSyntheticPopulation(config);
    config.seed = 124U;
    const cg::world::SyntheticPopulation third = cg::world::GenerateSyntheticPopulation(config);
    config.seed = 0U;
    const cg::world::SyntheticPopulation zeroSeed = cg::world::GenerateSyntheticPopulation(config);
    config.seed = 0x6D2B79F5U;
    const cg::world::SyntheticPopulation fallbackSeed = cg::world::GenerateSyntheticPopulation(config);

    failures += Expect(PopulationSpritesEqual(first, second), "同 seed/config 结果应一致");
    failures += Expect(HasAnyDifferentKeyField(first, third), "不同 seed 至少一个关键字段不同");
    failures += Expect(HasAnyDifferentKeyField(zeroSeed, fallbackSeed), "seed 0 与 0x6D2B79F5U 结果应不同");
    return failures;
}

int TestInvalidConfigsReturnEmpty()
{
    int failures = 0;
    constexpr std::array<cg::render::TextureHandle, 1U> Textures{cg::render::TextureHandle{30U, 1U}};
    constexpr std::array<cg::render::RectF, 1U> Sources{cg::render::RectF{0.0F, 0.0F, 8.0F, 8.0F}};

    cg::world::SyntheticPopulationConfig config = MakeBaseConfig(1U, Textures, Sources);
    config.count = 0U;
    failures += Expect(PopulationIsEmpty(cg::world::GenerateSyntheticPopulation(config)), "count 为 0 返回空");

    config = MakeBaseConfig(1U, std::span<const cg::render::TextureHandle>{}, Sources);
    failures += Expect(PopulationIsEmpty(cg::world::GenerateSyntheticPopulation(config)), "空 textures 返回空");

    config = MakeBaseConfig(1U, Textures, std::span<const cg::render::RectF>{});
    failures += Expect(PopulationIsEmpty(cg::world::GenerateSyntheticPopulation(config)), "空 sources 返回空");

    config = MakeBaseConfig(1U, Textures, Sources);
    config.width = 0.0F;
    failures += Expect(PopulationIsEmpty(cg::world::GenerateSyntheticPopulation(config)), "无效 width 返回空");

    config = MakeBaseConfig(1U, Textures, Sources);
    config.height = -1.0F;
    failures += Expect(PopulationIsEmpty(cg::world::GenerateSyntheticPopulation(config)), "无效 height 返回空");

    config = MakeBaseConfig(1U, Textures, Sources);
    config.placement = cg::world::SyntheticPopulationPlacement::Tile;
    config.minTile = cg::world::TileCoord{2, 0};
    config.maxTile = cg::world::TileCoord{1, 0};
    failures += Expect(PopulationIsEmpty(cg::world::GenerateSyntheticPopulation(config)), "非法 tile range 返回空");
    return failures;
}

int TestFixedScreenRange()
{
    int failures = 0;
    constexpr std::array<cg::render::TextureHandle, 1U> Textures{cg::render::TextureHandle{40U, 1U}};
    constexpr std::array<cg::render::RectF, 1U> Sources{cg::render::RectF{0.0F, 0.0F, 8.0F, 8.0F}};

    cg::world::SyntheticPopulationConfig config = MakeBaseConfig(4U, Textures, Sources);
    config.screenRange = cg::render::RectF{123.0F, 456.0F, 0.0F, 0.0F};
    config.layerRange = cg::world::SyntheticPopulationLayerRange{7U, 7U};
    config.sortYRange = cg::world::SyntheticPopulationSortYRange{-3, -3};

    const cg::world::SyntheticPopulation population = cg::world::GenerateSyntheticPopulation(config);
    failures += ExpectEquals(population.sprites.size(), static_cast<std::size_t>(4U), "固定 screen range 数量");
    for (const cg::world::WorldEntitySprite& sprite : population.sprites) {
        failures += Expect(sprite.anchorScreenX == 123.0F, "固定 screen anchor x");
        failures += Expect(sprite.anchorScreenY == 456.0F, "固定 screen anchor y");
        failures += ExpectEquals(sprite.layer, 7U, "固定 layer");
        failures += ExpectEquals(sprite.sortY, -3, "固定 sortY");
    }

    return failures;
}

int TestFixedTileRange()
{
    int failures = 0;
    constexpr std::array<cg::render::TextureHandle, 1U> Textures{cg::render::TextureHandle{50U, 1U}};
    constexpr std::array<cg::render::RectF, 1U> Sources{cg::render::RectF{0.0F, 0.0F, 8.0F, 8.0F}};

    cg::world::SyntheticPopulationConfig config = MakeBaseConfig(1U, Textures, Sources);
    config.placement = cg::world::SyntheticPopulationPlacement::Tile;
    config.minTile = cg::world::TileCoord{11, 19};
    config.maxTile = cg::world::TileCoord{11, 19};
    config.originTile = cg::world::TileCoord{10, 20};
    config.originScreen = cg::world::ScreenPixelCoord{100, 200};

    const cg::world::SyntheticPopulation population = cg::world::GenerateSyntheticPopulation(config);
    const cg::world::ScreenPixelCoord expected = cg::world::ProjectTileDeltaToScreen(
        cg::world::TileCoord{1, -1},
        config.originScreen);

    failures += ExpectEquals(population.sprites.size(), static_cast<std::size_t>(1U), "固定 tile range 数量");
    if (!population.sprites.empty()) {
        failures += Expect(population.sprites[0].anchorScreenX == static_cast<float>(expected.x), "tile anchor x");
        failures += Expect(population.sprites[0].anchorScreenY == static_cast<float>(expected.y), "tile anchor y");
    }

    return failures;
}

int TestExtremeInt32Ranges()
{
    int failures = 0;
    constexpr std::array<cg::render::TextureHandle, 1U> Textures{cg::render::TextureHandle{55U, 1U}};
    constexpr std::array<cg::render::RectF, 1U> Sources{cg::render::RectF{0.0F, 0.0F, 8.0F, 8.0F}};
    constexpr std::int32_t MinInt32 = std::numeric_limits<std::int32_t>::min();
    constexpr std::int32_t MaxInt32 = std::numeric_limits<std::int32_t>::max();

    cg::world::SyntheticPopulationConfig config = MakeBaseConfig(8U, Textures, Sources);
    config.seed = 0x6D2B79F5U;
    config.placement = cg::world::SyntheticPopulationPlacement::Tile;
    config.minTile = cg::world::TileCoord{MinInt32, MinInt32};
    config.maxTile = cg::world::TileCoord{MaxInt32, MaxInt32};
    config.originTile = cg::world::TileCoord{0, 0};
    config.originScreen = cg::world::ScreenPixelCoord{0, 0};
    config.layerRange = cg::world::SyntheticPopulationLayerRange{0U, 0U};
    config.sortYRange = cg::world::SyntheticPopulationSortYRange{MinInt32, MaxInt32};

    const cg::world::SyntheticPopulation population = cg::world::GenerateSyntheticPopulation(config);
    failures += ExpectEquals(population.sprites.size(), static_cast<std::size_t>(8U), "极端 int32 range 数量");
    for (const cg::world::WorldEntitySprite& sprite : population.sprites) {
        failures += Expect(sprite.sortY >= MinInt32 && sprite.sortY <= MaxInt32, "极端 sortY 在范围");
        failures += Expect(std::isfinite(sprite.anchorScreenX), "极端 tile anchor x 有限");
        failures += Expect(std::isfinite(sprite.anchorScreenY), "极端 tile anchor y 有限");
    }

    return failures;
}

int TestWorldRenderInputEntitySpan()
{
    int failures = 0;
    constexpr std::array<cg::render::TextureHandle, 2U> Textures{
        cg::render::TextureHandle{60U, 1U},
        cg::render::TextureHandle{61U, 1U},
    };
    constexpr std::array<cg::render::RectF, 1U> Sources{cg::render::RectF{0.0F, 0.0F, 16.0F, 24.0F}};

    cg::world::SyntheticPopulationConfig config = MakeBaseConfig(256U, Textures, Sources);
    config.screenRange = cg::render::RectF{10.0F, 20.0F, 100.0F, 100.0F};
    const cg::world::SyntheticPopulation population = cg::world::GenerateSyntheticPopulation(config);

    cg::world::WorldRenderInput input;
    input.map = nullptr;
    input.entities = population.sprites;
    input.camera.cameraRect = cg::render::RectF{-1000.0F, -1000.0F, 4000.0F, 4000.0F};

    failures += ExpectEquals(input.entities.size(), population.sprites.size(), "WorldRenderInput entity span 数量保持");
    failures += Expect(input.entities.data() == population.sprites.data(), "WorldRenderInput entity span 指向合成人口 sprite");
    return failures;
}

int TestRenderQueueInput()
{
    int failures = 0;
    constexpr std::array<cg::render::TextureHandle, 2U> Textures{
        cg::render::TextureHandle{70U, 1U},
        cg::render::TextureHandle{71U, 1U},
    };
    constexpr std::array<cg::render::RectF, 1U> Sources{cg::render::RectF{0.0F, 0.0F, 16.0F, 24.0F}};

    cg::world::SyntheticPopulationConfig config = MakeBaseConfig(512U, Textures, Sources);
    const cg::world::SyntheticPopulation population = cg::world::GenerateSyntheticPopulation(config);

    cg::render::RenderQueue queue;
    for (std::size_t index = 0; index < population.sprites.size(); ++index) {
        const cg::world::WorldEntitySprite& entity = population.sprites[index];
        cg::render::SpriteCmd cmd{};
        cmd.texture = entity.texture;
        cmd.source = entity.source;
        cmd.destination = cg::render::RectF{
            entity.anchorScreenX + entity.drawOffsetX,
            entity.anchorScreenY + entity.drawOffsetY,
            entity.width,
            entity.height,
        };
        cmd.layer = entity.layer;
        cmd.sortY = entity.sortY;
        cmd.order = static_cast<std::uint32_t>(index);
        queue.PushSprite(cmd);

        failures += Expect(entity.layer >= config.layerRange.min && entity.layer <= config.layerRange.max, "layer 在合法范围");
        failures += Expect(entity.sortY >= config.sortYRange.min && entity.sortY <= config.sortYRange.max, "sortY 在合法范围");
    }

    failures += ExpectEquals(queue.Size(), population.sprites.size(), "RenderQueue push 数量保持");
    RecordingRenderer renderer;
    failures += Expect(queue.Flush(renderer), "RenderQueue Flush 成功");
    failures += ExpectEquals(renderer.drawnSprites.size(), population.sprites.size(), "RenderQueue Flush 数量保持");
    failures += Expect(SpriteCommandsSortedByRenderOrder(renderer.drawnSprites), "RenderQueue Flush 后排序字段合法");
    return failures;
}

int TestNoObjectIndexOnSyntheticTypes()
{
    int failures = 0;
    failures += Expect(!HasObjectIndex<cg::world::SyntheticPopulationConfig>, "SyntheticPopulationConfig 不新增 object index 字段");
    failures += Expect(!HasObjectIndex<cg::world::SyntheticPopulation>, "SyntheticPopulation 不新增 object index 字段");
    failures += Expect(!HasObjectIndex<cg::world::WorldEntitySprite>, "WorldEntitySprite 不新增 object index 字段");
    failures += Expect(!HasObjectIndex<cg::world::EntityData>, "EntityData 不新增 object index 字段");
    failures += Expect(!HasObjectIndex<cg::render::SpriteCmd>, "SpriteCmd 不新增 object index 字段");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestRequestedCounts();
    failures += TestSeedDeterminism();
    failures += TestInvalidConfigsReturnEmpty();
    failures += TestFixedScreenRange();
    failures += TestFixedTileRange();
    failures += TestExtremeInt32Ranges();
    failures += TestWorldRenderInputEntitySpan();
    failures += TestRenderQueueInput();
    failures += TestNoObjectIndexOnSyntheticTypes();

    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
