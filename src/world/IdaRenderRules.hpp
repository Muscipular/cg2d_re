#pragma once

#include <cstdint>
#include <optional>

namespace cg::world {

// TileRenderLayer 对应 IDA normal scene 的地板 tile 分层。
enum class TileRenderLayer : std::uint32_t {
    Special = 1U,
    Normal = 5U,
};

// GraphicDrawPosition 是 DrawGraphic 入队前叠加 Graphic::getXY 后的坐标。
struct GraphicDrawPosition {
    std::int32_t x{};
    std::int32_t y{};
};

// MapObjectRenderGroup 只表达 IDA object 入队分组条件，不声明透明/遮挡规则。
enum class MapObjectRenderGroup : std::uint8_t {
    MapNoOne,
    HighMapNoFlag2Three,
    HighMapNoOtherFlag2,
};

// DecodeMapCellForWorld 校对自 IDA 0x46CF00 查询逻辑。
[[nodiscard]] std::uint32_t DecodeMapCellForWorld(std::uint32_t raw) noexcept;

// ClassifyWorldTileLayer 对应 IDA 0x46CF60 normal scene tile layer 选择。
[[nodiscard]] std::optional<TileRenderLayer> ClassifyWorldTileLayer(std::uint32_t decodedImageNo) noexcept;

// ComputeDrawGraphicPosition 对应 Renderer::DrawGraphic(0x521B10) 叠加 Graphic::getXY。
[[nodiscard]] GraphicDrawPosition ComputeDrawGraphicPosition(
    std::int32_t baseX,
    std::int32_t baseY,
    std::int32_t graphicX,
    std::int32_t graphicY) noexcept;

// ClassifyMapObjectRenderGroup 对应 sub_46BE50/sub_46BD60：
// mapNo == 1 进入普通 object 队列；mapNo > 999 时 flag2 == 3 进入末尾追加队列，其他 flag2 进入参与排序合并的队列。
[[nodiscard]] std::optional<MapObjectRenderGroup> ClassifyMapObjectRenderGroup(
    std::int32_t mapNo,
    std::int32_t flag2) noexcept;

// MakeIdaCompatibleOrder 将 Renderer::SortTexture(0x520790)/0x5222A0 同 layer cacheIndex 逆序映射到 RenderQueue order 升序。
[[nodiscard]] std::uint32_t MakeIdaCompatibleOrder(std::uint32_t enqueueIndex) noexcept;

} // namespace cg::world
