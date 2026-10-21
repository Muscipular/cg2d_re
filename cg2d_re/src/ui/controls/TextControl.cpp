#include "../WindowComponents.hpp"
#include <algorithm>
#include <bit>
#include <limits>
namespace cg::ui {
void WindowElement_DrawText(const WindowElement& element, int x, int y, const UIClipRect* clip) {
    const auto& data = *element.data.text;
    const auto font = std::bit_cast<std::int8_t>(std::to_integer<std::uint8_t>(data.style.font));
    const auto color = std::bit_cast<std::int8_t>(std::to_integer<std::uint8_t>(data.style.color));
    const bool box = data.layout == WindowTextLayout::Box;
    int textX = box ? x : x - element.width;
    if (box && element.width > 0 && data.align != WindowTextAlign::Left) {
        const int spacing = element.width - render::TextMeasure(data.text, font);
        textX += data.align == WindowTextAlign::Center ? spacing / 2 : spacing;
    }
    auto drawX = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(textX));
    const auto drawY = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(box ? y : y - element.height));
    int left = clip ? clip->left : std::numeric_limits<int>::min();
    int right = clip ? clip->right : std::numeric_limits<int>::max();
    int top = clip ? clip->top : std::numeric_limits<int>::min();
    int bottom = clip ? clip->bottom : std::numeric_limits<int>::max();
    // 正数尺寸与父裁剪相交；省略的尺寸为0，不限制该轴显示，也不回写元素几何。
    if (box && element.width > 0) {
        left = std::max(left, x); right = std::min(right, x + element.width);
    }
    if (box && element.height > 0) {
        top = std::max(top, y); bottom = std::min(bottom, y + element.height);
    }
    std::wstring clippedText;
    std::wstring_view value = data.text;
    if (clip || (box && (element.width > 0 || element.height > 0))) {
        if (left >= right || top >= bottom) return;
        const auto& metrics = render::GetFontMetrics(font);
        int fontHeight = metrics.fontHeight > 0 ? metrics.fontHeight : metrics.sHeight;
        if (fontHeight <= 0) fontHeight = 16;
        if (drawY < top || drawY + fontHeight > bottom) return;
        int currentX = drawX;
        for (std::size_t offset = 0; offset < value.size();) {
            const auto next = text::Next(value, offset);
            const auto glyph = value.substr(offset, next - offset);
            int width = render::TextMeasure(glyph, font);
            if (width <= 0) width = metrics.sWidth * (value[offset] < 0x80 ? 1 : 2);
            if (currentX >= left && currentX + width <= right) {
                if (clippedText.empty()) drawX = static_cast<std::int16_t>(currentX);
                clippedText.append(glyph);
            }
            currentX += width;
            offset = next;
        }
        if (clippedText.empty()) return;
        value = clippedText;
    }
    // 只裁去不完整的字形，仍用原生文本入口；不引入自动换行或颜色转义规则。
    render::DrawText2_456670(drawX, drawY, 3, font, color, value, 0);
}
}
