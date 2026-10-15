#include "../ui/WindowRuntimeState.hpp"
#include "../platform/ImeLifecycle.hpp"
#include "../platform/WindowProcedureState.hpp"

#include "cg/input/Input.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/TextPendingQueue.hpp"

#include <cstddef>
#include <cstdint>

namespace cg::world::detail {
namespace {

// 原 CP950 A2 78「│」按用户要求改用单字节 ASCII「|」，绘制按单字节宽度居中。
inline constexpr wchar_t kCaretText[] = L"|";

void DrawBasicCaret()
{
    input::InputContext* const context = input::gCurrentInputData;
    const std::int32_t color = context->field140;
    const std::int32_t fontType = context->field13C;
    const std::int8_t renderMode = static_cast<std::int8_t>(
        static_cast<std::uint8_t>(context->field138) + 1U);
    const std::int32_t y = context->y;
    const std::int32_t x = context->x;
    const std::int32_t halfCaretWidth =
        render::gFontMetrics[static_cast<std::size_t>(fontType)].sWidth
        / 2;
    const std::int32_t drawX = x - halfCaretWidth;
    static_cast<void>(render::DrawText2_456670(
        static_cast<std::int16_t>(drawX),
        static_cast<std::int16_t>(y),
        renderMode,
        static_cast<std::int8_t>(fontType),
        static_cast<std::int8_t>(color),
        kCaretText,
        0));
}

void DrawImeCaret()
{
    input::InputContext* const preMeasureContext =
        input::gCurrentInputData;
    const std::int32_t color = preMeasureContext->field140;
    const std::int32_t fontType = preMeasureContext->field13C;
    const std::int8_t renderMode = static_cast<std::int8_t>(
        static_cast<std::uint8_t>(preMeasureContext->field138) + 1U);
    const std::int32_t y = preMeasureContext->y;
    const std::int32_t cursorPosition =
        platform::gImeCompositionCursorPosition;

    const std::int32_t measuredWidth = render::Font::measure(
        platform::gImeComposition,
        fontType,
        cursorPosition);

    input::InputContext* const postMeasureContext =
        input::gCurrentInputData;
    const std::int32_t postMeasureFontType =
        postMeasureContext->field13C;
    const std::int32_t postMeasureX = postMeasureContext->x;
    const std::int32_t halfCaretWidth =
        render::gFontMetrics[
            static_cast<std::size_t>(postMeasureFontType)].sWidth
        / 2;
    const std::int32_t drawX = measuredWidth + (postMeasureX - halfCaretWidth);
    static_cast<void>(render::DrawText2_456670(
        static_cast<std::int16_t>(drawX),
        static_cast<std::int16_t>(y),
        renderMode,
        static_cast<std::int8_t>(fontType),
        static_cast<std::int8_t>(color),
        kCaretText,
        0));
}

void AdvanceCaretPhase()
{
    const std::int32_t nextPhase = input::gValue626C0C + 1;
    const bool resetPhase = nextPhase >= 40;
    input::gValue626C0C = nextPhase;
    if (resetPhase) {
        input::gValue626C0C = 0;
    }
}

} // namespace

// 原 sub_44A410@[0x44A410,0x44A535)。
void Sub_44A410()
{
    input::InputContext* const initialContext = input::gCurrentInputData;
    if (initialContext == nullptr) {
        return;
    }
    if (initialContext == &cg::ui::detail::gInputContext626AA0
        && input::gValueC9A96C == 0) {
        return;
    }

    if (input::gValue626C0C >= 20) {
        const std::int32_t isIme = platform::IsImeKeyboardLayout_459090();
        if (isIme == 0) {
            DrawBasicCaret();
        } else {
            const wchar_t firstCompositionByte =
                platform::gImeComposition[0];
            if (firstCompositionByte == '\0') {
                DrawBasicCaret();
            } else {
                const std::uint8_t firstAttribute =
                    platform::gImeAttributes.empty() ? 0 : platform::gImeAttributes[0];
                if (firstAttribute == 0) {
                    DrawImeCaret();
                }
            }
        }
    }

    AdvanceCaretPhase();
}

} // namespace cg::world::detail
