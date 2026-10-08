#include "cg/render/Font.hpp"

#include "cg/render/TextPendingQueue.hpp"
#include "BackTextDrawing.hpp"

#include <bit>
#include <cstring>

namespace cg::render {
namespace {

// 原 5590E0；558F08 起四个可写族指针的初值均指向这段窄字节。
constexpr char kFaceName5590E0[] = "\xB2\xD3\xA9\xFA\xC5\xE9";
std::array<const char*, 4> gFaceFamilies558F08 {
    kFaceName5590E0, kFaceName5590E0, kFaceName5590E0, kFaceName5590E0,
};

// 原 gFontList 每项 +00 是 GDI 句柄；与 0x14 字节度量分开承载。
std::array<HFONT, kFontTypeCount> gFontHandles558F68 {};
// 原 [5590B8,5590DF) 三字节色表；按字节组装 COLORREF，不按反编译字段名换色。
std::uint8_t gTextColors5590B8[13][3] {
    {0xFF, 0xFF, 0xFF},
    {0x00, 0xFF, 0xFF},
    {0xFF, 0x00, 0xFF},
    {0x00, 0x00, 0xFF},
    {0xFF, 0xFF, 0x00},
    {0x00, 0xFF, 0x00},
    {0xFF, 0x00, 0x00},
    {0xA0, 0xA0, 0xA4},
    {0xA6, 0xCA, 0xF0},
    {0xC0, 0xDC, 0xC0},
    {0x00, 0x00, 0x00},
    {0xCD, 0xCD, 0xCD},
    {0x66, 0x54, 0x25}
};
static_assert(sizeof(gTextColors5590B8) == 39);
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

const TextPendingRecord* BorrowTextRecord(const std::uint32_t token)
{
    return token == 0 ? nullptr : &gTextPendingRecords[token - 1];
}

// 每次重新扫描原字节长度，然后按 Y→X 读取坐标；不在此缓存字体或颜色。
void TextOutRecord(
    const HDC dc, const TextPendingRecord& record, const std::int32_t offset)
{
    const int length = static_cast<int>(std::strlen(record.text));
    const std::int32_t y = record.y + offset;
    const std::int32_t x = record.x + offset;
    static_cast<void>(::TextOutA(dc, x, y, record.text, length));
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
    const std::array<const char*, 3> initialFaces {
        kFaceName5590E0, kFaceName5590E0, kFaceName5590E0,
    };
    for (HFONT handle : gFontHandles558F68) {
        if (handle != nullptr) {
            static_cast<void>(::DeleteObject(handle));
        }
    }

    const char* const initialFace = initialFaces[static_cast<std::size_t>(family)];
    constexpr std::array<std::int32_t, 3> kBoldHeights {16, 14, 20};
    for (std::size_t index = 0; index < kBoldHeights.size(); ++index) {
        gFontHandles558F68[index] =
            CreateLegacyFont(kBoldHeights[index], 0, FW_BOLD, initialFace);
    }
    for (std::size_t index = 0; index < kBoldHeights.size(); ++index) {
        gFontHandles558F68[index + 3] =
            CreateLegacyFont(kBoldHeights[index], 0, FW_BOLD, kFaceName5590E0);
    }

    for (std::size_t index = 7; index <= 12; ++index) {
        FontMetrics& metrics = gFontMetrics[index];
        std::int8_t familyIndex = metrics.familyIndex;
        if (familyIndex < 0) {
            familyIndex = std::bit_cast<std::int8_t>(
                static_cast<std::uint8_t>(family));
            metrics.familyIndex = familyIndex;
        }
        const std::int32_t width = index == 7 ? 0 : metrics.sWidth;
        const char* const face = gFaceFamilies558F08[
            static_cast<std::size_t>(familyIndex)];
        const std::int32_t height = metrics.fontHeight;
        const std::int32_t weight = index == 7 ? FW_BOLD : FW_NORMAL;
        gFontHandles558F68[index] =
            CreateLegacyFont(height, width, weight, face);
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
        if (std::bit_cast<std::uint32_t>(difference) < 1U) {
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

void RenderTextToSurface_456D70(const std::uint32_t headToken)
{
    const HDC dc = AcquireCpuBackTextDc();
    static_cast<void>(::SetBkMode(dc, TRANSPARENT));
    const TextPendingRecord* const head = BorrowTextRecord(headToken);
    if (head->renderMode == 0) {
        const TextPendingRecord* record = head;
        while (record != nullptr) {
            if (record->text[0] != '\0') {
                static_cast<void>(::SelectObject(dc, gFontHandles558F68[record->fontType]));
                const std::uint8_t color = record->color;
                if ((color & 0x80u) == 0) {
                    static_cast<void>(::SetTextColor(dc, 0));
                    static_cast<void>(::SelectObject(dc, gFontHandles558F68[record->fontType]));
                    TextOutRecord(dc, *record, 1);
                }
                const std::uint8_t index = color & 0x7Fu;
                const std::uint32_t third = gTextColors5590B8[index][2];
                const std::uint32_t first = gTextColors5590B8[index][0];
                const std::uint32_t second = gTextColors5590B8[index][1];
                static_cast<void>(::SetTextColor(dc, first | (second << 8) | (third << 16)));
                TextOutRecord(dc, *record, 0);
            }
            record = BorrowTextRecord(record->nextToken);
        }
    } else {
        static_cast<void>(::SetTextColor(dc, 0));
        const TextPendingRecord* record = head;
        while (record != nullptr) {
            if ((record->color & 0x80u) == 0) {
                static_cast<void>(::SelectObject(dc, gFontHandles558F68[record->fontType]));
                TextOutRecord(dc, *record, 1);
            }
            record = BorrowTextRecord(record->nextToken);
        }
        for (std::int32_t index = 0; index < 13; ++index) {
            bool selectedColor = false;
            record = head;
            while (record != nullptr) {
                if (static_cast<std::int32_t>(record->color & 0x7Fu) == index) {
                    if (!selectedColor) {
                        const std::uint32_t third = gTextColors5590B8[index][2];
                        const std::uint32_t second = gTextColors5590B8[index][1];
                        const std::uint32_t first = gTextColors5590B8[index][0];
                        static_cast<void>(::SetTextColor(dc, first | (second << 8) | (third << 16)));
                        selectedColor = true;
                    }
                    static_cast<void>(::SelectObject(dc, gFontHandles558F68[record->fontType]));
                    TextOutRecord(dc, *record, 0);
                }
                record = BorrowTextRecord(record->nextToken);
            }
        }
    }
    ReleaseCpuBackTextDc(dc);
}

std::int32_t Font::measure(
    const char* text,
    std::int32_t fontType,
    std::int32_t maxBytes) noexcept
{
    if (maxBytes <= 0) {
        return 0;
    }

    const auto* cursor = reinterpret_cast<const std::uint8_t*>(text);
    std::int32_t totalWidth = 0;
    std::int32_t consumed = 0;
    const auto index = static_cast<std::size_t>(fontType);

    for (;;) {
        const std::uint8_t firstByte = *cursor;
        if (firstByte == 0) {
            break;
        }

        std::int32_t width;
        if ((firstByte & 0x80U) == 0) {
            ++cursor;
            width = gFontMetrics[index].sWidth;
        } else {
            cursor += 2;
            ++consumed;
            width = gFontMetrics[index].doubleWidth;
        }
        totalWidth += width;
        // 原44B320为两支路共用的最后一次消费计数；高位分支此前另计一次。
        ++consumed;

        if (consumed >= maxBytes) {
            break;
        }
    }

    return totalWidth;
}

} // namespace cg::render

namespace cg::login::detail {

std::int32_t MeasureDefaultText_44B260(const char* text)
{
    const auto* cursor = reinterpret_cast<const std::uint8_t*>(text);
    std::uint8_t first = *cursor;
    if (first == 0) {
        return 0;
    }
    const std::int32_t doubleWidth = cg::render::gFontMetrics[0].doubleWidth;
    const std::int32_t singleWidth = cg::render::gFontMetrics[0].sWidth;
    std::int32_t width = 0;
    do {
        if ((first & 0x80u) != 0) {
            cursor += 2;
            width += doubleWidth;
        } else {
            ++cursor;
            width += singleWidth;
        }
        first = *cursor;
    } while (first != 0);
    return width;
}

// 原 TextMeasure@[0x44B2A0,0x44B2DC)：无字节预算的旧编码文本测宽。
std::int32_t TextMeasure(const char* text, std::int32_t fontType)
{
    const auto* cursor = reinterpret_cast<const std::uint8_t*>(text);
    std::uint8_t firstByte = *cursor;
    if (firstByte == 0) {
        return 0;
    }

    const auto index = static_cast<std::size_t>(fontType);
    std::int32_t totalWidth = 0;
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
        totalWidth += width;
        if (firstByte == 0) {
            break;
        }
    }

    return totalWidth;
}

std::int32_t& gValue558F78 = cg::render::gFontMetrics[0].doubleHeight;

} // namespace cg::login::detail
