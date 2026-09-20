#include "cg/render/Font.hpp"

#include <bit>

namespace cg::render {

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
