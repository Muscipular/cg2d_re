#include "cg/render/Font.hpp"

#include <bit>

#define WIN32_LEAN_AND_MEAN
#define NOMINMAX
#include <Windows.h>

namespace cg::render {
namespace {

// 原 5590E0；558F08 起四个可写族指针的初值均指向这段窄字节。
constexpr char kFaceName5590E0[] = "\xB2\xD3\xA9\xFA\xC5\xE9";
std::array<const char*, 4> gFaceFamilies558F08 {
    kFaceName5590E0, kFaceName5590E0, kFaceName5590E0, kFaceName5590E0,
};

// 原 gFontList 每项 +00 是 GDI 句柄；与 0x14 字节度量分开承载。
std::array<HFONT, kFontTypeCount> gFontHandles558F68 {};
std::array<std::int32_t, 8> gDerived558F30 {};
std::array<std::int32_t, 8> gDerived5545AC {};
// 原 554618 初值 0x39；随后每项 55461C+i*4 先写钳制值再被商覆盖。
std::array<std::int32_t, 9> gDerived554618 {{0x39}};

static_assert(sizeof(kFaceName5590E0) == 7);
static_assert(sizeof(gDerived558F30) == 0x20);
static_assert(sizeof(gDerived5545AC) == 0x20);

HFONT CreateLegacyFont(
    const std::int32_t height,
    const std::int32_t width,
    const std::int32_t weight,
    const char* const face)
{
    return ::CreateFontA(
        height, width, 0, 0, weight, 0, 0, 0,
        DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS,
        DEFAULT_QUALITY, 0x11, face);
}

} // namespace

std::array<FontMetrics, kFontTypeCount> gFontMetrics {{
    {9, 17, 17, 17, 0, 0, 0, 0},
    {8, 15, 15, 15, 0, 0, 0, 0},
    {11, 21, 21, 21, 0, 0, 0, 0},
    {9, 17, 17, 17, 0, 0, 0, 0},
    {8, 15, 15, 15, 0, 0, 0, 0},
    {11, 21, 21, 21, 0, 0, 0, 0},
    {11, 21, 21, 21, 1, 20, 0, 0},
    {25, 49, 49, 49, -1, 48, 0, 0},
    {5, 11, 10, 11, 0, 11, 0, 0},
    {6, 12, 12, 12, -1, 12, 0, 0},
    {8, 16, 16, 16, -1, 16, 0, 0},
    {10, 20, 20, 20, -1, 20, 0, 0},
    {6, 12, 12, 12, 0, 12, 0, 0},
    {7, 13, 13, 13, 0, 12, 0, 0},
}};

// 原 [456190,456528)：旧句柄只删除不清槽，索引 6 不创建新句柄。
std::int32_t InitializeFont_456190(const std::int32_t family)
{
    for (HFONT handle : gFontHandles558F68) {
        if (handle != nullptr) {
            static_cast<void>(::DeleteObject(handle));
        }
    }

    constexpr std::array<std::int32_t, 3> kBoldHeights {16, 14, 20};
    for (std::size_t index = 0; index < kBoldHeights.size(); ++index) {
        gFontHandles558F68[index] =
            CreateLegacyFont(kBoldHeights[index], 0, FW_BOLD, kFaceName5590E0);
    }
    for (std::size_t index = 0; index < kBoldHeights.size(); ++index) {
        gFontHandles558F68[index + 3] =
            CreateLegacyFont(kBoldHeights[index], 0, FW_BOLD, kFaceName5590E0);
    }

    for (std::size_t index = 7; index <= 12; ++index) {
        FontMetrics& metrics = gFontMetrics[index];
        if (metrics.familyIndex < 0) {
            metrics.familyIndex = static_cast<std::int8_t>(family);
        }
        const char* const face = gFaceFamilies558F08[
            static_cast<std::size_t>(metrics.familyIndex)];
        const std::int32_t width = index == 7 ? 0 : metrics.sWidth;
        const std::int32_t weight = index == 7 ? FW_BOLD : FW_NORMAL;
        gFontHandles558F68[index] =
            CreateLegacyFont(metrics.fontHeight, width, weight, face);
    }
    gFontHandles558F68[13] = CreateLegacyFont(12, 0, FW_BOLD, kFaceName5590E0);

    for (std::size_t index = 0; index < 8; ++index) {
        gDerived558F30[index] =
            static_cast<std::int32_t>(gFontMetrics[index + 6].fontHeight / 4) - 1;
    }

    std::int32_t result = 0;
    for (std::size_t index = 0; index < 8; ++index) {
        const FontMetrics& metrics = gFontMetrics[index + 6];
        const std::int32_t width = metrics.doubleWidth;
        const std::int32_t height = metrics.fontHeight;
        gDerived558F30[index] = height / 4 - 1;
        gDerived5545AC[index] = 20 - height;
        std::int32_t difference = 25 - width;
        gDerived554618[index + 1] = difference;
        if (difference < 1) {
            gDerived554618[index + 1] = 1;
        }
        result = 1980 / width;
        gDerived554618[index + 1] = result;
    }
    return result;
}

// 原 [456530,456554)：末项无句柄返回零，否则透传其 DeleteObject 返回。
std::int32_t ReleaseFont_456530()
{
    std::int32_t result = 0;
    for (HFONT handle : gFontHandles558F68) {
        result = handle == nullptr ? 0 : ::DeleteObject(handle);
    }
    return result;
}

std::int32_t Font::measure(
    const char* text,
    std::int32_t fontType,
    std::int32_t maxBytes) noexcept
{
    if (maxBytes <= 0) {
        return 0;
    }
    if (text == nullptr
        || fontType < 0
        || fontType >= static_cast<std::int32_t>(kFontTypeCount)) {
        return 0;
    }

    const auto* cursor = reinterpret_cast<const std::uint8_t*>(text);
    std::uint32_t totalWidth = 0;
    std::uint32_t consumed = 0;
    const auto index = static_cast<std::size_t>(fontType);

    for (;;) {
        const std::uint8_t firstByte = *cursor;
        if (firstByte == 0) {
            break;
        }

        std::int32_t width;
        if ((firstByte & 0x80U) == 0) {
            ++cursor;
            consumed += 1U;
            width = gFontMetrics[index].sWidth;
        } else {
            cursor += 2;
            consumed += 2U;
            width = gFontMetrics[index].doubleWidth;
        }
        totalWidth += static_cast<std::uint32_t>(width);

        if (std::bit_cast<std::int32_t>(consumed) >= maxBytes) {
            break;
        }
    }

    return std::bit_cast<std::int32_t>(totalWidth);
}

} // namespace cg::render

namespace cg::login::detail {

// 原 TextMeasure@[0x44B2A0,0x44B2DC)：无字节预算的旧编码文本测宽。
std::int32_t TextMeasure(const char* text, std::int32_t fontType)
{
    if (text == nullptr) {
        return 0;
    }

    const auto* cursor = reinterpret_cast<const std::uint8_t*>(text);
    std::uint8_t firstByte = *cursor;
    if (firstByte == 0) {
        return 0;
    }
    if (fontType < 0
        || fontType >= static_cast<std::int32_t>(cg::render::kFontTypeCount)) {
        return 0;
    }

    const auto index = static_cast<std::size_t>(fontType);
    std::uint32_t totalWidth = 0;
    for (;;) {
        std::int32_t width;
        if ((firstByte & 0x80U) == 0) {
            ++cursor;
            width = cg::render::gFontMetrics[index].sWidth;
        } else {
            cursor += 2;
            width = cg::render::gFontMetrics[index].doubleWidth;
        }

        firstByte = *cursor;
        totalWidth += static_cast<std::uint32_t>(width);
        if (firstByte == 0) {
            break;
        }
    }

    return std::bit_cast<std::int32_t>(totalWidth);
}

std::int32_t& gValue558F78 = cg::render::gFontMetrics[0].doubleHeight;

} // namespace cg::login::detail
