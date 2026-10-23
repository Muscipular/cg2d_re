#include "../WindowComponents.hpp"
#include <algorithm>
#include <array>
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
        const int spacing = element.width - render::MeasureColoredText(data.text, font);
        textX += data.align == WindowTextAlign::Center ? spacing / 2 : spacing;
    }
    const auto drawX = std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(textX));
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
    const bool clipping = clip || (box && (element.width > 0 || element.height > 0));
    if (clipping && (left >= right || top >= bottom)) return;
    const std::array<std::int32_t, 4> bounds {left, top, right, bottom};
    // 队列复制裁剪区域，最终绘制字形及阴影纹理时裁剪，不再丢整字或整行。
    if (box) {
        // Lua文本框独立占据本元素的绘制顺序，不能追加到前方控件的文字链穿过背景。
        render::ClearTextQueueTail_456D50(3);
        render::DrawColoredText(drawX, drawY, 3, font, color, data.text, 0,
            std::bit_cast<std::int8_t>(std::uint8_t{0xF9}), clipping ? &bounds : nullptr);
        render::ClearTextQueueTail_456D50(3);
    } else {
        render::DrawText2_456670(drawX, drawY, 3, font, color, data.text, 0, clipping ? &bounds : nullptr);
    }
}
}
