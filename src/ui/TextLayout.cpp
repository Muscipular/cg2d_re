#include "TextLayout.hpp"

#include <algorithm>
#include <cstddef>
#include <string>
#include <string_view>
#include <utility>

namespace cg::ui {
namespace {

[[nodiscard]] bool IsUtf8Continuation(unsigned char value) noexcept
{
    return (value & 0xC0U) == 0x80U;
}

[[nodiscard]] std::size_t NextUtf8CodepointSize(std::string_view text, std::size_t offset) noexcept
{
    const auto first = static_cast<unsigned char>(text[offset]);
    const std::size_t remaining = text.size() - offset;

    if ((first & 0x80U) == 0U) {
        return 1;
    }
    if ((first & 0xE0U) == 0xC0U && remaining >= 2 &&
        IsUtf8Continuation(static_cast<unsigned char>(text[offset + 1]))) {
        return 2;
    }
    if ((first & 0xF0U) == 0xE0U && remaining >= 3 &&
        IsUtf8Continuation(static_cast<unsigned char>(text[offset + 1])) &&
        IsUtf8Continuation(static_cast<unsigned char>(text[offset + 2]))) {
        return 3;
    }
    if ((first & 0xF8U) == 0xF0U && remaining >= 4 &&
        IsUtf8Continuation(static_cast<unsigned char>(text[offset + 1])) &&
        IsUtf8Continuation(static_cast<unsigned char>(text[offset + 2])) &&
        IsUtf8Continuation(static_cast<unsigned char>(text[offset + 3]))) {
        return 4;
    }

    return 1;
}

void AddLine(
    TextLayoutResult& result,
    const render::FontCache& fontCache,
    std::string_view fontKey,
    std::string_view text)
{
    const render::TextMetrics metrics = fontCache.MeasureText(fontKey, text);
    const int width = metrics.valid ? metrics.width : 0;
    const int height = metrics.valid ? metrics.height : 0;

    result.lines.push_back(TextLine{std::string{text}, width, height});
    result.width = std::max(result.width, width);
    result.height += height;
}

void LayoutParagraph(
    TextLayoutResult& result,
    const render::FontCache& fontCache,
    std::string_view fontKey,
    std::string_view paragraph,
    int maxWidth)
{
    if (maxWidth <= 0 || paragraph.empty()) {
        AddLine(result, fontCache, fontKey, paragraph);
        return;
    }

    std::string currentLine;
    std::size_t offset = 0;
    while (offset < paragraph.size()) {
        const std::size_t codepointSize = NextUtf8CodepointSize(paragraph, offset);
        const std::string_view codepoint = paragraph.substr(offset, codepointSize);

        std::string candidate = currentLine;
        candidate.append(codepoint);

        const render::TextMetrics metrics = fontCache.MeasureText(fontKey, candidate);
        if (metrics.valid && metrics.width > maxWidth && !currentLine.empty()) {
            AddLine(result, fontCache, fontKey, currentLine);
            currentLine.assign(codepoint);
        } else {
            currentLine = std::move(candidate);
        }

        offset += codepointSize;
    }

    AddLine(result, fontCache, fontKey, currentLine);
}

} // namespace

TextLayoutResult LayoutText(
    const render::FontCache& fontCache,
    std::string_view fontKey,
    std::string_view text,
    int maxWidth)
{
    TextLayoutResult result;
    std::size_t paragraphStart = 0;

    while (paragraphStart <= text.size()) {
        const std::size_t lineBreak = text.find('\n', paragraphStart);
        if (lineBreak == std::string_view::npos) {
            LayoutParagraph(result, fontCache, fontKey, text.substr(paragraphStart), maxWidth);
            break;
        }

        LayoutParagraph(result, fontCache, fontKey, text.substr(paragraphStart, lineBreak - paragraphStart), maxWidth);
        paragraphStart = lineBreak + 1;
    }

    return result;
}

} // namespace cg::ui
