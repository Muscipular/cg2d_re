#include "cg/network/CgConnection.hpp"
#include "cg/session/ClientSession.hpp"
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
#include "cg/scene/login/LoginRequest.hpp"
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
void Update(session::ClientSession& session)
{
    const std::int32_t pendingSceneType = gPendingSceneType;
    if (pendingSceneType >= 0) {
        gCurrentSceneType = pendingSceneType;
        const std::int32_t pendingSceneState = gPendingSceneState;
        gPendingSceneType = -1;
        gCurrentSceneState = pendingSceneState;
    }

    const std::int32_t sceneType = gCurrentSceneType;
    gMapSurfaceCacheEnabled_C82204 = 0;
    switch (sceneType) {
    case 1: {
        const ui::LegacyUiLayoutScope layout;
        platform::gFrameBackgroundMode_73ECC0 = 0;
        input::ProcessKeyInput_4596D0();
        const std::int32_t capability = render::gPaletteCapability;
        switch (capability) {
        case 0:
        case 1:
            login::ProcessModeScene_45C5A0(session.Connection());
            break;
        case 2:
            login::ProcessOuterScene_45C2D0(session.Connection());
            break;
        case 3:
            UpdateCapability3LoginScene_45F190(session.Connection());
            break;
        case 4:
            UpdateCapability4LoginScene_464840(session.Connection());
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
        platform::gFrameBackgroundMode_73ECC0 = 0;
        login::ProcessScene_45FEA0(session);
        cg::yobi::YobiUpdateAll_404C50();
        detail::RenderChara_521F20();
        return;
    }
    case 3: {
        const ui::LegacyUiLayoutScope layout;
        platform::gFrameBackgroundMode_73ECC0 = 0;
        character_select::ProcessScene_460FF0(session.Connection());
        cg::yobi::YobiUpdateAll_404C50();
        detail::RenderChara_521F20();
        return;
    }
    case 4:
        platform::gFrameBackgroundMode_73ECC0 = 0;
        input::ProcessKeyInput_4596D0();
        character_select::detail::ProcessCreationScene_4626A0(session.Connection(), 1);
        cg::yobi::YobiUpdateAll_404C50();
        detail::RenderChara_521F20();
        detail::RenderInput_4590E0();
        cg::input::DrawAndAdvanceInputCaret_44A410();
        return;
    case 5:
        platform::gFrameBackgroundMode_73ECC0 = 0;
        static_cast<void>(platform::ResetImeOpenStatus_4590A0());
        detail::InitializeChatInputAndLog_449DA0();
        detail::ResetPendingPathEvent_46A9E0();
        detail::InitializeWorldMapState_467F30();
        detail::InitializePlayerRuntimeState_5199A0(session.Connection());
        detail::ResetWorldObjectTable_446470();
        detail::ResetCurrentDialogType_4D3170();
        detail::InitializeWorldSceneFlags_455BD0();
        detail::InitializeMapEffectPool_46E2A0();
        ResetMapDrawCacheCounts_46BD40();
        gBattleEntryPending_5ABFC4 = 0;
        gGameplayRepliesSuppressed_9A6E0C = 0;
        detail::InitializeGameplayNotifications_504A30();
        detail::ResetBattleCommandSelection_43C8F0();
        detail::ResetSceneUiState_504100();
        detail::ClearContactAndGuildMailStorage_518C80();
        detail::LoadContactAndGuildMailFiles_518CB0();
        gPendingMapBgmIndex_FF0D5C = 0xFFFFFFFFu;
        gMapBgmChangePending_FF0D58 = 0;
        gLogoutToTownPending_D67E78 = 0;
        cg::core::NullSub_509080();
        detail::LoadAlbumData_518780();
        ui::WindowRuntime_ProcessScene();
        cg::yobi::ResetObservationGroupList_43F220();
        gObservationTravelTransitionCount_FF0D74 = 0;
        ClearMapName_9DAD30();
        SetCurrentSceneType(6);
        cg::audio::BeginBgmFadeOut_52B990();
        gInventorySlotUseCounts_D67B08.fill(0);
        return;
    case 6:
        platform::gFrameBackgroundMode_73ECC0 = 0;
        login::ProcessState_45E960(session.Connection());
        goto sceneSixTail;
    case 7:
        detail::ProcessLogoutScene_45EBB0(session);
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
            detail::DestroyPlayerMapPresence_519D50();
            detail::ResetWorldObjectTable_446470();
            cg::yobi::YobiClearUp_404D30();
            ui::WindowRuntime_ProcessScene();
            cg::yobi::ResetObservationGroupList_43F220();
            gSceneTransitionResetPending_58B6D0 = 1;
            detail::CaptureSceneAndSelectEvenVariant_51D750();
            platform::gFrameBackgroundMode_73ECC0 = 2;
            cg::audio::BeginBgmFadeOut_52B990();
            const std::uint64_t tick = platform::ReadTickCount64();
            platform::gLastRenderDt = static_cast<double>(tick);
            ++gCurrentSceneState;
        } else if (sceneState == 1
            && detail::ProcessSceneTransitionEffect_51FDE0(2, 0, 0x3F19999Au) == 1) {
            platform::gFrameBackgroundMode_73ECC0 = 0;
            ++gCurrentSceneState;
        }

        if (detail::UpdateDisconnectDialog_455C90(session) == 0) {
            goto sceneSixTail;
        }
        cg::login::detail::SaveCurrentClientPreferences_520160(session.Connection());
        detail::DestroyPlayerMapPresence_519D50();
        detail::ResetWorldObjectTable_446470();
        session.Connection().Shutdown();
        session.Connection().DisconnectState() = 0;
        const std::int32_t capability = resource::ProbePaletteCapability_45C970();
        const std::int32_t paletteIndex = capability == 3 ? 27 : 0;
        render::gPaletteCapability = capability;
        render::SelectPalette(paletteIndex, 0);
        SetCurrentSceneType(2);
        cg::yobi::YobiClearUp_404D30();
        session.Connection().DisconnectState() = 0;
        return;
    }
    case 12:
        return;
    case 13:
        platform::gFrameBackgroundMode_73ECC0 = 0;
        input::ProcessKeyInput_4596D0();
        if (render::gPaletteCapability < 3) {
            character_select::ProcessLegacyVersionScene_45E3D0(session.Connection());
        } else {
            character_select::ProcessScene_462330(session.Connection());
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

// 原函数：0x00455C90。
std::int32_t UpdateDisconnectDialog_455C90(session::ClientSession& session)
{
    auto& dialog = session.WorldScene().disconnectDialog;
    cg::render::GraphicDescriptor16 descriptor {
        0, 0, 0, 0, 0xFFFFFFFFu, 0, cg::render::GraphicMixFlag_OriginalPalette, 0,
    };
    std::int32_t clicked = 0;
    bool hovered = false;
    cg::yobi::YobiAction* action = dialog.action;
    if (action == nullptr) {
        dialog.sentinel = -2;
        // 模态背景按64×48瓦片取整，布局使用实际尺寸，统一背景、文字及命中位置。
        dialog.width = 384;
        dialog.height = 144;
        dialog.x = (platform::gWindowWidth - dialog.width) / 2;
        dialog.y = (platform::gWindowHeight - dialog.height) / 2;
        action = cg::yobi::CreateModalAction_5044E0(
            dialog.x, dialog.y,
            dialog.width, dialog.height, 4, 1);
        dialog.action = action;
        if (action == nullptr) {
            return 0;
        }
    }
    if (!cg::yobi::IsYobiActionReady(action, 1)) {
        return 0;
    }

    const std::int32_t scaledHitHeight = dialog.height * 4;
    const std::int32_t hitWidth = dialog.width;
    const std::int32_t hitWidthDelta = hitWidth - 66;
    const std::int32_t hitHeightStep = scaledHitHeight / 5;
    const std::int32_t hitY = dialog.y;
    const std::int32_t hitBaseY = hitY + hitHeightStep;
    const std::int32_t hitX = dialog.x;
    const std::int32_t left = (hitWidthDelta >> 1) + hitX;
    const std::int32_t bottom = hitBaseY + 18;
    const std::int32_t top = hitBaseY + 1;
    const std::int32_t right = left + 66;
    hovered = cg::ui::HitTestMouseRect_5049A0(
        left, top, right, bottom, -1) != 0;
    if (hovered
        && (static_cast<std::uint8_t>(cg::platform::gLegacyMouseState.edgeButtons) & 1U) != 0) {
        cg::yobi::MarkDeath(dialog.action);
        dialog.action = nullptr;
        clicked = 1;
    }

    const std::int32_t row = dialog.height / 5;
    std::int32_t width = cg::render::TextMeasure(dialog.text, 2);
    const std::int32_t firstWindowWidth = dialog.width;
    const std::int32_t firstWindowY = dialog.y;
    const std::int32_t firstWidthDelta = firstWindowWidth - width;
    const std::int32_t firstTextY = firstWindowY + row - 4;
    const std::int32_t firstWindowX = dialog.x;
    const std::int32_t firstTextX = firstWindowX + firstWidthDelta / 2;
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(firstTextX),
        static_cast<std::int16_t>(firstTextY),
        2, 2, 0, dialog.text, 0, 0));

    std::wstring_view secondText;
    std::wstring_view thirdText;
    std::wstring errorText;
    const bool connectionError = session.Connection().LoginRequests().connectionErrorPresent != 0;
    if (connectionError) {
        cg::text::FormatTo(errorText, L"伺服器连线中断({})", session.Connection().Error());
        secondText = errorText;
        thirdText = L"请尝试其他的伺服器。";
    } else {
        secondText = L"与伺服器的连线被切断了。";
        thirdText = L"回到伺服器选择画面。";
    }

    width = cg::render::TextMeasure(secondText, 0);
    const std::int32_t secondWindowWidth = dialog.width;
    std::int32_t secondWidthDelta;
    std::int32_t secondWindowY;
    if (connectionError) {
        secondWidthDelta = secondWindowWidth - width;
        secondWindowY = dialog.y;
    } else {
        secondWindowY = dialog.y;
        secondWidthDelta = secondWindowWidth - width;
    }
    const std::int32_t secondTextY = secondWindowY + 2 * row - 4;
    const std::int32_t secondWindowX = dialog.x;
    const std::int32_t secondTextX = secondWindowX + secondWidthDelta / 2;
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(secondTextX),
        static_cast<std::int16_t>(secondTextY),
        2, 0, 0, secondText, 0, 0));

    width = cg::render::TextMeasure(thirdText, 0);
    const std::int32_t thirdWindowWidth = dialog.width;
    std::int32_t thirdTextX;
    std::int32_t thirdTextY;
    if (connectionError) {
        const std::int32_t thirdWidthDelta = thirdWindowWidth - width;
        const std::int32_t thirdWindowY = dialog.y;
        const std::int32_t thirdHalfWidth = thirdWidthDelta / 2;
        const std::int32_t thirdBaseY = thirdWindowY + 2 * row;
        thirdTextY = thirdBaseY + row - 4;
        const std::int32_t thirdWindowX = dialog.x;
        thirdTextX = thirdWindowX + thirdHalfWidth;
    } else {
        const std::int32_t thirdWindowY = dialog.y;
        const std::int32_t thirdWidthDelta = thirdWindowWidth - width;
        const std::int32_t thirdBaseY = thirdWindowY + 2 * row;
        thirdTextY = thirdBaseY + row - 4;
        const std::int32_t thirdWindowX = dialog.x;
        thirdTextX = thirdWindowX + thirdWidthDelta / 2;
    }
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(thirdTextX),
        static_cast<std::int16_t>(thirdTextY),
        2, 0, 0, thirdText, 0, 0));

    const std::int32_t imageWindowY = dialog.y;
    const std::int32_t imageWindowWidth = dialog.width;
    const std::int32_t imageNo = hovered ? 243050 : 243048;
    const std::int32_t imageY = imageWindowY + 4 * row + 9;
    const std::int32_t imageWindowX = dialog.x;
    const std::int32_t imageX = (imageWindowWidth >> 1) + imageWindowX;
    static_cast<void>(cg::render::DrawGraphic(
        imageX, imageY, 0xDF, imageNo, 0, &descriptor));
    return clicked;
}

// 原 sub_51FDE0@[0x51FDE0,0x5200D0)：按 W32 selector 分派并透传目标返回。
std::int32_t ProcessSceneTransitionEffect_51FDE0(
    std::int32_t selector, std::int32_t value, std::uint32_t payloadBits)
{
    switch (selector) {
    case 0: return ProcessSceneSlideOut_51D790(0, value, payloadBits);
    case 1: return ProcessSceneSlideIn_51E0D0(0, value, payloadBits);
    case 2: return ProcessSceneSlideOut_51D790(1, value, payloadBits);
    case 3: return ProcessSceneSlideIn_51E0D0(1, value, payloadBits);
    case 4: return ProcessSceneSlideOut_51D790(2, value, payloadBits);
    case 5: return ProcessSceneSlideIn_51E0D0(2, value, payloadBits);
    case 6: return ProcessSceneSlideOut_51D790(3, value, payloadBits);
    case 7: return ProcessSceneSlideIn_51E0D0(3, value, payloadBits);
    case 8: return ProcessSceneSlideOut_51D790(4, value, payloadBits);
    case 9: return ProcessSceneSlideIn_51E0D0(4, value, payloadBits);
    case 10: return ProcessSceneSlideOut_51D790(5, value, payloadBits);
    case 11: return ProcessSceneSlideIn_51E0D0(5, value, payloadBits);
    case 12: return ProcessSceneSlideOut_51D790(6, value, payloadBits);
    case 13: return ProcessSceneSlideIn_51E0D0(6, value, payloadBits);
    case 14: return ProcessSceneSlideOut_51D790(7, value, payloadBits);
    case 15: return ProcessSceneSlideIn_51E0D0(7, value, payloadBits);
    case 16: return ProcessSceneSlideOut_51D790(8, value, payloadBits);
    case 17: return ProcessSceneSlideIn_51E0D0(8, value, payloadBits);
    case 18: return ProcessSceneQuadrantTransition_51EC10(0, value);
    case 19: return ProcessSceneQuadrantTransition_51EC10(1, value);
    case 20: return ProcessSceneRandomTileTransition_51EF80(0, value, 0);
    case 21: return ProcessSceneRandomTileTransition_51EF80(1, value, 0);
    case 22: return ProcessSceneRandomTileTransition_51EF80(2, value, payloadBits);
    case 23: return ProcessSceneRandomTileTransition_51EF80(3, value, payloadBits);
    case 24:
    case 25: {
        const std::int32_t signedLow =
            std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(value));
        const std::int32_t selected = signedLow != 0
            ? signedLow : (selector == 24 ? -1 : 1);
        return ProcessSceneGridScaleTransition_51F610(selected);
    }
    case 26:
    case 28: return ProcessTopScanlineTransition_51F860(-1, value);
    case 27:
    case 33: return ProcessTopScanlineTransition_51F860(1, value);
    case 29:
    case 31: return ProcessBottomScanlineTransition_51FA70(1, value);
    case 30:
    case 32: return ProcessBottomScanlineTransition_51FA70(-1, value);
    case 34: return ProcessSceneCenterScanlineTransition_51FC90(0, value);
    case 35: return ProcessSceneCenterScanlineTransition_51FC90(1, value);
    default: return 1;
    }
}

} // namespace detail

} // namespace cg::world
