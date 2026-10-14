#include "platform/ImeLifecycle.hpp"
#include "platform/WindowProcedureState.hpp"

#include "cg/Input.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/TextPendingQueue.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>

namespace cg::world::detail {
namespace {

// 原 CP950 A2 78「│」按用户要求改用单字节 ASCII「|」，绘制按单字节宽度居中。
inline constexpr wchar_t kCaretText[] = L"|";

std::int32_t AddW32(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return left + right;
}

std::int32_t SubW32(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return left - right;
}

std::int16_t LowS16(const std::int32_t value) noexcept
{
    return std::bit_cast<std::int16_t>(static_cast<std::uint16_t>(
        std::bit_cast<std::uint32_t>(value)));
}

std::int8_t LowS8(const std::int32_t value) noexcept
{
    return std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(
        std::bit_cast<std::uint32_t>(value)));
}

std::int8_t IncrementModeW8(const std::uint8_t mode) noexcept
{
    return std::bit_cast<std::int8_t>(
        static_cast<std::uint8_t>(mode + 1U));
}

void DrawBasicCaret()
{
    input::InputContext* const context = input::gCurrentInputData;
    const std::int32_t color = context->field140;
    const std::int32_t fontType = context->field13C;
    const std::int8_t renderMode = IncrementModeW8(context->field138[0]);
    const std::int32_t y = context->y;
    const std::int32_t x = context->x;
    const std::int32_t halfCaretWidth =
        render::gFontMetrics[static_cast<std::size_t>(fontType)].sWidth
        / 2;
    const std::int32_t drawX = SubW32(x, halfCaretWidth);
    static_cast<void>(render::DrawText2_456670(
        LowS16(drawX),
        LowS16(y),
        renderMode,
        LowS8(fontType),
        LowS8(color),
        kCaretText,
        0));
}

void DrawImeCaret()
{
    input::InputContext* const preMeasureContext =
        input::gCurrentInputData;
    const std::int32_t color = preMeasureContext->field140;
    const std::int32_t fontType = preMeasureContext->field13C;
    const std::int8_t renderMode = IncrementModeW8(
        preMeasureContext->field138[0]);
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
    const std::int32_t drawX = AddW32(
        measuredWidth,
        SubW32(postMeasureX, halfCaretWidth));
    static_cast<void>(render::DrawText2_456670(
        LowS16(drawX),
        LowS16(y),
        renderMode,
        LowS8(fontType),
        LowS8(color),
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
    if (initialContext == &input::gInputContext626AA0
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
