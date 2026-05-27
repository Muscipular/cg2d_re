#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <filesystem>
#include <span>

namespace cg::resource {

// Rgba8 表示资源解码边界使用的 8 位 RGBA 颜色。
struct Rgba8 {
    std::uint8_t r = 0;
    std::uint8_t g = 0;
    std::uint8_t b = 0;
    std::uint8_t a = 255;
};

// Palette 保存旧客户端 256 色调色板，颜色顺序为 RGBA8。
class Palette final {
public:
    static constexpr std::size_t ColorCount = 256;
    using ColorArray = std::array<Rgba8, ColorCount>;

    Palette();

    [[nodiscard]] static Palette FromColors(ColorArray colors);

    [[nodiscard]] const ColorArray& Colors() const noexcept;
    [[nodiscard]] const Rgba8& operator[](std::uint8_t index) const noexcept;

private:
    explicit Palette(ColorArray colors);

    ColorArray colors_ {};
};

// PaletteLoadError 表示 .cgp 调色板读取错误。
enum class PaletteLoadError {
    None,
    FileOpenFailed,
    UnexpectedEof,
};

// PaletteLoadResult 返回调色板读取结果与失败上下文。
struct PaletteLoadResult {
    Palette palette;
    PaletteLoadError error = PaletteLoadError::None;
    std::filesystem::path path;

    [[nodiscard]] bool Succeeded() const noexcept
    {
        return error == PaletteLoadError::None;
    }

    [[nodiscard]] explicit operator bool() const noexcept
    {
        return Succeeded();
    }
};

// LoadPaletteFromFile 读取 .cgp 中间 224 色，并补齐固定头尾 16 色。
[[nodiscard]] PaletteLoadResult LoadPaletteFromFile(const std::filesystem::path& path);

// LoadPaletteFromBytes 从 .cgp 原始字节读取调色板，便于离线验证。
[[nodiscard]] PaletteLoadResult LoadPaletteFromBytes(std::span<const std::uint8_t> bytes);

} // namespace cg::resource
