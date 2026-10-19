#include "cg/core/LegacyNoOp.hpp"
#include "../scene/login/LoginSavedState.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/ui/HitTest.hpp"
#include "cg/world/SceneState.hpp"
#include "cg/text/Encoding.hpp"

#include "../scene/character_select/CharacterSelectSceneDependencies.hpp"
#include "../scene/login/LoginMessageDependencies.hpp"
#include "../platform/imeImpl.hpp"
#include "../ui/UiControlRecord.hpp"

#include "cg/scene/character_select/CharacterSelectScene.hpp"
#include "cg/resource/GraphicInfo.hpp"
#include "cg/input/Input.hpp"
#include "cg/scene/login/LoginConnection.hpp"
#include "cg/scene/login/LoginScene.hpp"
#include "cg/scene/login/LoginState.hpp"
#include "cg/render/Palette.hpp"
#include "cg/Yobi/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/ui/WindowRuntime.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "AlbumState.hpp"
#include "BattleState.hpp"
#include "ChatHistory.hpp"
#include "MailState.hpp"
#include "PlayerState.hpp"
#include "WorldMapState.hpp"
#include "WorldObjectState.hpp"
#include "WorldUpdateDependencies.hpp"

#include <bit>
#include <cstdio> // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。

namespace cg::world {

// 原 [0x51C430,0x51C726)，188 条指令、13 项主分发表和 5 项次分发表。
void Update()
{
    const std::int32_t pendingSceneType = gPendingSceneType;
    if (pendingSceneType >= 0) {
        gCurrentSceneType = pendingSceneType;
        const std::int32_t pendingSceneState = gPendingSceneState;
        gPendingSceneType = -1;
        gCurrentSceneState = pendingSceneState;
    }

    const std::int32_t sceneType = gCurrentSceneType;
    gValueC82204 = 0;
    switch (sceneType) {
    case 1: {
        const ui::LegacyUiLayoutScope layout;
        platform::gValue73ECC0 = 0;
        input::ProcessKeyInput_4596D0();
        const std::int32_t capability = render::gPaletteCapability;
        switch (capability) {
        case 0:
        case 1:
            login::ProcessModeScene_45C5A0();
            break;
        case 2:
            login::ProcessOuterScene_45C2D0();
            break;
        case 3:
            Sub_45F190();
            break;
        case 4:
            Sub_464840();
            break;
        default:
            break;
        }
        cg::yobi::YobiUpdateAll_404C50();
        detail::RenderChara_521F20();
        detail::RenderInput_4590E0();
        cg::input::DrawAndAdvanceInputCaret_44A410();
        return;
    }
    case 2: {
        const ui::LegacyUiLayoutScope layout;
        platform::gValue73ECC0 = 0;
        login::ProcessScene_45FEA0();
        cg::yobi::YobiUpdateAll_404C50();
        detail::RenderChara_521F20();
        return;
    }
    case 3: {
        const ui::LegacyUiLayoutScope layout;
        platform::gValue73ECC0 = 0;
        character_select::ProcessScene_460FF0();
        cg::yobi::YobiUpdateAll_404C50();
        detail::RenderChara_521F20();
        return;
    }
    case 4:
        platform::gValue73ECC0 = 0;
        input::ProcessKeyInput_4596D0();
        character_select::detail::ProcessCreationScene_4626A0(1);
        cg::yobi::YobiUpdateAll_404C50();
        detail::RenderChara_521F20();
        detail::RenderInput_4590E0();
        cg::input::DrawAndAdvanceInputCaret_44A410();
        return;
    case 5:
        platform::gValue73ECC0 = 0;
        static_cast<void>(platform::ResetImeOpenStatus_4590A0());
        detail::Sub_449DA0();
        detail::Sub_46A9E0();
        detail::Sub_467F30();
        detail::Sub_5199A0();
        detail::Sub_446470();
        detail::Sub_4D3170();
        detail::Sub_455BD0();
        detail::Sub_46E2A0();
        Sub_46BD40();
        gValue5ABFC4 = 0;
        gValue9A6E0C = 0;
        detail::Sub_504A30();
        detail::Sub_43C8F0();
        detail::ResetSceneUiState_504100();
        detail::Sub_518C80();
        detail::Sub_518CB0();
        gValueFF0D5C = 0xFFFFFFFFu;
        gValueFF0D58 = 0;
        gValueD67E78 = 0;
        cg::core::NullSub_509080();
        detail::Sub_518780();
        ui::WindowRuntime_ProcessScene();
        cg::yobi::Sub_43F220();
        gValueFF0D74 = 0;
        gValue9DAD30 = 0;
        SetCurrentSceneType(6);
        cg::audio::BeginBgmFadeOut_52B990();
        gValueD67B08.fill(0);
        return;
    case 6:
        platform::gValue73ECC0 = 0;
        login::ProcessState_45E960();
        goto sceneSixTail;
    case 7:
        detail::Sub_45EBB0();
        cg::yobi::YobiUpdateAll_404C50();
        return;
    case 8:
        return;
    case 9:
        detail::DrawNormalScene_51C7D0();
        return;
    case 10:
        detail::DrawBattleScene_43CBB0();
        return;
    case 11: {
        const std::int32_t sceneState = gCurrentSceneState;
        if (sceneState == 0) {
            detail::Sub_519D50();
            detail::Sub_446470();
            cg::yobi::YobiClearUp_404D30();
            ui::WindowRuntime_ProcessScene();
            cg::yobi::Sub_43F220();
            gValue58B6D0 = 1;
            detail::Sub_51D750();
            platform::gValue73ECC0 = 2;
            cg::audio::BeginBgmFadeOut_52B990();
            const std::uint64_t tick = platform::ReadTickCount64();
            platform::gLastRenderDt = static_cast<double>(tick);
            ++gCurrentSceneState;
        } else if (sceneState == 1
            && detail::Sub_51FDE0(2, 0, 0x3F19999Au) == 1) {
            platform::gValue73ECC0 = 0;
            ++gCurrentSceneState;
        }

        if (detail::Sub_455C90() == 0) {
            goto sceneSixTail;
        }
        cg::login::detail::Sub_520160();
        detail::Sub_519D50();
        detail::Sub_446470();
        cg::network::ShutdownNetworkRuntime_5081A0();
        const std::int32_t capability = resource::ProbePaletteCapability_45C970();
        const std::int32_t paletteIndex = capability == 3 ? 27 : 0;
        render::gPaletteCapability = capability;
        render::SelectPalette(paletteIndex, 0);
        SetCurrentSceneType(2);
        cg::yobi::YobiClearUp_404D30();
        gValueD9F2CA = 0;
        return;
    }
    case 12:
        return;
    case 13:
        platform::gValue73ECC0 = 0;
        input::ProcessKeyInput_4596D0();
        if (render::gPaletteCapability < 3) {
            character_select::ProcessLegacyVersionScene_45E3D0();
        } else {
            character_select::ProcessScene_462330();
        }
        cg::yobi::YobiUpdateAll_404C50();
        detail::RenderChara_521F20();
        detail::RenderInput_4590E0();
        cg::input::DrawAndAdvanceInputCaret_44A410();
        return;
    default:
        return;
    }

// 原函数：0x00404C50。
sceneSixTail:
    cg::yobi::YobiUpdateAll_404C50();
    detail::RenderChara_521F20();
}

namespace detail {

std::wstring gDisconnectText9A4D10; // 原全局：0x009A4D10。

static cg::yobi::YobiAction* gDisconnectAction62B5B4 = nullptr; // 原全局：0x0062B5B4。
static std::int32_t gDisconnectState62B3A8 = 0; // 原全局：0x0062B3A8。
static std::int32_t gDisconnectWidth62B32C = 0; // 原全局：0x0062B32C。
static std::int32_t gDisconnectHeight62B304 = 0; // 原全局：0x0062B304。
static std::int32_t gDisconnectX62B390 = 0; // 原全局：0x0062B390。
static std::int32_t gDisconnectY62B2EC = 0; // 原全局：0x0062B2EC。

// 原函数：0x00455C90。
std::int32_t Sub_455C90()
{
    cg::render::GraphicDescriptor16 descriptor {
        0, 0, 0, 0, 0xFFFFFFFFu, 0, cg::render::GraphicMixFlag_OriginalPalette, 0,
    };
    std::int32_t clicked = 0;
    bool hovered = false;
    cg::yobi::YobiAction* action = gDisconnectAction62B5B4;
    if (action == nullptr) {
        gDisconnectState62B3A8 = -2;
        // 模态背景按64×48瓦片取整，布局使用实际尺寸，统一背景、文字及命中位置。
        gDisconnectWidth62B32C = 384;
        gDisconnectHeight62B304 = 144;
        gDisconnectX62B390 = (platform::gWindowWidth - gDisconnectWidth62B32C) / 2;
        gDisconnectY62B2EC = (platform::gWindowHeight - gDisconnectHeight62B304) / 2;
        action = cg::yobi::CreateModalAction_5044E0(
            gDisconnectX62B390, gDisconnectY62B2EC,
            gDisconnectWidth62B32C, gDisconnectHeight62B304, 4, 1);
        gDisconnectAction62B5B4 = action;
        if (action == nullptr) {
            return 0;
        }
    }
    if (!cg::yobi::IsYobiActionReady(action, 1)) {
        return 0;
    }

    const std::int32_t scaledHitHeight = gDisconnectHeight62B304 * 4;
    const std::int32_t hitWidth = gDisconnectWidth62B32C;
    const std::int32_t hitWidthDelta = hitWidth - 66;
    const std::int32_t hitHeightStep = scaledHitHeight / 5;
    const std::int32_t hitY = gDisconnectY62B2EC;
    const std::int32_t hitBaseY = hitY + hitHeightStep;
    const std::int32_t hitX = gDisconnectX62B390;
    const std::int32_t left = (hitWidthDelta >> 1) + hitX;
    const std::int32_t bottom = hitBaseY + 18;
    const std::int32_t top = hitBaseY + 1;
    const std::int32_t right = left + 66;
    hovered = cg::ui::HitTestMouseRect_5049A0(
        left, top, right, bottom, -1) != 0;
    if (hovered
        && (static_cast<std::uint8_t>(cg::platform::gLegacyMouseState.edgeButtons) & 1U) != 0) {
        cg::yobi::MarkDeath(gDisconnectAction62B5B4);
        gDisconnectAction62B5B4 = nullptr;
        clicked = 1;
    }

    const std::int32_t row = gDisconnectHeight62B304 / 5;
    std::int32_t width = cg::render::TextMeasure(gDisconnectText9A4D10, 2);
    const std::int32_t firstWindowWidth = gDisconnectWidth62B32C;
    const std::int32_t firstWindowY = gDisconnectY62B2EC;
    const std::int32_t firstWidthDelta = firstWindowWidth - width;
    const std::int32_t firstTextY = firstWindowY + row - 4;
    const std::int32_t firstWindowX = gDisconnectX62B390;
    const std::int32_t firstTextX = firstWindowX + firstWidthDelta / 2;
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(firstTextX),
        static_cast<std::int16_t>(firstTextY),
        2, 2, 0, gDisconnectText9A4D10, 0, 0));

    std::wstring_view secondText;
    std::wstring_view thirdText;
    std::wstring errorText;
    const bool connectionError = login::gValue593FC0 != 0;
    if (connectionError) {
        cg::text::FormatTo(errorText, L"伺服器连线中断({})", network::ReadNetworkErrorD9F2CC());
        secondText = errorText;
        thirdText = L"请尝试其他的伺服器。";
    } else {
        secondText = L"与伺服器的连线被切断了。";
        thirdText = L"回到伺服器选择画面。";
    }

    width = cg::render::TextMeasure(secondText, 0);
    const std::int32_t secondWindowWidth = gDisconnectWidth62B32C;
    std::int32_t secondWidthDelta;
    std::int32_t secondWindowY;
    if (connectionError) {
        secondWidthDelta = secondWindowWidth - width;
        secondWindowY = gDisconnectY62B2EC;
    } else {
        secondWindowY = gDisconnectY62B2EC;
        secondWidthDelta = secondWindowWidth - width;
    }
    const std::int32_t secondTextY = secondWindowY + 2 * row - 4;
    const std::int32_t secondWindowX = gDisconnectX62B390;
    const std::int32_t secondTextX = secondWindowX + secondWidthDelta / 2;
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(secondTextX),
        static_cast<std::int16_t>(secondTextY),
        2, 0, 0, secondText, 0, 0));

    width = cg::render::TextMeasure(thirdText, 0);
    const std::int32_t thirdWindowWidth = gDisconnectWidth62B32C;
    std::int32_t thirdTextX;
    std::int32_t thirdTextY;
    if (connectionError) {
        const std::int32_t thirdWidthDelta = thirdWindowWidth - width;
        const std::int32_t thirdWindowY = gDisconnectY62B2EC;
        const std::int32_t thirdHalfWidth = thirdWidthDelta / 2;
        const std::int32_t thirdBaseY = thirdWindowY + 2 * row;
        thirdTextY = thirdBaseY + row - 4;
        const std::int32_t thirdWindowX = gDisconnectX62B390;
        thirdTextX = thirdWindowX + thirdHalfWidth;
    } else {
        const std::int32_t thirdWindowY = gDisconnectY62B2EC;
        const std::int32_t thirdWidthDelta = thirdWindowWidth - width;
        const std::int32_t thirdBaseY = thirdWindowY + 2 * row;
        thirdTextY = thirdBaseY + row - 4;
        const std::int32_t thirdWindowX = gDisconnectX62B390;
        thirdTextX = thirdWindowX + thirdWidthDelta / 2;
    }
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(thirdTextX),
        static_cast<std::int16_t>(thirdTextY),
        2, 0, 0, thirdText, 0, 0));

    const std::int32_t imageWindowY = gDisconnectY62B2EC;
    const std::int32_t imageWindowWidth = gDisconnectWidth62B32C;
    const std::int32_t imageNo = hovered ? 243050 : 243048;
    const std::int32_t imageY = imageWindowY + 4 * row + 9;
    const std::int32_t imageWindowX = gDisconnectX62B390;
    const std::int32_t imageX = (imageWindowWidth >> 1) + imageWindowX;
    static_cast<void>(cg::render::DrawGraphic(
        imageX, imageY, 0xDF, imageNo, 0, &descriptor));
    return clicked;
}

// 原 sub_51FDE0@[0x51FDE0,0x5200D0)：按 W32 selector 分派并透传目标返回。
std::int32_t Sub_51FDE0(
    std::int32_t selector, std::int32_t value, std::uint32_t payloadBits)
{
    switch (selector) {
    case 0: return Sub_51D790(0, value, payloadBits);
    case 1: return Sub_51E0D0(0, value, payloadBits);
    case 2: return Sub_51D790(1, value, payloadBits);
    case 3: return Sub_51E0D0(1, value, payloadBits);
    case 4: return Sub_51D790(2, value, payloadBits);
    case 5: return Sub_51E0D0(2, value, payloadBits);
    case 6: return Sub_51D790(3, value, payloadBits);
    case 7: return Sub_51E0D0(3, value, payloadBits);
    case 8: return Sub_51D790(4, value, payloadBits);
    case 9: return Sub_51E0D0(4, value, payloadBits);
    case 10: return Sub_51D790(5, value, payloadBits);
    case 11: return Sub_51E0D0(5, value, payloadBits);
    case 12: return Sub_51D790(6, value, payloadBits);
    case 13: return Sub_51E0D0(6, value, payloadBits);
    case 14: return Sub_51D790(7, value, payloadBits);
    case 15: return Sub_51E0D0(7, value, payloadBits);
    case 16: return Sub_51D790(8, value, payloadBits);
    case 17: return Sub_51E0D0(8, value, payloadBits);
    case 18: return Sub_51EC10(0, value);
    case 19: return Sub_51EC10(1, value);
    case 20: return Sub_51EF80(0, value, 0);
    case 21: return Sub_51EF80(1, value, 0);
    case 22: return Sub_51EF80(2, value, payloadBits);
    case 23: return Sub_51EF80(3, value, payloadBits);
    case 24:
    case 25: {
        const std::int32_t signedLow =
            std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(value));
        const std::int32_t selected = signedLow != 0
            ? signedLow : (selector == 24 ? -1 : 1);
        return Sub_51F610(selected);
    }
    case 26:
    case 28: return Sub_51F860(-1, value);
    case 27:
    case 33: return Sub_51F860(1, value);
    case 29:
    case 31: return Sub_51FA70(1, value);
    case 30:
    case 32: return Sub_51FA70(-1, value);
    case 34: return Sub_51FC90(0, value);
    case 35: return Sub_51FC90(1, value);
    default: return 1;
    }
}

} // namespace detail

} // namespace cg::world
