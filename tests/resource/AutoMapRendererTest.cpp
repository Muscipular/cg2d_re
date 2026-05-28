#include "core/Log.hpp"
#include "resource/AutoMapRenderer.hpp"

#include <array>
#include <cstdint>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <span>
#include <string_view>
#include <vector>

namespace cg::core {

void Log(LogLevel, std::string_view, std::string_view) {}
void LogSdlError(std::string_view, std::string_view) {}

} // namespace cg::core

namespace {

constexpr std::size_t PaletteFileColorStart = 16;
constexpr std::size_t PaletteFileColorCount = 224;
constexpr std::size_t BytesPerPaletteColor = 3;
constexpr std::uint8_t TileColorIndex = 16;
constexpr std::uint8_t ObjectColorIndex = 17;
constexpr std::int32_t EmptyColorMapNo = 1002;

void AppendI32Le(std::vector<std::uint8_t>& bytes, std::int32_t value)
{
    const auto unsignedValue = static_cast<std::uint32_t>(value);
    bytes.push_back(static_cast<std::uint8_t>(unsignedValue & 0xFFU));
    bytes.push_back(static_cast<std::uint8_t>((unsignedValue >> 8U) & 0xFFU));
    bytes.push_back(static_cast<std::uint8_t>((unsignedValue >> 16U) & 0xFFU));
    bytes.push_back(static_cast<std::uint8_t>((unsignedValue >> 24U) & 0xFFU));
}

void WriteBytes(const std::filesystem::path& path, const std::vector<std::uint8_t>& bytes)
{
    std::ofstream file(path, std::ios::binary);
    file.write(reinterpret_cast<const char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
}

void SetPaletteColor(std::vector<std::uint8_t>& bytes, std::uint8_t index, cg::resource::Rgba8 color)
{
    const std::size_t offset = (static_cast<std::size_t>(index) - PaletteFileColorStart) * BytesPerPaletteColor;
    bytes[offset] = color.b;
    bytes[offset + 1U] = color.g;
    bytes[offset + 2U] = color.r;
}

void WritePaletteFile(const std::filesystem::path& path)
{
    std::vector<std::uint8_t> bytes(PaletteFileColorCount * BytesPerPaletteColor, 0);
    SetPaletteColor(bytes, TileColorIndex, cg::resource::Rgba8 {250, 0, 0, 255});
    SetPaletteColor(bytes, ObjectColorIndex, cg::resource::Rgba8 {0, 250, 0, 255});
    WriteBytes(path, bytes);
}

std::vector<std::uint8_t> MakeGraphicBlock(std::uint8_t colorIndex)
{
    std::vector<std::uint8_t> bytes;
    bytes.push_back('R');
    bytes.push_back('D');
    bytes.push_back(0);
    bytes.push_back(0);
    AppendI32Le(bytes, 1);
    AppendI32Le(bytes, 1);
    AppendI32Le(bytes, 17);
    bytes.push_back(colorIndex);
    return bytes;
}

void AppendGraphicInfo(
    std::vector<std::uint8_t>& bytes,
    std::int32_t seqNo,
    std::int32_t offset,
    std::int32_t length,
    std::int8_t sx,
    std::int8_t sy,
    std::int8_t blockFlag,
    std::int32_t mapNo)
{
    AppendI32Le(bytes, seqNo);
    AppendI32Le(bytes, offset);
    AppendI32Le(bytes, length);
    AppendI32Le(bytes, 0);
    AppendI32Le(bytes, 0);
    AppendI32Le(bytes, 1);
    AppendI32Le(bytes, 1);
    bytes.push_back(static_cast<std::uint8_t>(sx));
    bytes.push_back(static_cast<std::uint8_t>(sy));
    bytes.push_back(static_cast<std::uint8_t>(blockFlag));
    bytes.push_back(0);
    bytes.insert(bytes.end(), 4, 0);
    AppendI32Le(bytes, mapNo);
}

void WriteResourceFiles(const std::filesystem::path& graphicInfoPath, const std::filesystem::path& graphicDataPath)
{
    std::vector<std::uint8_t> graphicData = MakeGraphicBlock(TileColorIndex);
    const std::vector<std::uint8_t> objectBlock = MakeGraphicBlock(ObjectColorIndex);
    graphicData.insert(graphicData.end(), objectBlock.begin(), objectBlock.end());
    const std::vector<std::uint8_t> emptyBlock = MakeGraphicBlock(0);
    graphicData.insert(graphicData.end(), emptyBlock.begin(), emptyBlock.end());

    std::vector<std::uint8_t> graphicInfo;
    AppendGraphicInfo(graphicInfo, 0, 0, 17, 1, 1, 1, 1000);
    AppendGraphicInfo(graphicInfo, 1, 17, 17, 2, 2, 0, 1001);
    AppendGraphicInfo(graphicInfo, 2, 34, 17, 1, 1, 1, EmptyColorMapNo);

    WriteBytes(graphicInfoPath, graphicInfo);
    WriteBytes(graphicDataPath, graphicData);
}

bool Expect(bool condition, const char* message)
{
    if (condition) {
        return true;
    }

    std::cerr << message << '\n';
    return false;
}

cg::resource::Rgba8 ReadPixel(const cg::resource::AutoMapImage& image, int x, int y)
{
    const std::size_t offset =
        (static_cast<std::size_t>(y) * static_cast<std::size_t>(image.width) + static_cast<std::size_t>(x)) * 4U;
    return cg::resource::Rgba8 {
        image.pixels[offset],
        image.pixels[offset + 1U],
        image.pixels[offset + 2U],
        image.pixels[offset + 3U],
    };
}

bool PixelEquals(const cg::resource::AutoMapImage& image, int x, int y, cg::resource::Rgba8 expected)
{
    const cg::resource::Rgba8 actual = ReadPixel(image, x, y);
    return actual.r == expected.r
        && actual.g == expected.g
        && actual.b == expected.b
        && actual.a == expected.a;
}

std::size_t CountVisiblePixels(const cg::resource::AutoMapImage& image)
{
    std::size_t count = 0;
    for (std::size_t offset = 3; offset < image.pixels.size(); offset += 4U) {
        if (image.pixels[offset] != 0) {
            ++count;
        }
    }
    return count;
}

cg::resource::MapData MakeMapData()
{
    cg::resource::MapData map;
    map.width = 2;
    map.height = 2;
    map.tileData = {1000, 0, 0, 0};
    map.objData = {0, 0, 1001, 0};
    map.hitData = {0, 0x4000, 0, 0};
    return map;
}

bool LoadTestResources(
    const std::filesystem::path& directory,
    cg::resource::ResourceManager& resourceManager,
    cg::resource::Palette& palette)
{
    const std::filesystem::path palettePath = directory / "test.cgp";
    const std::filesystem::path graphicInfoPath = directory / "GraphicInfo.bin";
    const std::filesystem::path graphicDataPath = directory / "Graphic.bin";

    WritePaletteFile(palettePath);
    WriteResourceFiles(graphicInfoPath, graphicDataPath);

    const cg::resource::PaletteLoadResult paletteResult = cg::resource::LoadPaletteFromFile(palettePath);
    if (!paletteResult) {
        return false;
    }

    cg::resource::BinSetDesc binSet;
    binSet.name = "test";
    binSet.graphicInfoPath = graphicInfoPath;
    binSet.graphicDataPath = graphicDataPath;
    palette = paletteResult.palette;
    return resourceManager.LoadBinSets(std::span<const cg::resource::BinSetDesc>(&binSet, 1), palettePath);
}

bool Run()
{
    const std::filesystem::path directory =
        std::filesystem::temp_directory_path() / "cg_automap_renderer_test";
    std::filesystem::remove_all(directory);
    std::filesystem::create_directories(directory);

    cg::resource::ResourceManager resourceManager;
    cg::resource::Palette palette;
    if (!Expect(LoadTestResources(directory, resourceManager, palette), "资源加载失败")) {
        return false;
    }

    const cg::resource::MapData map = MakeMapData();

    cg::resource::AutoMapRenderOptions options;
    options.drawObjects = false;
    cg::resource::AutoMapImage image = cg::resource::RenderAutoMapRgba8(map, resourceManager, palette, options);
    bool ok = true;
    ok &= Expect(image.width == 7 && image.height == 5, "自动地图输出尺寸错误");
    ok &= Expect(image.pixels.size() == static_cast<std::size_t>(image.width * image.height * 4), "RGBA8 buffer 大小错误");
    ok &= Expect(PixelEquals(image, 1, 3, cg::resource::Rgba8 {250, 0, 0, 255}), "tile 像素颜色错误");

    options.drawTiles = false;
    options.drawObjects = true;
    image = cg::resource::RenderAutoMapRgba8(map, resourceManager, palette, options);
    ok &= Expect(PixelEquals(image, 1, 3, cg::resource::Rgba8 {0, 250, 0, 255}), "object 覆盖像素颜色错误");

    options.drawObjects = false;
    options.drawCollision = false;
    image = cg::resource::RenderAutoMapRgba8(map, resourceManager, palette, options);
    ok &= Expect(CountVisiblePixels(image) == 0, "collision 关闭时应保持透明");

    options.drawCollision = true;
    options.collisionColor = cg::resource::Rgba8 {1, 2, 3, 4};
    image = cg::resource::RenderAutoMapRgba8(map, resourceManager, palette, options);
    ok &= Expect(PixelEquals(image, 3, 2, cg::resource::Rgba8 {1, 2, 3, 4}), "collision overlay 像素颜色错误");

    cg::resource::MapData missingColorMap;
    missingColorMap.width = 1;
    missingColorMap.height = 1;
    missingColorMap.tileData = {999};
    missingColorMap.objData = {0};
    missingColorMap.hitData = {0};
    options = cg::resource::AutoMapRenderOptions {};
    image = cg::resource::RenderAutoMapRgba8(missingColorMap, resourceManager, palette, options);
    ok &= Expect(CountVisiblePixels(image) == 0, "缺少颜色时应保持透明");

    missingColorMap.tileData = {EmptyColorMapNo};
    image = cg::resource::RenderAutoMapRgba8(missingColorMap, resourceManager, palette, options);
    ok &= Expect(CountVisiblePixels(image) == 0, "颜色索引 0 时应保持透明");

    std::filesystem::remove_all(directory);
    return ok;
}

} // namespace

int main()
{
    return Run() ? 0 : 1;
}
