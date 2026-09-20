#include "platform/StartupNative.hpp"
#include "platform/WindowProcedureState.hpp"

#include "cg/Input.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/TextPendingQueue.hpp"

#include <imm.h>

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>

namespace cg::world::detail {
namespace {

inline constexpr std::array<char, 3> kCaretText {{
    std::bit_cast<char>(std::uint8_t {0xA2}),
    std::bit_cast<char>(std::uint8_t {0x78}),
    '\0',
}};

std::int32_t AddW32(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left)
        + std::bit_cast<std::uint32_t>(right));
}

std::int32_t SubW32(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left)
        - std::bit_cast<std::uint32_t>(right));
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

bool IsValidFontType(const std::int32_t fontType) noexcept
{
    return fontType >= 0
        && fontType < static_cast<std::int32_t>(render::kFontTypeCount);
}

bool IsImeKeyboardLayout_459090() noexcept
{
    const HKL keyboardLayout = GetKeyboardLayout(0);
    return ImmIsIME(keyboardLayout) != FALSE;
}

bool CanMeasureImeComposition(const std::int32_t maxBytes) noexcept
{
    if (maxBytes <= 0) {
        return true;
    }

    constexpr std::size_t capacity =
        sizeof(platform::gImeComposition);
    std::size_t cursor = 0;
    std::uint32_t consumed = 0;
    for (;;) {
        if (cursor >= capacity) {
            return false;
        }

        const std::uint8_t firstByte = static_cast<std::uint8_t>(
            static_cast<unsigned char>(platform::gImeComposition[cursor]));
        if (firstByte == 0) {
            return true;
        }
        if ((firstByte & 0x80U) != 0U) {
            if (cursor + 2U > capacity) {
                return false;
            }
            cursor += 2U;
            consumed += 2U;
        } else {
            ++cursor;
            ++consumed;
        }

        if (std::bit_cast<std::int32_t>(consumed) >= maxBytes) {
            return true;
        }
    }
}

void DrawBasicCaret()
{
    input::InputContext* const context = input::gCurrentInputData;
    if (context == nullptr) {
        return;
    }

    const std::int32_t color = context->field140;
    const std::int32_t fontType = context->field13C;
    const std::int8_t renderMode = IncrementModeW8(context->field138[0]);
    const std::int32_t y = context->y;
    const std::int32_t x = context->x;
    if (!IsValidFontType(fontType)) {
        return;
    }

    const std::int32_t halfDoubleWidth =
        render::gFontMetrics[static_cast<std::size_t>(fontType)].doubleWidth
        / 2;
    const std::int32_t drawX = SubW32(x, halfDoubleWidth);
    static_cast<void>(render::DrawText2_456670(
        LowS16(drawX),
        LowS16(y),
        renderMode,
        LowS8(fontType),
        LowS8(color),
        kCaretText.data(),
        0));
}

void DrawImeCaret()
{
    input::InputContext* const preMeasureContext =
        input::gCurrentInputData;
    if (preMeasureContext == nullptr) {
        return;
    }

    const std::int32_t color = preMeasureContext->field140;
    const std::int32_t fontType = preMeasureContext->field13C;
    const std::int8_t renderMode = IncrementModeW8(
        preMeasureContext->field138[0]);
    const std::int32_t y = preMeasureContext->y;
    const std::int32_t cursorPosition =
        platform::gImeCompositionCursorPosition;

    std::int32_t measuredWidth = 0;
    if (cursorPosition > 0 && IsValidFontType(fontType)) {
        if (!CanMeasureImeComposition(cursorPosition)) {
            return;
        }
        measuredWidth = render::Font::measure(
            platform::gImeComposition,
            fontType,
            cursorPosition);
    }

    input::InputContext* const postMeasureContext =
        input::gCurrentInputData;
    if (postMeasureContext == nullptr) {
        return;
    }
    const std::int32_t postMeasureFontType =
        postMeasureContext->field13C;
    const std::int32_t postMeasureX = postMeasureContext->x;
    if (!IsValidFontType(postMeasureFontType)) {
        return;
    }

    const std::int32_t halfDoubleWidth =
        render::gFontMetrics[
            static_cast<std::size_t>(postMeasureFontType)].doubleWidth
        / 2;
    const std::int32_t drawX = AddW32(
        measuredWidth,
        SubW32(postMeasureX, halfDoubleWidth));
    static_cast<void>(render::DrawText2_456670(
        LowS16(drawX),
        LowS16(y),
        renderMode,
        LowS8(fontType),
        LowS8(color),
        kCaretText.data(),
        0));
}

void AdvanceCaretPhase()
{
    const std::uint32_t phaseBits = std::bit_cast<std::uint32_t>(
        input::gValue626C0C);
    input::gValue626C0C = std::bit_cast<std::int32_t>(phaseBits + 1U);
    if (input::gValue626C0C >= 40) {
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
        const bool isIme = IsImeKeyboardLayout_459090();
        if (!isIme) {
            DrawBasicCaret();
        } else {
            const char firstCompositionByte =
                platform::gImeComposition[0];
            if (firstCompositionByte == '\0') {
                DrawBasicCaret();
            } else {
                const std::uint8_t firstAttribute =
                    platform::gImeAttributes[0];
                if (firstAttribute == 0) {
                    DrawImeCaret();
                }
            }
        }
    }

    AdvanceCaretPhase();
}

} // namespace cg::world::detail
