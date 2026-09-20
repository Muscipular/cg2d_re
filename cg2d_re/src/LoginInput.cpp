#include "cg/LoginInput.hpp"

#include "LoginMessageDependencies.hpp"

#include "cg/GraphicInfo.hpp"
#include "cg/Palette.hpp"

#include <bit>
#include <cstddef>

namespace cg::login {

input::InputContext gInputContext9A5960 {};
input::InputContext gInputContext9A5678 {};

std::int16_t gInputContext9A5960X = 0;
std::int16_t gInputContext9A5960Y = 0;
std::int16_t gInputContext9A5678X = 0;
std::int16_t gInputContext9A5678Y = 0;
std::int32_t gValue9A6E20 = 0;

input::InputContext* gInputContextFocusTable[3] {
    &gInputContext9A5960,
    &gInputContext9A5678,
    &gInputContext9A5960,
};

namespace {

inline constexpr std::size_t kStoredInputValueSize = 51;

char gValueFF494F[kStoredInputValueSize] {};
char gValueFF4982[kStoredInputValueSize] {};

char* ReadStoredInputValue(
    char* const output,
    const char (&storedValue)[kStoredInputValueSize])
{
    for (std::size_t index = 0; index < kStoredInputValueSize; ++index) {
        const char value = storedValue[index];
        output[index] = value;
        if (value == '\0') {
            return output;
        }
    }
    return output;
}

} // namespace

char* ReadInputValueFF494F_5203E0(char* const output)
{
    if (output == nullptr) {
        return nullptr;
    }
    return ReadStoredInputValue(output, gValueFF494F);
}

char* ReadInputValueFF4982_520440(char* const output)
{
    if (output == nullptr) {
        return nullptr;
    }
    return ReadStoredInputValue(output, gValueFF4982);
}

std::int32_t QueryLoginControlState_5036F0(
    const LoginSceneControl32* const control)
{
    if (control == nullptr) {
        return 0;
    }

    const std::int32_t top = control->top;
    const std::int32_t left = control->left;
    const std::int32_t height = control->height;
    const std::int32_t width = control->width;
    const std::int32_t bottom = std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(top)
        + std::bit_cast<std::uint32_t>(height));
    const std::int32_t right = std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left)
        + std::bit_cast<std::uint32_t>(width));
    if (detail::Sub_5049A0(left, top, right, bottom, -1) == 0) {
        return 0;
    }

    const std::uint8_t edge =
        static_cast<std::uint8_t>(detail::gValueD68D9C);
    std::uint32_t result = 0x40U;
    if ((edge & 0x01U) != 0) {
        result |= 0x01U;
    }
    if ((edge & 0x02U) != 0) {
        result |= 0x04U;
    }

    const std::uint8_t held =
        static_cast<std::uint8_t>(detail::gValueD68D98);
    if ((held & 0x01U) != 0) {
        result |= 0x02U;
    }
    if ((held & 0x02U) != 0) {
        result |= 0x08U;
    }

    const std::uint8_t pulse =
        static_cast<std::uint8_t>(detail::gValueD68DA0);
    if ((pulse & 0x01U) != 0) {
        result |= 0x10U;
    }
    if ((pulse & 0x02U) != 0) {
        result |= 0x20U;
    }
    if ((edge & 0x10U) != 0) {
        result |= 0x80U;
    }
    if ((edge & 0x20U) != 0) {
        result |= 0x100U;
    }
    return static_cast<std::int32_t>(result);
}

void DrawLoginControl_503780(
    const LoginSceneControl32* const control,
    const std::uint8_t layer,
    const std::int32_t state,
    const std::uint32_t overlayImageNo,
    const std::int32_t renderMode)
{
    const render::GraphicDescriptor16 descriptor {{
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0,
        0xFF,
        0xFF,
        0xFF,
        0xFF,
        0,
        0,
        0,
        0,
    }};
    if (control == nullptr) {
        return;
    }

    std::int32_t imageNo = 0;
    const std::int32_t baseRenderMode = renderMode;
    if (baseRenderMode == 1) {
        imageNo = control->normalImageNo;
    } else if (baseRenderMode == 2) {
        imageNo = control->activeImageNo;
    } else {
        const std::int32_t top = control->top;
        const std::int32_t height = control->height;
        const std::int32_t left = control->left;
        const std::int32_t width = control->width;
        const std::int32_t bottom = std::bit_cast<std::int32_t>(
            std::bit_cast<std::uint32_t>(top)
            + std::bit_cast<std::uint32_t>(height));
        const std::int32_t right = std::bit_cast<std::int32_t>(
            std::bit_cast<std::uint32_t>(left)
            + std::bit_cast<std::uint32_t>(width));
        if (detail::Sub_5049A0(left, top, right, bottom, -1) != 0) {
            imageNo = control->activeImageNo;
        } else {
            imageNo = control->normalImageNo;
        }
    }

    const std::int32_t drawX = control->drawX;
    const std::int32_t drawY = control->drawY;
    detail::DrawGraphic(drawX, drawY, layer, imageNo, 1, &descriptor);

    const std::int32_t sampledState = state;
    if (sampledState == 0) {
        return;
    }

    const std::int32_t hitTop = control->top;
    const std::int32_t hitHeight = control->height;
    const std::int32_t hitLeft = control->left;
    const std::int32_t hitWidth = control->width;
    const std::int32_t hitBottom = std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(hitTop)
        + std::bit_cast<std::uint32_t>(hitHeight));
    const std::int32_t hitRight = std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(hitLeft)
        + std::bit_cast<std::uint32_t>(hitWidth));
    if (detail::Sub_5049A0(
            hitLeft,
            hitTop,
            hitRight,
            hitBottom,
            -1)
        == 0) {
        return;
    }

    const std::int32_t overlayRenderMode = renderMode;
    if (overlayRenderMode != 0) {
        return;
    }

    const std::int32_t step = sampledState == 2 ? 1 : 0;
    const std::uint32_t overlayLeft =
        std::bit_cast<std::uint32_t>(control->left);
    const std::uint32_t overlayTop =
        std::bit_cast<std::uint32_t>(control->top);
    const std::uint32_t overlayWidth =
        std::bit_cast<std::uint32_t>(control->width);
    const std::uint32_t overlayHeight =
        std::bit_cast<std::uint32_t>(control->height);
    detail::DrawGraphicHover(
        static_cast<std::uint16_t>(overlayLeft - 2U),
        static_cast<std::uint16_t>(overlayTop - 2U),
        static_cast<std::uint16_t>(overlayLeft + overlayWidth + 2U),
        static_cast<std::uint16_t>(overlayTop + overlayHeight + 2U),
        layer,
        overlayImageNo,
        step);
}

void InitializeInputContexts_45C7C0()
{
    char firstInitialText[256] {};
    char secondInitialText[256] {};

    const std::int32_t capability =
        resource::ProbePaletteCapability_45C970();
    const bool useHighLayout = capability >= 1;
    render::gPaletteCapability = capability;

    if (useHighLayout) {
        gInputContext9A5960X = 114;
        gInputContext9A5960Y = 341;
        gInputContext9A5960.text[0] = '\0';
        gInputContext9A5960.totalByteLength = 0;
        gInputContext9A5960.caretByteOffset = 0;
        ReadInputValueFF494F_5203E0(firstInitialText);
        input::InitializeContext_449CF0(
            &gInputContext9A5960,
            gInputContext9A5960X,
            gInputContext9A5960Y,
            0,
            0,
            3,
            firstInitialText,
            1,
            20,
            0,
            1);
        gInputContext9A5960.encrypt = 1;

        gInputContext9A5678X = 114;
        gInputContext9A5678Y = 309;
        gInputContext9A5678.text[0] = '\0';
        gInputContext9A5678.totalByteLength = 0;
        gInputContext9A5678.caretByteOffset = 0;
        ReadInputValueFF4982_520440(secondInitialText);
        input::InitializeContext_449CF0(
            &gInputContext9A5678,
            gInputContext9A5678X,
            gInputContext9A5678Y,
            0,
            0,
            3,
            secondInitialText,
            1,
            20,
            0,
            0);
    } else {
        gInputContext9A5960X = 335;
        gInputContext9A5960Y = 300;
        gInputContext9A5960.text[0] = '\0';
        gInputContext9A5960.totalByteLength = 0;
        gInputContext9A5960.caretByteOffset = 0;
        ReadInputValueFF494F_5203E0(firstInitialText);
        input::InitializeContext_449CF0(
            &gInputContext9A5960,
            gInputContext9A5960X,
            gInputContext9A5960Y,
            0,
            0,
            3,
            firstInitialText,
            1,
            20,
            0,
            0);
        gInputContext9A5960.encrypt = 1;

        gInputContext9A5678X = 335;
        gInputContext9A5678Y = 260;
        gInputContext9A5678.text[0] = '\0';
        gInputContext9A5678.totalByteLength = 0;
        gInputContext9A5678.caretByteOffset = 0;
        ReadInputValueFF4982_520440(secondInitialText);
        input::InitializeContext_449CF0(
            &gInputContext9A5678,
            gInputContext9A5678X,
            gInputContext9A5678Y,
            0,
            0,
            3,
            secondInitialText,
            1,
            23,
            0,
            0);
    }

    input::InputContext* const initialContext =
        gInputContextFocusTable[1];
    gValue9A6E20 = 1;
    input::SetContext_44B330(initialContext);
}

} // namespace cg::login
