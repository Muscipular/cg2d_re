#include "world/IdaRenderRules.hpp"

#include <climits>

namespace cg::world {
namespace {

constexpr std::uint32_t SpecialTileBeginA = 5000U;
constexpr std::uint32_t SpecialTileEndA = 5009U;
constexpr std::uint32_t SpecialTileBeginB = 7664U;
constexpr std::uint32_t SpecialTileEndB = 7679U;

[[nodiscard]] bool IsSpecialWorldTile(std::uint32_t decodedImageNo) noexcept
{
    return (decodedImageNo >= SpecialTileBeginA && decodedImageNo <= SpecialTileEndA)
        || (decodedImageNo >= SpecialTileBeginB && decodedImageNo <= SpecialTileEndB);
}

} // namespace

std::uint32_t DecodeMapCellForWorld(std::uint32_t raw) noexcept
{
    // 来源：IDA 0x46CF00；raw >= 0x10000 不走低位表。
    if (raw >= 0x10000U) {
        return raw;
    }

    return raw < 20000U ? raw : raw + 200000U;
}

std::optional<TileRenderLayer> ClassifyWorldTileLayer(std::uint32_t decodedImageNo) noexcept
{
    if (decodedImageNo <= 999U) {
        return std::nullopt;
    }

    if (IsSpecialWorldTile(decodedImageNo)) {
        return TileRenderLayer::Special;
    }

    return TileRenderLayer::Normal;
}

GraphicDrawPosition ComputeDrawGraphicPosition(
    std::int32_t baseX,
    std::int32_t baseY,
    std::int32_t graphicX,
    std::int32_t graphicY) noexcept
{
    return GraphicDrawPosition{
        baseX + graphicX,
        baseY + graphicY,
    };
}

std::optional<MapObjectRenderGroup> ClassifyMapObjectRenderGroup(std::int32_t mapNo, std::int32_t flag2) noexcept
{
    if (mapNo == 1) {
        return MapObjectRenderGroup::MapNoOne;
    }

    if (mapNo <= 999) {
        return std::nullopt;
    }

    if (flag2 == 3) {
        return MapObjectRenderGroup::HighMapNoFlag2Three;
    }

    return MapObjectRenderGroup::HighMapNoOtherFlag2;
}

std::uint32_t MakeIdaCompatibleOrder(std::uint32_t enqueueIndex) noexcept
{
    return static_cast<std::uint32_t>(UINT_MAX) - enqueueIndex;
}

} // namespace cg::world
