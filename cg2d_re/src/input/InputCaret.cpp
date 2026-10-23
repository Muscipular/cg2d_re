#include "../ui/WindowRuntimeState.hpp"
#include "../platform/imeImpl.hpp"
#include "../platform/WindowProcedureState.hpp"

#include "cg/input/Input.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/TextPendingQueue.hpp"

#include <cstddef>
#include <cstdint>

namespace cg::input {

// 原 CP950 A2 78「│」按用户要求改用单字节 ASCII「|」，绘制按单字节宽度居中。
static inline constexpr wchar_t kCaretText[] = L"|";

static void DrawBasicCaret()
{
    input::InputContext* const context = input::gCurrentInputData;
    const std::int32_t color = context->color;
    const std::int32_t fontType = render::ResolveFontId(context->fontType);
    const std::int8_t renderMode = static_cast<std::int8_t>(
        static_cast<std::uint8_t>(context->renderMode) + 1U);
    const std::int32_t y = context->y;
    const std::int32_t x = context->x;
    const std::int32_t halfCaretWidth =
        render::GetFontMetrics(fontType).sWidth
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

static void DrawImeCaret()
{
    input::InputContext* const preMeasureContext =
        input::gCurrentInputData;
    const std::int32_t color = preMeasureContext->color;
    const std::int32_t fontType = render::ResolveFontId(preMeasureContext->fontType);
    const std::int8_t renderMode = static_cast<std::int8_t>(
        static_cast<std::uint8_t>(preMeasureContext->renderMode) + 1U);
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
        postMeasureContext->fontType;
    const std::int32_t postMeasureX = postMeasureContext->x;
    const std::int32_t halfCaretWidth =
        render::GetFontMetrics(postMeasureFontType).sWidth
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

static void AdvanceCaretPhase()
{
    const std::int32_t nextPhase = input::gInputCaretBlinkPhase_626C0C + 1;
    const bool resetPhase = nextPhase >= 40;
    input::gInputCaretBlinkPhase_626C0C = nextPhase;
    if (resetPhase) {
        input::gInputCaretBlinkPhase_626C0C = 0;
    }
}

// 原 sub_44A410@[0x44A410,0x44A535)。
void DrawAndAdvanceInputCaret_44A410()
{
    input::InputContext* const initialContext = input::gCurrentInputData;
    if (initialContext == nullptr) {
        return;
    }
    if (initialContext == &cg::ui::detail::gInputContext626AA0
        && input::gChatPanelVisible_C9A96C == 0) {
        return;
    }

    if (input::gInputCaretBlinkPhase_626C0C >= 20) {
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

} // namespace cg::input
