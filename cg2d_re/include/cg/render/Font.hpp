#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

namespace cg::render {

// 原 gFontList@0x558F68 每项去除首个原生 HFONT W32 后的固定度量与配置。
// 原生句柄不属于地址中性的度量状态，后续须由独立资源类型承载。
struct FontMetrics {
    std::int32_t sWidth;
    std::int32_t sHeight;
    std::int32_t doubleWidth;
    std::int32_t doubleHeight;
    std::int8_t familyIndex;
    std::uint8_t fontHeight;
    std::uint8_t field16;
    std::uint8_t field17;
};

inline constexpr std::size_t kFontTypeCount = 14;

// 原 gFontList@0x558F68 的唯一地址中性度量状态，索引有效域为 0..13。
extern std::array<FontMetrics, kFontTypeCount> gFontMetrics;

// 原 456190/456530；GDI 句柄由字体边界唯一持有，退出后保留原槽残值。
std::int32_t InitializeFont_456190(std::int32_t family);
std::int32_t ReleaseFont_456530();

class Font final {
public:
    Font() = delete;

    // 原 Font::measure@[0x44B2E0,0x44B32A)。maxBytes 是旧编码字节预算。
    [[nodiscard]] static std::int32_t measure(
        const char* text,
        std::int32_t fontType,
        std::int32_t maxBytes) noexcept;
};

static_assert(sizeof(FontMetrics) == 0x14);
static_assert(offsetof(FontMetrics, sWidth) == 0x00);
static_assert(offsetof(FontMetrics, doubleWidth) == 0x08);
static_assert(offsetof(FontMetrics, doubleHeight) == 0x0C);
static_assert(offsetof(FontMetrics, familyIndex) == 0x10);
static_assert(offsetof(FontMetrics, field17) == 0x13);

} // namespace cg::render
