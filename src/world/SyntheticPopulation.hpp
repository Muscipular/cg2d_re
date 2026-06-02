#pragma once

#include "render/RenderTypes.hpp"
#include "render/Texture.hpp"
#include "world/EntityStore.hpp"
#include "world/Transform.hpp"
#include "world/WorldRenderer.hpp"

#include <cstddef>
#include <cstdint>
#include <span>
#include <vector>

namespace cg::world {

// SyntheticPopulationPlacement 声明合成实体 anchor 的生成坐标系。
enum class SyntheticPopulationPlacement : std::uint8_t {
    Screen,
    Tile,
};

// SyntheticPopulationLayerRange 表示合成 sprite layer 的闭区间。
struct SyntheticPopulationLayerRange {
    std::uint32_t min = 0;
    std::uint32_t max = 0;
};

// SyntheticPopulationSortYRange 表示合成 sprite sortY 的闭区间。
struct SyntheticPopulationSortYRange {
    std::int32_t min = 0;
    std::int32_t max = 0;
};

// SyntheticPopulationConfig 是 WorldRenderer/RenderQueue 压测用实体输入配置。
struct SyntheticPopulationConfig {
    std::uint32_t seed = 1U;
    std::size_t count = 0U;
    SyntheticPopulationPlacement placement = SyntheticPopulationPlacement::Screen;
    cg::render::RectF screenRange{};
    TileCoord minTile{};
    TileCoord maxTile{};
    TileCoord originTile{};
    ScreenPixelCoord originScreen{};
    std::span<const cg::render::TextureHandle> textures{};
    std::span<const cg::render::RectF> sources{};
    float width = 0.0F;
    float height = 0.0F;
    SyntheticPopulationLayerRange layerRange{};
    SyntheticPopulationSortYRange sortYRange{};
};

// SyntheticPopulation 保存合成实体 store 和可直接提交给 WorldRenderer 的 sprite 输入。
struct SyntheticPopulation {
    EntityStore store;
    std::vector<WorldEntitySprite> sprites;
};

[[nodiscard]] SyntheticPopulation GenerateSyntheticPopulation(const SyntheticPopulationConfig& config);

} // namespace cg::world
