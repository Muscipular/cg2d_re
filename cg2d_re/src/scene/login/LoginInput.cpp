#include "cg/session/ClientSession.hpp"
#include "LoginSavedState.hpp"
#include "cg/ui/HitTest.hpp"
#include "cg/scene/login/LoginInput.hpp"
#include "cg/text/Encoding.hpp"

#include "LoginInputDependencies.hpp"
#include "LoginSceneState.hpp"
#include "LoginMessageDependencies.hpp"
#include "../../ui/UiControlRecord.hpp"

#include "cg/resource/GraphicInfo.hpp"
#include "cg/network/CgConnection.hpp"
#include "cg/render/Palette.hpp"
#include "cg/platform/LegacyMouse.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/render/GraphicPendingQueue.hpp"

#include <array>
#include <bit>
#include <cstring>
#include <cwchar>
#include <cstddef>

#include "cg/core/m_log.hpp"

namespace cg::login {

static inline constexpr std::size_t kStoredInputValueSize = LoginInputValueCapacity;

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
static inline constexpr Capability4SavedSlotGeometry
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

static void DrawCapability4SavedSlotGraphic(session::ClientSession& session,
    const std::int32_t slotIndex,
    const std::int32_t offsetX,
    const std::int32_t offsetY,
    const std::uint8_t layer,
    const std::int32_t imageNo)
{
    const std::int32_t drawX = kCapability4SavedSlotGeometry[slotIndex].x + offsetX;
    const std::int32_t drawY = kCapability4SavedSlotGeometry[slotIndex].y + offsetY;
    static_cast<void>(cg::render::QueueOriginalPaletteGraphic_45F090(session,
        drawX,
        drawY,
        layer,
        imageNo,
        0,
        -1));
}

static const wchar_t* WriteStoredInputValue(
    const wchar_t* const source,
    std::wstring& storedValue)
{
    if (source == nullptr) {
        return nullptr;
    }

    char bytes[kStoredInputValueSize]{};
    cg::text::StoreGbk(bytes, sizeof(bytes), source);
    storedValue = source;
    return source + std::wcslen(source);
}

static std::wstring& ReadStoredInputValue(std::wstring& output, const std::wstring& storedValue)
{
    output = storedValue;
    return output;
}

// 原0x005203B0/0x00520410：两项完整编码后再按原顺序发布，失败不改存档。
bool SaveInputValues(
    session::ClientSession& session, const std::wstring_view first, const std::wstring_view second)
{
    std::array<char, kStoredInputValueSize> firstStored;
    std::array<char, kStoredInputValueSize> secondStored;
    try {
        cg::text::StoreGbk(firstStored.data(), firstStored.size(), first);
        cg::text::StoreGbk(secondStored.data(), secondStored.size(), second);
    } catch (const cg::text::GbkStorageError&) {
        m_log_iw(L"文字无法保存为GBK，或超过登录字段容量。");
        return false;
    }
    detail::GetCurrentSavedInputValueFF494F(session) = first;
    detail::GetCurrentSavedInputValueFF4982(session) = second;
    return true;
}

// 原0x00533570/0x005335B0/0x00533640：三个51字节字段先全组校验，再发布。
bool SaveCapability4InputSlot(network::CgConnection& connection)
{
    auto& sceneUi = connection.LoginSceneUi();
    std::array<std::array<char, kStoredInputValueSize>, 3> stored;
    try {
        cg::text::StoreGbk(stored[0].data(), stored[0].size(), input::ContextText(sceneUi.context9A4D20));
        cg::text::StoreGbk(stored[1].data(), stored[1].size(), input::ContextText(sceneUi.context9A5960));
        cg::text::StoreGbk(stored[2].data(), stored[2].size(), input::ContextText(sceneUi.context9A5678));
    } catch (const cg::text::GbkStorageError&) {
        m_log_iw(L"文字无法保存为GBK，或超过登录字段容量。");
        return false;
    }
    detail::Sub_533570(connection.Owner(), input::ContextText(sceneUi.context9A4D20), connection.Owner().SavedLogin().selectedSlot);
    detail::WriteSavedPassword_5335B0(connection.Owner(), input::ContextText(sceneUi.context9A5960), connection.Owner().SavedLogin().selectedSlot);
    detail::WriteSavedAccount_533640(connection.Owner(), input::ContextText(sceneUi.context9A5678), connection.Owner().SavedLogin().selectedSlot);
    return true;
}

// 原函数：0x005203B0。
const wchar_t* WriteInputValueFF494F_5203B0(session::ClientSession& session, const wchar_t* const source)
{
    return WriteStoredInputValue(
        source,
        detail::GetCurrentSavedInputValueFF494F(session));
}

// 原函数：0x005203E0。
std::wstring& ReadInputValueFF494F_5203E0(session::ClientSession& session, std::wstring& output)
{
    return ReadStoredInputValue(output, detail::GetCurrentSavedInputValueFF494F(session));
}

// 原函数：0x00520410。
const wchar_t* WriteInputValueFF4982_520410(session::ClientSession& session, const wchar_t* const source)
{
    return WriteStoredInputValue(
        source,
        detail::GetCurrentSavedInputValueFF4982(session));
}

// 原函数：0x00520440。
std::wstring& ReadInputValueFF4982_520440(session::ClientSession& session, std::wstring& output)
{
    return ReadStoredInputValue(output, detail::GetCurrentSavedInputValueFF4982(session));
}

// 原函数：0x005036F0。
std::int32_t QueryLoginControlState_5036F0(
    session::ClientSession& session,
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
    if (cg::ui::HitTestMouseRect_5049A0(session, left, top, right, bottom, -1) == 0) {
        return 0;
    }

    const std::uint8_t edge =
        static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(session));
    std::uint32_t result = 0x40U;
    if ((edge & 0x01U) != 0) {
        result |= 0x01U;
    }
    if ((edge & 0x02U) != 0) {
        result |= 0x04U;
    }

    const std::uint8_t held =
        static_cast<std::uint8_t>(platform::ReadSessionMouseHeldButtons(session));
    if ((held & 0x01U) != 0) {
        result |= 0x02U;
    }
    if ((held & 0x02U) != 0) {
        result |= 0x08U;
    }

    const std::uint8_t pulse =
        static_cast<std::uint8_t>(platform::ReadSessionMousePulseButtons(session));
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
    session::ClientSession& session,
    const LoginSceneControl32* const control,
    const std::uint8_t layer,
    const std::int32_t state,
    const std::uint32_t overlayImageNo,
    const std::int32_t renderMode)
{
    const render::GraphicDescriptor16 descriptor {
        0, 0, 0, 0, 0xFFFFFFFFu, 0, cg::render::GraphicMixFlag_None, 0,
    };
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
        if (cg::ui::HitTestMouseRect_5049A0(session, left, top, right, bottom, -1) != 0) {
            imageNo = control->activeImageNo;
        } else {
            imageNo = control->normalImageNo;
        }
    }

    const std::int32_t drawY = control->drawY;
    const std::int32_t drawX = control->drawX;
    cg::render::DrawGraphic(session, drawX, drawY, layer, imageNo, 1, &descriptor);

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
    if (cg::ui::HitTestMouseRect_5049A0(session,
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
    cg::render::DrawGraphicHover(session,
        static_cast<std::uint16_t>(overlayDrawLeft),
        static_cast<std::uint16_t>(overlayDrawTop),
        static_cast<std::uint16_t>(overlayRight),
        static_cast<std::uint16_t>(overlayBottom),
        layer,
        overlayImageNo,
        step);
}

// 原函数：0x0045C7C0。
void InitializeInputContexts_45C7C0(network::CgConnection& connection)
{
    auto& sceneUi = connection.LoginSceneUi();
    std::wstring firstInitialText;
    std::wstring secondInitialText;

    const std::int32_t capability =
        resource::ProbePaletteCapability_45C970();
    const bool useHighLayout = capability >= 1;
    render::gPaletteCapability = capability;

    if (useHighLayout) {
        sceneUi.context9A5960X = 114;
        sceneUi.context9A5960Y = 341;
        sceneUi.context9A5960.clearText();
        sceneUi.context9A5960.totalByteLength = 0;
        sceneUi.context9A5960.caretByteOffset = 0;
        ReadInputValueFF494F_5203E0(connection.Owner(), firstInitialText);
        input::InitializeContext_449CF0(
            connection.Owner(),
            &sceneUi.context9A5960,
            sceneUi.context9A5960X,
            sceneUi.context9A5960Y,
            0,
            0,
            3,
            firstInitialText,
            1,
            20,
            0,
            1);
        sceneUi.context9A5960.encrypt = 1;

        sceneUi.context9A5678X = 114;
        sceneUi.context9A5678Y = 309;
        sceneUi.context9A5678.clearText();
        sceneUi.context9A5678.totalByteLength = 0;
        sceneUi.context9A5678.caretByteOffset = 0;
        ReadInputValueFF4982_520440(connection.Owner(), secondInitialText);
        input::InitializeContext_449CF0(
            connection.Owner(),
            &sceneUi.context9A5678,
            sceneUi.context9A5678X,
            sceneUi.context9A5678Y,
            0,
            0,
            3,
            secondInitialText,
            1,
            20,
            0,
            0);
    } else {
        sceneUi.context9A5960X = 335;
        sceneUi.context9A5960Y = 300;
        sceneUi.context9A5960.clearText();
        sceneUi.context9A5960.totalByteLength = 0;
        sceneUi.context9A5960.caretByteOffset = 0;
        ReadInputValueFF494F_5203E0(connection.Owner(), firstInitialText);
        input::InitializeContext_449CF0(
            connection.Owner(),
            &sceneUi.context9A5960,
            sceneUi.context9A5960X,
            sceneUi.context9A5960Y,
            0,
            0,
            3,
            firstInitialText,
            1,
            20,
            0,
            0);
        sceneUi.context9A5960.encrypt = 1;

        sceneUi.context9A5678X = 335;
        sceneUi.context9A5678Y = 260;
        sceneUi.context9A5678.clearText();
        sceneUi.context9A5678.totalByteLength = 0;
        sceneUi.context9A5678.caretByteOffset = 0;
        ReadInputValueFF4982_520440(connection.Owner(), secondInitialText);
        input::InitializeContext_449CF0(
            connection.Owner(),
            &sceneUi.context9A5678,
            sceneUi.context9A5678X,
            sceneUi.context9A5678Y,
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
        sceneUi.inputFocusTable[1];
    sceneUi.inputFocusIndex = 1;
    input::SetContext_44B330(connection.Owner(), initialContext);
}

// 原函数：0x0045F970。
void InitializeCapabilityInput_45F970(network::CgConnection& connection)
{
    auto& sceneUi = connection.LoginSceneUi();
    std::wstring firstInitialText;
    std::wstring secondInitialText;

    render::gPaletteCapability =
        resource::ProbePaletteCapability_45C970();

    sceneUi.context9A5960X = 257;
    sceneUi.context9A5960Y = 358;
    sceneUi.context9A5960.clearText();
    sceneUi.context9A5960.totalByteLength = 0;
    sceneUi.context9A5960.caretByteOffset = 0;
    ReadInputValueFF4982_520440(connection.Owner(), firstInitialText);
    input::InitializeContext_449CF0(
        connection.Owner(),
        &sceneUi.context9A5960,
        sceneUi.context9A5960X,
        sceneUi.context9A5960Y,
        0,
        0,
        3,
        firstInitialText,
        1,
        20,
        0,
        1);
    sceneUi.context9A5960.encrypt = 0;

    sceneUi.context9A5678X = 257;
    sceneUi.context9A5678Y = 389;
    sceneUi.context9A5678.clearText();
    sceneUi.context9A5678.totalByteLength = 0;
    sceneUi.context9A5678.caretByteOffset = 0;
    ReadInputValueFF494F_5203E0(connection.Owner(), secondInitialText);
    input::InitializeContext_449CF0(
        connection.Owner(),
        &sceneUi.context9A5678,
        sceneUi.context9A5678X,
        sceneUi.context9A5678Y,
        0,
        0,
        3,
        secondInitialText,
        1,
        20,
        0,
        0);
    sceneUi.context9A5678.encrypt = 1;

    sceneUi.inputFocusIndex = 0;
    input::SetContext_44B330(connection.Owner(), sceneUi.inputFocusTable[0]);
    sceneUi.capability3OverlayAlpha = 255;
}

// 原函数：0x00465320。
void InitializeCapability4Input_465320(network::CgConnection& connection)
{
    auto& sceneUi = connection.LoginSceneUi();
    std::wstring initialText;

    render::gPaletteCapability =
        resource::ProbePaletteCapability_45C970();

    const std::int32_t firstSelectedSlot =
        connection.Owner().SavedLogin().selectedSlot;
    sceneUi.context9A5678X = 187;
    sceneUi.context9A5678Y = 358;
    sceneUi.context9A5678.clearText();
    sceneUi.context9A5678.totalByteLength = 0;
    sceneUi.context9A5678.caretByteOffset = 0;
    detail::ReadSavedAccount_533680(connection.Owner(), initialText, firstSelectedSlot);
    const std::int32_t firstY = sceneUi.context9A5678Y;
    const std::int32_t firstX = sceneUi.context9A5678X;
    input::InitializeContext_449CF0(
        connection.Owner(),
        &sceneUi.context9A5678,
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
        connection.Owner().SavedLogin().selectedSlot;
    sceneUi.context9A5960X = 187;
    sceneUi.context9A5960Y = 389;
    sceneUi.context9A5960.clearText();
    sceneUi.context9A5960.totalByteLength = 0;
    sceneUi.context9A5960.caretByteOffset = 0;
    detail::ReadSavedPassword_5335F0(connection.Owner(), initialText, secondSelectedSlot);
    const std::int32_t secondY = sceneUi.context9A5960Y;
    const std::int32_t secondX = sceneUi.context9A5960X;
    input::InitializeContext_449CF0(
        connection.Owner(),
        &sceneUi.context9A5960,
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
        sceneUi.inputFocusTable[1];
    sceneUi.inputFocusIndex = 1;
    input::SetContext_44B330(connection.Owner(), initialContext);
    sceneUi.capability3OverlayAlpha = 255;
}

// 原函数：0x00465740。
void SaveAndSwitchCapability4InputSlot_465740(
    network::CgConnection& connection, const std::int32_t newSlot)
{
    if (!SaveCapability4InputSlot(connection)) {
        return;
    }

    detail::SaveCredentialRecordsWithoutPasswords_533730(connection.Owner());
    connection.Owner().SavedLogin().selectedSlot = newSlot;
    InitializeCapability4Input_465320(connection);
}

// 原函数：0x00465430。
std::int32_t ProcessCapability4SavedSlotSelector_465430(network::CgConnection& connection)
{
    std::int32_t visibleCount =
        connection.Owner().SavedLogin().visibleSlotCount;
    std::int32_t hoveredSlot = -1;
    std::int32_t slotIndex = 0;
    if (visibleCount <= 0) {
        return visibleCount;
    }

    const std::int32_t offsetX = ui::LegacyUiLayoutScope::GetOffsetX();
    const std::int32_t offsetY = ui::LegacyUiLayoutScope::GetOffsetY();
    do {
        const Capability4SavedSlotGeometry& geometry =
            kCapability4SavedSlotGeometry[slotIndex];
        if ((geometry.x + offsetX < connection.Owner().UiFrame().mouse.state->mouseX)
            && (connection.Owner().UiFrame().mouse.state->mouseX < geometry.x + offsetX + geometry.width)
            && (geometry.y + offsetY < connection.Owner().UiFrame().mouse.state->mouseY)
            && (connection.Owner().UiFrame().mouse.state->mouseY < geometry.y + offsetY + geometry.height)) {
            const std::uint8_t click =
                static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(connection.Owner()));
            hoveredSlot = slotIndex;
            if ((click & 0x01U) != 0) {
                detail::PlaySe(connection.Owner(), 51, 320, 240);
                SaveAndSwitchCapability4InputSlot_465740(connection, slotIndex);
            }
        }

        visibleCount = connection.Owner().SavedLogin().visibleSlotCount;
        slotIndex = slotIndex + 1;
    } while (slotIndex < visibleCount);

    if (visibleCount <= 0) {
        return visibleCount;
    }

    slotIndex = 0;
    do {
        const std::int32_t selectedSlot =
            connection.Owner().SavedLogin().selectedSlot;
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

        const std::int32_t oneBased = slotIndex + 1;
        if (oneBased < 10) {
            DrawCapability4SavedSlotGraphic(connection.Owner(),
                slotIndex,
                18,
                3,
                207,
                digitBase + oneBased);
        } else {
            const std::int32_t tens = oneBased / 10;
            DrawCapability4SavedSlotGraphic(connection.Owner(),
                slotIndex,
                15,
                3,
                207,
                digitBase + tens);
            const std::int32_t ones = oneBased % 10;
            DrawCapability4SavedSlotGraphic(connection.Owner(),
                slotIndex,
                21,
                3,
                207,
                digitBase + ones);
        }
        DrawCapability4SavedSlotGraphic(connection.Owner(),
            slotIndex,
            0,
            0,
            frameLayer,
            frameImageNo);

        visibleCount = connection.Owner().SavedLogin().visibleSlotCount;
        slotIndex = slotIndex + 1;
    } while (slotIndex < visibleCount);

    return visibleCount;
}

} // namespace cg::login
