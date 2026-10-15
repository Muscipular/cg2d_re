#include "LoginCapability4Modal.hpp"
#include <cwchar>

#include "LoginMessageDependencies.hpp"
#include "cg/platform/LegacyTime.hpp"

#include <bit>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <cstring> // 原CRT接口：0x0053C2F0；原参数／返回合同由直接调用承接。

namespace {

constexpr std::int32_t kScreenWidth = 640;
constexpr std::int32_t kScreenHeight = 480;

// 原 dword_9A57E4@0x9A57E4，PE 虚拟零填充。
std::int32_t gCapability4ModalInitialized = 0;
// 原 dword_55C580@0x55C580，文件初值与每次初始化值均为 -2。
std::int32_t gCapability4ModalSentinel = -2;
// 原 0x9A4EA8/0x9A5448/0x9A4B78/0x9A4E7C，PE 虚拟零填充。
std::int32_t gCapability4ModalWidth = 0; // 原全局：0x009A4EA8。
std::int32_t gCapability4ModalHeight = 0; // 原全局：0x009A5448。
std::int32_t gCapability4ModalX = 0; // 原全局：0x009A4B78。
std::int32_t gCapability4ModalY = 0; // 原全局：0x009A4E7C。
// 原 dword_9A6EBC@0x9A6EBC；原生指针宽度观察者，不放入 W32。
cg::login::YobiAction* gCapability4ModalAction = nullptr;

constexpr cg::login::detail::GraphicDescriptor16 kGraphicDescriptor {
    0, 0, 0, 0, 0xFFFFFFFFu, 0, cg::render::GraphicMixFlag_OriginalPalette, 0,
};

std::int32_t AddW32(
    const std::int32_t left,
    const std::int32_t right)
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left)
        + std::bit_cast<std::uint32_t>(right));
}

std::int32_t SubtractW32(
    const std::int32_t left,
    const std::int32_t right)
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left)
        - std::bit_cast<std::uint32_t>(right));
}

std::int32_t MultiplyW32(
    const std::int32_t left,
    const std::int32_t right)
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left)
        * std::bit_cast<std::uint32_t>(right));
}

std::int16_t Low16(const std::int32_t value)
{
    return std::bit_cast<std::int16_t>(
        static_cast<std::uint16_t>(std::bit_cast<std::uint32_t>(value)));
}

} // namespace

namespace cg::login::detail {

// 原函数：0x00464440。
void ResetCapability4Modal_464440()
{
    gCapability4ModalInitialized = 0;
}

// 原函数：0x00464450。
std::int32_t UpdateCapability4Modal_464450(const std::wstring_view text)
{
    std::wstring splitText(text);

    std::int32_t lineCount = 1;
    std::int32_t maximumTextWidth = 0;
    wchar_t* currentLine = splitText.data();
    for (;;) {
        wchar_t* const newline = std::wcschr(currentLine, L'\n');
        if (newline == nullptr) {
            const std::int32_t width = TextMeasure(currentLine, 0);
            if (width > maximumTextWidth) {
                maximumTextWidth = width;
            }
            break;
        }

        *newline = L'\0';
        const std::int32_t width = TextMeasure(currentLine, 0);
        if (width > maximumTextWidth) {
            maximumTextWidth = width;
        }
        currentLine = newline + 1;
        lineCount = AddW32(lineCount, 1);
    }

    if (gCapability4ModalInitialized == 0) {
        gCapability4ModalInitialized = 1;
        gCapability4ModalSentinel = -2;

        const std::int32_t roundedWidth = MultiplyW32(
            AddW32(maximumTextWidth, 95) / 64,
            64);
        const std::int32_t lineStep = AddW32(gValue558F78, 2);
        const std::int32_t lineOffset = MultiplyW32(
            SubtractW32(lineCount, 1),
            lineStep);
        const std::int32_t modalHeight = AddW32(lineOffset, 96);
        const std::int32_t modalX =
            SubtractW32(kScreenWidth, roundedWidth) / 2;
        const std::int32_t modalY =
            SubtractW32(kScreenHeight, modalHeight) / 2;

        gCapability4ModalWidth = roundedWidth;
        gCapability4ModalHeight = modalHeight;
        gCapability4ModalX = modalX;
        gCapability4ModalY = modalY;
        gCapability4ModalAction = CreateModalAction_5044E0(
            modalX,
            modalY,
            roundedWidth,
            modalHeight,
            4,
            1);
    }

    const std::int32_t lastLineIndex = SubtractW32(lineCount, 1);

    const std::int32_t leftHitLineStep = AddW32(gValue558F78, 2);
    const std::int32_t leftHitBaseY = AddW32(
        gCapability4ModalY,
        MultiplyW32(lastLineIndex, leftHitLineStep));
    const std::int32_t leftHitWidth = gCapability4ModalWidth;
    const std::int32_t leftHitX = gCapability4ModalX;
    const std::int32_t leftHitCenter = AddW32(
        leftHitX,
        leftHitWidth / 3);
    const std::int32_t leftHitLeft = SubtractW32(leftHitCenter, 33);
    const std::int32_t leftHitTop = AddW32(leftHitBaseY, 61);
    const std::int32_t leftHitRight = AddW32(leftHitCenter, 33);
    const std::int32_t leftHitBottom = AddW32(leftHitBaseY, 78);
    const bool leftHovered = Sub_5049A0(
        leftHitLeft,
        leftHitTop,
        leftHitRight,
        leftHitBottom,
        -1) != 0;

    const std::int32_t rightHitLineStep = AddW32(gValue558F78, 2);
    const std::int32_t rightHitBaseY = AddW32(
        gCapability4ModalY,
        MultiplyW32(lastLineIndex, rightHitLineStep));
    const std::int32_t rightHitWidth = gCapability4ModalWidth;
    const std::int32_t rightHitX = gCapability4ModalX;
    const std::int32_t rightHitCenter = AddW32(
        rightHitX,
        MultiplyW32(rightHitWidth, 2) / 3);
    const std::int32_t rightHitLeft = SubtractW32(rightHitCenter, 33);
    const std::int32_t rightHitTop = AddW32(rightHitBaseY, 61);
    const std::int32_t rightHitRight = AddW32(rightHitCenter, 33);
    const std::int32_t rightHitBottom = AddW32(rightHitBaseY, 78);
    const bool rightHovered = Sub_5049A0(
        rightHitLeft,
        rightHitTop,
        rightHitRight,
        rightHitBottom,
        -1) != 0;

    const std::uint8_t mouseClickState =
        static_cast<std::uint8_t>(gValueD68D9C);
    const bool clicked = (mouseClickState & 1U) != 0;
    std::int32_t selection = -1;
    if (leftHovered && clicked) {
        selection = 0;
    }
    if (rightHovered && clicked) {
        selection = 1;
    }

    if (selection != -1) {
        const std::int32_t result = AddW32(selection, 1);
        PlaySe(79, 320, 240);
        YobiAction* const action = gCapability4ModalAction;
        if (action != nullptr) {
            MarkDeath(action);
            gCapability4ModalAction = nullptr;
        }
        return result;
    }

    YobiAction* const action = gCapability4ModalAction;
    if (action == nullptr || !IsYobiActionReady(action, 1)) {
        return 0;
    }

    const std::int32_t textHorizontalOffset =
        SubtractW32(gCapability4ModalWidth, maximumTextWidth) / 2;
    wchar_t* drawLine = splitText.data();
    std::int32_t lineIndex = 0;
    while (lineIndex < lineCount) {
        const std::int32_t textLineStep = AddW32(gValue558F78, 2);
        const std::int32_t textY = AddW32(
            AddW32(
                gCapability4ModalY,
                MultiplyW32(lineIndex, textLineStep)),
            30);
        const std::int32_t textX = AddW32(
            gCapability4ModalX,
            textHorizontalOffset);
        DrawText(
            Low16(textX),
            Low16(textY),
            2,
            0,
            0,
            drawLine,
            0,
            0);
        drawLine += std::wcslen(drawLine) + 1U;
        lineIndex = AddW32(lineIndex, 1);
    }

    const std::int32_t leftGraphicLineStep = AddW32(gValue558F78, 2);
    const std::int32_t leftGraphicY = AddW32(
        AddW32(
            gCapability4ModalY,
            MultiplyW32(lastLineIndex, leftGraphicLineStep)),
        69);
    const std::int32_t leftGraphicWidth = gCapability4ModalWidth;
    const std::int32_t leftGraphicX = AddW32(
        gCapability4ModalX,
        leftGraphicWidth / 3);
    DrawGraphic(
        leftGraphicX,
        leftGraphicY,
        223,
        leftHovered ? 243050 : 243048,
        0,
        &kGraphicDescriptor);

    const std::int32_t rightGraphicYBase = gCapability4ModalY;
    const std::int32_t rightGraphicLineStep = AddW32(gValue558F78, 2);
    const std::int32_t rightGraphicY = AddW32(
        AddW32(
            rightGraphicYBase,
            MultiplyW32(lastLineIndex, rightGraphicLineStep)),
        69);
    const std::int32_t rightGraphicXBase = gCapability4ModalX;
    const std::int32_t rightGraphicWidth = gCapability4ModalWidth;
    const std::int32_t rightGraphicX = AddW32(
        rightGraphicXBase,
        MultiplyW32(rightGraphicWidth, 2) / 3);
    DrawGraphic(
        rightGraphicX,
        rightGraphicY,
        223,
        rightHovered ? 245328 : 245326,
        0,
        &kGraphicDescriptor);
    return 0;
}

} // namespace cg::login::detail
