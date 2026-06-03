#include "resource/GraphicDecoder.hpp"

#include <cstddef>
#include <cstdint>
#include <iostream>
#include <span>
#include <vector>

namespace {

void PushI32Le(std::vector<std::uint8_t>& bytes, std::int32_t value)
{
    const auto unsignedValue = static_cast<std::uint32_t>(value);
    bytes.push_back(static_cast<std::uint8_t>(unsignedValue & 0xFFU));
    bytes.push_back(static_cast<std::uint8_t>((unsignedValue >> 8U) & 0xFFU));
    bytes.push_back(static_cast<std::uint8_t>((unsignedValue >> 16U) & 0xFFU));
    bytes.push_back(static_cast<std::uint8_t>((unsignedValue >> 24U) & 0xFFU));
}

std::vector<std::uint8_t> MakeUncompressedGraphicBlock(std::uint8_t attributes)
{
    std::vector<std::uint8_t> block;
    block.reserve(20U);
    block.push_back('R');
    block.push_back('D');
    block.push_back(static_cast<std::uint8_t>(cg::resource::CG_VER_UNZIP));
    block.push_back(attributes);
    PushI32Le(block, 2);
    PushI32Le(block, 2);
    PushI32Le(block, 20);
    block.push_back(0);
    block.push_back(1);
    block.push_back(2);
    block.push_back(0);
    return block;
}

cg::resource::Palette MakePalette()
{
    cg::resource::Palette::ColorArray colors {};
    colors[0] = cg::resource::Rgba8 {0, 0, 0, 255};
    colors[1] = cg::resource::Rgba8 {0, 0, 0, 255};
    colors[2] = cg::resource::Rgba8 {10, 20, 30, 255};
    return cg::resource::Palette::FromColors(colors);
}

std::size_t CountAlphaZero(std::span<const std::uint8_t> pixels)
{
    std::size_t count = 0;
    for (std::size_t offset = 3; offset < pixels.size(); offset += 4U) {
        if (pixels[offset] == 0) {
            ++count;
        }
    }
    return count;
}

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }
    std::cerr << message << '\n';
    return 1;
}

int TestPaletteZeroAlwaysTransparent()
{
    const cg::resource::GraphicDecoder decoder;
    const cg::resource::GraphicInfo info {};
    const std::vector<std::uint8_t> block = MakeUncompressedGraphicBlock(0);
    const cg::resource::GraphicDecodeResult result = decoder.DecodeBlock(info, block, MakePalette());

    int failures = 0;
    failures += Expect(result.Succeeded(), "Graphic 解码应成功");
    if (!result) {
        return failures;
    }

    failures += Expect(result.image.dataMetadata.zeroTransparent, "metadata 应标记 0 号色透明");
    failures += Expect(
        CountAlphaZero(std::span<const std::uint8_t>(result.image.pixels.data(), result.image.pixels.size())) == 2U,
        "两个 0 号色像素应透明");

    bool hasOpaqueBlack = false;
    for (std::size_t offset = 0; offset + 3U < result.image.pixels.size(); offset += 4U) {
        if (result.image.pixels[offset] == 0
            && result.image.pixels[offset + 1U] == 0
            && result.image.pixels[offset + 2U] == 0
            && result.image.pixels[offset + 3U] == 255) {
            hasOpaqueBlack = true;
        }
    }
    failures += Expect(hasOpaqueBlack, "非 0 色即使是黑色也应保持不透明");
    return failures;
}

} // namespace

int main()
{
    const int failures = TestPaletteZeroAlwaysTransparent();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }
    return 0;
}
