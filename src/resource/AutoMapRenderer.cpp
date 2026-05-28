#include "resource/AutoMapRenderer.hpp"

#include "resource/AutoMapRules.hpp"

#include <algorithm>
#include <limits>
#include <optional>

namespace cg::resource {
namespace {

constexpr int Rgba8Stride = 4;

[[nodiscard]] int ComputeAutoMapImageWidth(const MapData& map) noexcept
{
    return static_cast<int>(2U * (static_cast<unsigned>(map.width) + static_cast<unsigned>(map.height)) - 1U);
}

[[nodiscard]] int ComputeAutoMapImageHeight(const MapData& map) noexcept
{
    return static_cast<int>(static_cast<unsigned>(map.width) + static_cast<unsigned>(map.height) + 1U);
}

[[nodiscard]] bool CanAddressPixels(int width, int height) noexcept
{
    if (width <= 0 || height <= 0) {
        return false;
    }

    const auto pixelCount = static_cast<std::uint64_t>(width) * static_cast<std::uint64_t>(height);
    return pixelCount <= std::numeric_limits<std::size_t>::max() / Rgba8Stride;
}

void PutPixel(AutoMapImage& image, int x, int y, const Rgba8& color) noexcept
{
    if (x < 0 || y < 0 || x >= image.width || y >= image.height) {
        return;
    }

    const std::size_t offset =
        (static_cast<std::size_t>(y) * static_cast<std::size_t>(image.width) + static_cast<std::size_t>(x)) * Rgba8Stride;
    image.pixels[offset] = color.r;
    image.pixels[offset + 1U] = color.g;
    image.pixels[offset + 2U] = color.b;
    image.pixels[offset + 3U] = color.a;
}

void DrawCellDiamond(AutoMapImage& image, int mapWidth, int x, int y, const Rgba8& color) noexcept
{
    const AutoMapScreenPoint point = ProjectMapCellToAutoMap(mapWidth, x, y);
    PutPixel(image, point.x, point.y, color);
    PutPixel(image, point.x - 1, point.y, color);
    PutPixel(image, point.x + 1, point.y, color);
    PutPixel(image, point.x, point.y - 1, color);
}

void DrawColorIndex(
    AutoMapImage& image,
    int mapWidth,
    int x,
    int y,
    std::uint8_t colorIndex,
    const Palette& palette) noexcept
{
    if (colorIndex == 0) {
        return;
    }

    DrawCellDiamond(image, mapWidth, x, y, palette[colorIndex]);
}

[[nodiscard]] std::optional<int> DecodeMapCellToInt(std::uint16_t raw) noexcept
{
    const std::uint32_t mapNo = DecodeMapCell(raw);
    if (mapNo > static_cast<std::uint32_t>(std::numeric_limits<int>::max())) {
        return std::nullopt;
    }

    return static_cast<int>(mapNo);
}

void DrawTileLayer(
    AutoMapImage& image,
    const MapData& map,
    const ResourceManager& resourceManager,
    const Palette& palette)
{
    const std::size_t count = std::min(map.CellCount(), map.tileData.size());
    for (std::size_t index = 0; index < count; ++index) {
        const std::optional<int> mapNo = DecodeMapCellToInt(map.tileData[index]);
        if (!mapNo.has_value()) {
            continue;
        }

        const std::optional<std::uint8_t> colorIndex = resourceManager.FindAutoMapColorIndexByMapNo(*mapNo);
        if (!colorIndex.has_value()) {
            continue;
        }

        const int x = static_cast<int>(index % map.width);
        const int y = static_cast<int>(index / map.width);
        DrawColorIndex(image, map.width, x, y, *colorIndex, palette);
    }
}

void DrawAutoMapDataLayer(AutoMapImage& image, const MapData& map, const AutoMapData& autoMapData) noexcept
{
    if (autoMapData.width != map.width || autoMapData.height != map.height) {
        return;
    }

    const std::size_t count = std::min(map.CellCount(), autoMapData.cells.size());
    for (std::size_t index = 0; index < count; ++index) {
        const int x = static_cast<int>(index % map.width);
        const int y = static_cast<int>(index / map.width);
        DrawCellDiamond(image, map.width, x, y, autoMapData.cells[index]);
    }
}

void DrawObjectCoverRect(
    AutoMapImage& image,
    const MapData& map,
    const AutoMapCellRect& rect,
    std::uint8_t colorIndex,
    const Palette& palette) noexcept
{
    const int minX = std::max(0, rect.minX);
    const int minY = std::max(0, rect.minY);
    const int maxX = std::min(static_cast<int>(map.width) - 1, rect.maxX);
    const int maxY = std::min(static_cast<int>(map.height) - 1, rect.maxY);
    if (minX > maxX || minY > maxY) {
        return;
    }

    for (int y = minY; y <= maxY; ++y) {
        for (int x = minX; x <= maxX; ++x) {
            DrawColorIndex(image, map.width, x, y, colorIndex, palette);
        }
    }
}

void DrawObjectLayer(
    AutoMapImage& image,
    const MapData& map,
    const ResourceManager& resourceManager,
    const Palette& palette)
{
    const std::size_t count = std::min(map.CellCount(), map.objData.size());
    for (std::size_t index = 0; index < count; ++index) {
        const std::optional<int> mapNo = DecodeMapCellToInt(map.objData[index]);
        if (!mapNo.has_value()) {
            continue;
        }

        const std::optional<std::uint8_t> colorIndex = resourceManager.FindAutoMapColorIndexByMapNo(*mapNo);
        if (!colorIndex.has_value() || *colorIndex == 0) {
            continue;
        }

        const std::optional<GraphicRef> graphic = resourceManager.FindGraphicByMapNo(*mapNo);
        if (!graphic.has_value()) {
            continue;
        }

        const int x = static_cast<int>(index % map.width);
        const int y = static_cast<int>(index / map.width);
        const std::optional<AutoMapCellRect> rect = ComputeObjectCoverRect(graphic->info, x, y, *colorIndex);
        if (!rect.has_value()) {
            continue;
        }

        DrawObjectCoverRect(image, map, *rect, *colorIndex, palette);
    }
}

void DrawCollisionLayer(AutoMapImage& image, const MapData& map, const AutoMapRenderOptions& options) noexcept
{
    const std::size_t count = std::min(map.CellCount(), map.hitData.size());
    for (std::size_t index = 0; index < count; ++index) {
        if (!IsAutoMapCollisionVisible(map.hitData[index])) {
            continue;
        }

        const int x = static_cast<int>(index % map.width);
        const int y = static_cast<int>(index / map.width);
        DrawCellDiamond(image, map.width, x, y, options.collisionColor);
    }
}

} // namespace

AutoMapImage RenderAutoMapRgba8(
    const MapData& map,
    const ResourceManager& resourceManager,
    const Palette& palette,
    const AutoMapRenderOptions& options)
{
    return RenderAutoMapRgba8(map, resourceManager, palette, options, nullptr);
}

AutoMapImage RenderAutoMapRgba8(
    const MapData& map,
    const ResourceManager& resourceManager,
    const Palette& palette,
    const AutoMapRenderOptions& options,
    const AutoMapData* autoMapData)
{
    AutoMapImage image;
    if (map.IsEmpty()) {
        return image;
    }

    image.width = ComputeAutoMapImageWidth(map);
    image.height = ComputeAutoMapImageHeight(map);
    if (!CanAddressPixels(image.width, image.height)) {
        image = AutoMapImage {};
        return image;
    }
    image.pixels.resize(static_cast<std::size_t>(image.width) * static_cast<std::size_t>(image.height) * Rgba8Stride);

    if (options.drawTiles) {
        if (autoMapData != nullptr && !autoMapData->IsEmpty()) {
            DrawAutoMapDataLayer(image, map, *autoMapData);
        } else {
            DrawTileLayer(image, map, resourceManager, palette);
        }
    }
    if (options.drawObjects) {
        DrawObjectLayer(image, map, resourceManager, palette);
    }
    if (options.drawCollision) {
        DrawCollisionLayer(image, map, options);
    }

    return image;
}

} // namespace cg::resource
