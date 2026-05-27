#include "resource/Palette.hpp"

#include <fstream>
#include <iterator>
#include <vector>

namespace cg::resource {
namespace {

constexpr std::size_t FileColorStart = 16;
constexpr std::size_t FileColorCount = 224;
constexpr std::size_t BytesPerColor = 3;
constexpr std::size_t RequiredCgpBytes = FileColorCount * BytesPerColor;

[[nodiscard]] constexpr Rgba8 Color(std::uint8_t r, std::uint8_t g, std::uint8_t b)
{
    return Rgba8 {r, g, b, 255};
}

[[nodiscard]] constexpr Rgba8 ColorFromBgr(const std::array<std::uint8_t, 48>& bytes, std::size_t index)
{
    const std::size_t offset = index * BytesPerColor;
    return Color(bytes[offset + 2], bytes[offset + 1], bytes[offset]);
}

[[nodiscard]] Palette::ColorArray BuildFixedPalette()
{
    Palette::ColorArray colors {};

    constexpr std::array<std::uint8_t, 48> head {
        0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x80, 0x00, 0x00, 0x80, 0x80, 0x00,
        0x00, 0x00, 0x80, 0x00, 0x80, 0x80, 0x80, 0x00, 0x80, 0xc0, 0xc0, 0xc0,
        0xc0, 0xdc, 0xc0, 0xf0, 0xca, 0xa6, 0x00, 0x00, 0xde, 0x00, 0x5f, 0xff,
        0xa0, 0xff, 0xff, 0xd2, 0x5f, 0x00, 0xff, 0xd2, 0x50, 0x28, 0xe1, 0x28,
    };

    constexpr std::array<std::uint8_t, 48> tail {
        0x96, 0xc3, 0xf5, 0x5f, 0xa0, 0x1e, 0x46, 0x7d, 0xc3, 0x1e, 0x55, 0x9b,
        0x37, 0x41, 0x46, 0x1e, 0x23, 0x28, 0xf0, 0xfb, 0xff, 0xa5, 0x6e, 0x3a,
        0x80, 0x80, 0x80, 0x00, 0x00, 0xff, 0x00, 0xff, 0x00, 0x00, 0xff, 0xff,
        0xff, 0x00, 0x00, 0xff, 0x80, 0xff, 0xff, 0xff, 0x00, 0xff, 0xff, 0xff,
    };

    for (std::size_t i = 0; i < 16; ++i) {
        colors[i] = ColorFromBgr(head, i);
    }
    for (std::size_t i = 0; i < 16; ++i) {
        colors[Palette::ColorCount - 16 + i] = ColorFromBgr(tail, i);
    }

    return colors;
}

} // namespace

Palette::Palette()
    : colors_(BuildFixedPalette())
{
}

Palette::Palette(ColorArray colors)
    : colors_(colors)
{
}

Palette Palette::FromColors(ColorArray colors)
{
    return Palette(colors);
}

const Palette::ColorArray& Palette::Colors() const noexcept
{
    return colors_;
}

const Rgba8& Palette::operator[](std::uint8_t index) const noexcept
{
    return colors_[index];
}

PaletteLoadResult LoadPaletteFromFile(const std::filesystem::path& path)
{
    PaletteLoadResult result;
    result.path = path;

    std::ifstream file(path, std::ios::binary);
    if (!file) {
        result.error = PaletteLoadError::FileOpenFailed;
        return result;
    }

    const std::vector<std::uint8_t> bytes {
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>(),
    };

    result = LoadPaletteFromBytes(bytes);
    result.path = path;
    return result;
}

PaletteLoadResult LoadPaletteFromBytes(std::span<const std::uint8_t> bytes)
{
    PaletteLoadResult result;

    if (bytes.size() < RequiredCgpBytes) {
        result.error = PaletteLoadError::UnexpectedEof;
        return result;
    }

    auto colors = BuildFixedPalette();
    for (std::size_t i = 0; i < FileColorCount; ++i) {
        const std::size_t source = i * BytesPerColor;
        colors[FileColorStart + i] = Rgba8 {
            bytes[source + 2],
            bytes[source + 1],
            bytes[source],
            255,
        };
    }

    result.palette = Palette::FromColors(colors);
    return result;
}

} // namespace cg::resource
