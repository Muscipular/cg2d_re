#include "cg/LoginInput.hpp"

#include "LoginInputDependencies.hpp"
#include "LoginMessageDependencies.hpp"

#include "cg/GraphicInfo.hpp"
#include "cg/Palette.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/render/GraphicPendingQueue.hpp"

#include <bit>
#include <cstddef>

namespace cg::login {

input::InputContext gInputContext9A4D20 {}; // 原全局：0x009A4D20。
input::InputContext gInputContext9A5960 {}; // 原全局：0x009A5960。
input::InputContext gInputContext9A5678 {}; // 原全局：0x009A5678。

std::int16_t gInputContext9A5960X = 0; // 原全局：0x009A4B8C。
std::int16_t gInputContext9A5960Y = 0; // 原全局：0x009A4B8A。
std::int16_t gInputContext9A5678X = 0; // 原全局：0x009A4B9E。
std::int16_t gInputContext9A5678Y = 0; // 原全局：0x009A4B9C。
std::int32_t gValue9A6E20 = 0; // 原全局：0x009A6E20。
std::uint32_t gValue9A6200 = 0; // 原全局：0x009A6200。

input::InputContext* gInputContextFocusTable[3] { // 原全局：0x00559FFC。
    &gInputContext9A5960,
    &gInputContext9A5678,
    &gInputContext9A5960,
};

namespace {

inline constexpr std::size_t kStoredInputValueSize = 51;

struct Capability4SavedSlotGeometry final {
    std::int32_t x;
    std::int32_t y;
    std::int32_t width;
    std::int32_t height;
};

static_assert(offsetof(Capability4SavedSlotGeometry, x) == 0x00);
static_assert(offsetof(Capability4SavedSlotGeometry, y) == 0x04);
static_assert(offsetof(Capability4SavedSlotGeometry, width) == 0x08);
static_assert(offsetof(Capability4SavedSlotGeometry, height) == 0x0C);
static_assert(sizeof(Capability4SavedSlotGeometry) == 0x10);

// 原 unk_55C4E0：能力 4 的十个持久槽命中/绘制几何。
inline constexpr Capability4SavedSlotGeometry
    kCapability4SavedSlotGeometry[10] {
        {80, 335, 44, 20},
        {124, 335, 44, 20},
        {168, 335, 44, 20},
        {212, 335, 44, 20},
        {256, 335, 44, 20},
        {300, 335, 44, 20},
        {344, 335, 44, 20},
        {388, 335, 44, 20},
        {432, 335, 44, 20},
        {476, 335, 44, 20},
    };

std::int32_t AddW32(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return left + right;
}

void DrawCapability4SavedSlotGraphic(
    const std::int32_t slotIndex,
    const std::int32_t offsetX,
    const std::int32_t offsetY,
    const std::uint8_t layer,
    const std::int32_t imageNo)
{
    const std::int32_t drawX = AddW32(
        kCapability4SavedSlotGeometry[slotIndex].x,
        offsetX);
    const std::int32_t drawY = AddW32(
        kCapability4SavedSlotGeometry[slotIndex].y,
        offsetY);
    static_cast<void>(render::Sub_45F090(
        drawX,
        drawY,
        layer,
        imageNo,
        0,
        -1));
}

char* WriteStoredInputValue(
    char* const source,
    char* const storedValue)
{
    if (source == nullptr) {
        return nullptr;
    }

    for (std::size_t index = 0; index < kStoredInputValueSize; ++index) {
        char* const cursor = source + index;
        const char value = *cursor;
        storedValue[index] = value;
        if (value == '\0') {
            return cursor;
        }
    }
    return source + kStoredInputValueSize;
}

char* ReadStoredInputValue(
    char* const output,
    const char* const storedValue)
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

// 原函数：0x005203B0。
char* WriteInputValueFF494F_5203B0(char* const source)
{
    return WriteStoredInputValue(
        source,
        detail::GetCurrentSavedInputValueFF494F());
}

// 原函数：0x005203E0。
char* ReadInputValueFF494F_5203E0(char* const output)
{
    if (output == nullptr) {
        return nullptr;
    }
    return ReadStoredInputValue(
        output,
        detail::GetCurrentSavedInputValueFF494F());
}

// 原函数：0x00520410。
char* WriteInputValueFF4982_520410(char* const source)
{
    return WriteStoredInputValue(
        source,
        detail::GetCurrentSavedInputValueFF4982());
}

// 原函数：0x00520440。
char* ReadInputValueFF4982_520440(char* const output)
{
    if (output == nullptr) {
        return nullptr;
    }
    return ReadStoredInputValue(
        output,
        detail::GetCurrentSavedInputValueFF4982());
}

// 原函数：0x005036F0。
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
    const std::int32_t bottom = height + top;
    const std::int32_t right = width + left;
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

// 原函数：0x00503780。
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
        const std::int32_t bottom = height + top;
        const std::int32_t width = control->width;
        const std::int32_t right = width + left;
        if (detail::Sub_5049A0(left, top, right, bottom, -1) != 0) {
            imageNo = control->activeImageNo;
        } else {
            imageNo = control->normalImageNo;
        }
    }

    const std::int32_t drawY = control->drawY;
    const std::int32_t drawX = control->drawX;
    detail::DrawGraphic(drawX, drawY, layer, imageNo, 1, &descriptor);

    const std::int32_t sampledState = state;
    if (sampledState == 0) {
        return;
    }

    const std::int32_t hitTop = control->top;
    const std::int32_t hitHeight = control->height;
    const std::int32_t hitLeft = control->left;
    const std::int32_t hitBottom = hitHeight + hitTop;
    const std::int32_t hitWidth = control->width;
    const std::int32_t hitRight = hitWidth + hitLeft;
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
    const std::int32_t overlayTop = control->top;
    const std::int32_t overlayLeft = control->left;
    const std::int32_t overlayHeight = control->height;
    const std::int32_t overlayBottom = overlayHeight + overlayTop + 2;
    const std::int32_t overlayDrawTop = overlayTop - 2;
    const std::int32_t overlayWidth = control->width;
    const std::int32_t overlayRight = overlayWidth + overlayLeft + 2;
    const std::int32_t overlayDrawLeft = overlayLeft - 2;
    detail::DrawGraphicHover(
        static_cast<std::uint16_t>(overlayDrawLeft),
        static_cast<std::uint16_t>(overlayDrawTop),
        static_cast<std::uint16_t>(overlayRight),
        static_cast<std::uint16_t>(overlayBottom),
        layer,
        overlayImageNo,
        step);
}

// 原函数：0x0045C7C0。
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

// 原函数：0x0045F970。
void InitializeCapabilityInput_45F970()
{
    char firstInitialText[256] {};
    char secondInitialText[256] {};

    render::gPaletteCapability =
        resource::ProbePaletteCapability_45C970();

    gInputContext9A5960X = 257;
    gInputContext9A5960Y = 358;
    gInputContext9A5960.text[0] = '\0';
    gInputContext9A5960.totalByteLength = 0;
    gInputContext9A5960.caretByteOffset = 0;
    ReadInputValueFF4982_520440(firstInitialText);
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
    gInputContext9A5960.encrypt = 0;

    gInputContext9A5678X = 257;
    gInputContext9A5678Y = 389;
    gInputContext9A5678.text[0] = '\0';
    gInputContext9A5678.totalByteLength = 0;
    gInputContext9A5678.caretByteOffset = 0;
    ReadInputValueFF494F_5203E0(secondInitialText);
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
    gInputContext9A5678.encrypt = 1;

    gValue9A6E20 = 0;
    input::SetContext_44B330(gInputContextFocusTable[0]);
    gValue9A6200 = 255;
}

// 原函数：0x00465320。
void InitializeCapability4Input_465320()
{
    char initialText[256] {};

    render::gPaletteCapability =
        resource::ProbePaletteCapability_45C970();

    const std::int32_t firstSelectedSlot =
        detail::gSelectedSavedSlot105C394;
    gInputContext9A5678X = 187;
    gInputContext9A5678Y = 358;
    gInputContext9A5678.text[0] = '\0';
    gInputContext9A5678.totalByteLength = 0;
    gInputContext9A5678.caretByteOffset = 0;
    detail::Sub_533680(initialText, firstSelectedSlot);
    const std::int32_t firstY = gInputContext9A5678Y;
    const std::int32_t firstX = gInputContext9A5678X;
    input::InitializeContext_449CF0(
        &gInputContext9A5678,
        firstX + 2,
        firstY + 2,
        0,
        0,
        3,
        initialText,
        1,
        23,
        0,
        0);

    const std::int32_t secondSelectedSlot =
        detail::gSelectedSavedSlot105C394;
    gInputContext9A5960X = 187;
    gInputContext9A5960Y = 389;
    gInputContext9A5960.text[0] = '\0';
    gInputContext9A5960.totalByteLength = 0;
    gInputContext9A5960.caretByteOffset = 0;
    detail::Sub_5335F0(initialText, secondSelectedSlot);
    const std::int32_t secondY = gInputContext9A5960Y;
    const std::int32_t secondX = gInputContext9A5960X;
    input::InitializeContext_449CF0(
        &gInputContext9A5960,
        secondX + 2,
        secondY + 2,
        0,
        0,
        3,
        initialText,
        1,
        20,
        0,
        1);

    input::InputContext* const initialContext =
        gInputContextFocusTable[1];
    gValue9A6E20 = 1;
    input::SetContext_44B330(initialContext);
    gValue9A6200 = 255;
}

// 原函数：0x00465740。
void SaveAndSwitchCapability4InputSlot_465740(
    const std::int32_t newSlot)
{
    const std::int32_t firstSavedSlot =
        detail::gSelectedSavedSlot105C394;
    detail::Sub_533570(gInputContext9A4D20.text, firstSavedSlot);

    const std::int32_t secondSavedSlot =
        detail::gSelectedSavedSlot105C394;
    detail::Sub_5335B0(gInputContext9A5960.text, secondSavedSlot);

    const std::int32_t thirdSavedSlot =
        detail::gSelectedSavedSlot105C394;
    detail::Sub_533640(gInputContext9A5678.text, thirdSavedSlot);

    detail::Sub_533730();
    detail::gSelectedSavedSlot105C394 = newSlot;
    InitializeCapability4Input_465320();
}

// 原函数：0x00465430。
std::int32_t ProcessCapability4SavedSlotSelector_465430()
{
    std::int32_t visibleCount =
        detail::gVisibleSavedSlotCount105C364;
    std::int32_t hoveredSlot = -1;
    std::int32_t slotIndex = 0;
    if (visibleCount <= 0) {
        return visibleCount;
    }

    do {
        const Capability4SavedSlotGeometry& geometry =
            kCapability4SavedSlotGeometry[slotIndex];
        if (static_cast<std::uint8_t>(
                platform::legacy_crypt_int::IntLessCryptRaw_401FA0(
                    geometry.x, &platform::gLegacyMouseState.mouseX)) != 0
            && platform::legacy_crypt_int::Less_401DB0(
                &platform::gLegacyMouseState.mouseX,
                AddW32(geometry.x, geometry.width))
            && static_cast<std::uint8_t>(
                platform::legacy_crypt_int::IntLessCryptRaw_401FA0(
                    geometry.y, &platform::gLegacyMouseState.mouseY)) != 0
            && platform::legacy_crypt_int::Less_401DB0(
                &platform::gLegacyMouseState.mouseY,
                AddW32(geometry.y, geometry.height))) {
            const std::uint8_t click =
                static_cast<std::uint8_t>(detail::gValueD68D9C);
            hoveredSlot = slotIndex;
            if ((click & 0x01U) != 0) {
                detail::PlaySe(51, 320, 240);
                SaveAndSwitchCapability4InputSlot_465740(slotIndex);
            }
        }

        visibleCount = detail::gVisibleSavedSlotCount105C364;
        slotIndex = AddW32(slotIndex, 1);
    } while (slotIndex < visibleCount);

    if (visibleCount <= 0) {
        return visibleCount;
    }

    slotIndex = 0;
    do {
        const std::int32_t selectedSlot =
            detail::gSelectedSavedSlot105C394;
        std::int32_t digitBase = 245540;
        std::int32_t frameImageNo = 246678;
        std::uint8_t frameLayer = 206;
        if (slotIndex == selectedSlot) {
            frameImageNo = 246677;
            frameLayer = 207;
        } else if (slotIndex == hoveredSlot) {
            digitBase = 245560;
            frameImageNo = 246679;
        }

        const std::int32_t oneBased = AddW32(slotIndex, 1);
        if (oneBased < 10) {
            DrawCapability4SavedSlotGraphic(
                slotIndex,
                18,
                3,
                207,
                AddW32(digitBase, oneBased));
        } else {
            const std::int32_t tens = oneBased / 10;
            DrawCapability4SavedSlotGraphic(
                slotIndex,
                15,
                3,
                207,
                AddW32(digitBase, tens));
            const std::int32_t ones = oneBased % 10;
            DrawCapability4SavedSlotGraphic(
                slotIndex,
                21,
                3,
                207,
                AddW32(digitBase, ones));
        }
        DrawCapability4SavedSlotGraphic(
            slotIndex,
            0,
            0,
            frameLayer,
            frameImageNo);

        visibleCount = detail::gVisibleSavedSlotCount105C364;
        slotIndex = AddW32(slotIndex, 1);
    } while (slotIndex < visibleCount);

    return visibleCount;
}

} // namespace cg::login
