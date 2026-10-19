#pragma once

#include <cstddef>
#include <cstdint>
#include <string>
#include <string_view>
#include <vector>

namespace cg::render {

// 原 gFontList@0x558F68 每项去除首个原生 HFONT W32 后的固定度量与配置。
// 原生句柄不属于地址中性的度量状态，后续须由独立资源类型承载。
struct FontMetrics {
    std::int32_t sWidth; // 原成员偏移：+0x04。
    std::int32_t sHeight; // 原成员偏移：+0x08。
    std::int32_t doubleWidth; // 原成员偏移：+0x0C。
    std::int32_t doubleHeight; // 原成员偏移：+0x10。
    std::int8_t familyIndex; // 原成员偏移：+0x14。
    std::uint8_t fontHeight; // 原成员偏移：+0x15。
    std::uint8_t field16; // 原成员偏移：+0x16。
    std::uint8_t field17; // 原成员偏移：+0x17。
};

inline constexpr std::int32_t kMaxFontId = 32;

// FC01：查询私有字体map；未注册或加载失败时统一回退0号，不插入新条目。
[[nodiscard]] std::int32_t ResolveFontId(std::int32_t id) noexcept;
[[nodiscard]] const FontMetrics& GetFontMetrics(std::int32_t id) noexcept;
// BMUI01：图形标记与文字共用现有128色表，返回0xAARRGGBB。
[[nodiscard]] std::uint32_t GetTextColor(std::uint8_t color) noexcept;

// 原456190/456530；FC01统一管理字体、私有集合和GDI+会话，支持重复初始化/释放。
std::int32_t InitializeFont_456190(std::int32_t family);
// 原函数：0x00456530。
std::int32_t ReleaseFont_456530();

// R06：字体或渲染器重建／释放时清除字形图集及栅格化资源。
void ResetFontGlyphCache() noexcept;

// sub_45D990@0x45D990 的直接目标。
// 原函数：0x0044B2A0。
std::int32_t TextMeasure(std::wstring_view text, std::int32_t fontType);
// 原函数：0x0044B260。
std::int32_t MeasureDefaultText_44B260(std::wstring_view text);

class Font final {
public:
    Font() = delete;

    // 原0x0044B2E0：maxUnits为UTF-16码元预算；R06与字形缓存使用同一步进，不截断代理对。
    [[nodiscard]] static std::int32_t measure(std::wstring_view text, std::int32_t fontType, std::int32_t maxUnits);
    [[nodiscard]] static std::vector<std::wstring> split(std::wstring_view text, std::int32_t fontType, std::int32_t width);

};

static_assert(sizeof(FontMetrics) == 0x14);
static_assert(offsetof(FontMetrics, sWidth) == 0x00);
static_assert(offsetof(FontMetrics, doubleWidth) == 0x08);
static_assert(offsetof(FontMetrics, doubleHeight) == 0x0C);
static_assert(offsetof(FontMetrics, familyIndex) == 0x10);
static_assert(offsetof(FontMetrics, field17) == 0x13);

} // namespace cg::render
