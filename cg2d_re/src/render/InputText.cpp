#include "cg/render/TextPendingQueue.hpp"
#include "cg/text/Encoding.hpp"

#include "cg/input/Input.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicCacheState.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

namespace cg::render {

// 原函数：0x004568A0。
void DrawInputText_4568A0(input::InputContext* const context)
{
    const std::uint32_t initialCountBits = platform::gPendingGraphicCount2;
    if (std::bit_cast<std::int32_t>(initialCountBits) >= MAX_QUEUE_SIZE) {
        return;
    }
    const std::wstring contents = input::ContextText(*context);
    const std::size_t caret = input::ContextUtf16Offset(*context, context->caretByteOffset);
    std::size_t caretLineStart = 0;
    std::int32_t caretLineYOffset = 0;
    if (context->maxLine >= 2) {
        const auto lines = input::SplitLines(*context);
        std::int32_t yOffset = 0;
        for (std::size_t index = 0; index < lines.size(); ++index) {
            const input::TextLine& line = lines[index];
            const std::wstring displayed = contents.substr(line.start, line.end - line.start);
            const bool ownsCaret = index + 1 == lines.size() ? caret >= line.start : caret >= line.start && caret < line.nextStart;
            if (ownsCaret) {
                caretLineStart = line.start;
                caretLineYOffset = yOffset;
            }
            // 原0x004569E9..0x00456A19：按当前颜色／字体／模式／Y／X取样。
            const std::int32_t color = context->field140;
            const std::int32_t fontType = ResolveFontId(context->field13C);
            const std::int8_t renderMode = static_cast<std::int8_t>(context->field138);
            const std::int32_t y = context->field134;
            const std::int32_t x = context->field130;
            static_cast<void>(DrawText2_456670(static_cast<std::int16_t>(x), static_cast<std::int16_t>(y + yOffset), renderMode,
                static_cast<std::int8_t>(fontType), static_cast<std::int8_t>(color), displayed.c_str(), 0));
            yOffset = yOffset + context->field14C;
        }
    } else {
        TextPendingRecord& entry = gTextPendingRecords[static_cast<std::size_t>(initialCountBits)];
        entry.x = static_cast<std::int16_t>(context->field130);
        entry.y = static_cast<std::int16_t>(context->field134);
        entry.renderMode = static_cast<std::uint8_t>(context->field138);
        entry.color = static_cast<std::uint8_t>(context->field140);
        entry.fontType = static_cast<std::uint8_t>(ResolveFontId(context->field13C));
        entry.value7 = 0;
        entry.clipRectangle.reset(); // 单行输入直接复用队列槽，不沿用UI文本框裁剪。
        const std::size_t visibleStart = input::ContextUtf16Offset(*context, context->field158);
        const std::wstring_view visible(contents.data() + visibleStart, contents.size() - visibleStart);
        if (context->encrypt >= 1) {
            entry.text.assign(text::Count(visible), L'*');
        } else {
            entry.text = visible;
        }
        // 原0x00456B46..0x00456B50的含NUL复制由拥有型字符串承接。
        if (context->field154 != 0) {
            while (!entry.text.empty() && Font::measure(entry.text.c_str(), context->field13C,
                    static_cast<std::int32_t>(entry.text.size())) > context->field154) {
                entry.text.resize(text::Previous(entry.text, entry.text.size()));
            }
        }
        entry.nextToken = 0;
        // 原456BED/456C0B/456C2F分别读取完整W32模式，不能共用入口快照。
        const std::size_t modeIndex = static_cast<std::size_t>(context->field138);
        const std::uint32_t tailToken = gGraphicCache4Queue[modeIndex];
        const std::uint32_t currentToken = initialCountBits + 1U;
        if (tailToken == 0) {
            gGraphicCache4Queue[modeIndex] = currentToken;
            const std::size_t layerModeIndex = static_cast<std::size_t>(context->field138);
            static_cast<void>(QueueText_521A30(currentToken, gTextFontLayerRecords[layerModeIndex].layer));
        } else {
            gTextPendingRecords[static_cast<std::size_t>(tailToken - 1U)].nextToken = currentToken;
            const std::size_t tailModeIndex = static_cast<std::size_t>(context->field138);
            gGraphicCache4Queue[tailModeIndex] = platform::gPendingGraphicCount2 + 1U;
        }
        ++platform::gPendingGraphicCount2;
    }
    if (input::gCurrentInputData != context) {
        return;
    }
    if (context->maxCharPerLine > 0 && context->maxLine >= 2) {
        context->x = context->field130 + Font::measure(contents.c_str() + caretLineStart,
            context->field13C, static_cast<std::int32_t>(caret - caretLineStart));
        context->y = context->field134 + caretLineYOffset;
        return;
    }
    std::size_t visibleStart = input::ContextUtf16Offset(*context, context->field158);
    if (caret < visibleStart) {
        visibleStart = caret;
        input::SetVisibleUtf16Offset(*context, visibleStart);
    }
    const std::wstring_view visible(contents.data() + visibleStart, caret - visibleStart);
    const std::wstring displayed = context->encrypt >= 1 ? std::wstring(text::Count(visible), '*') : std::wstring(visible);
    context->x = context->field130 + Font::measure(displayed.c_str(), context->field13C, static_cast<std::int32_t>(displayed.size()));
    context->y = context->field134;
}

} // namespace cg::render
