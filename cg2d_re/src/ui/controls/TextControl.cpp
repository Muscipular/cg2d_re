#include "../WindowComponents.hpp"
#include <bit>
namespace cg::ui {
void WindowElement_DrawText(const WindowElement& element, int x, int y, const UIClipRect* clip) {
    const auto& data = *element.data.text;
    const auto font = std::bit_cast<std::int8_t>(std::to_integer<std::uint8_t>(data.style.font));
    const auto color = std::bit_cast<std::int8_t>(std::to_integer<std::uint8_t>(data.style.color));
    auto drawX = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(x - element.width));
    const auto drawY = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(y - element.height));
    std::wstring clippedText;
    std::wstring_view value = data.text;
    if (clip) {
        const auto& metrics = render::GetFontMetrics(font);
        int fontHeight = metrics.fontHeight > 0 ? metrics.fontHeight : metrics.sHeight;
        if (fontHeight <= 0) fontHeight = 16;
        if (drawY < clip->top || drawY + fontHeight > clip->bottom) return;
        int currentX = drawX;
        for (std::size_t offset = 0; offset < value.size();) {
            const auto next = text::Next(value, offset);
            const auto glyph = value.substr(offset, next - offset);
            int width = render::TextMeasure(glyph, font);
            if (width <= 0) width = metrics.sWidth * (value[offset] < 0x80 ? 1 : 2);
            if (currentX >= clip->left && currentX + width <= clip->right) {
                if (clippedText.empty()) drawX = static_cast<std::int16_t>(currentX);
                clippedText.append(glyph);
            }
            currentX += width;
            offset = next;
        }
        if (clippedText.empty()) return;
        value = clippedText;
    }
    // 父裁剪只改变可见字形，仍用原生入口；不引入动态图文的颜色转义规则。
    render::DrawText2_456670(drawX, drawY, 3, font, color, value, 0);
}
}
