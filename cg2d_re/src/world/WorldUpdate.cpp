#include "cg/network/CgConnection.hpp"
#include "cg/session/ClientSession.hpp"
#include "cg/session/ClientHost.hpp"
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
    const bool submitPixels = cg::session::GetClientHost().CurrentSession() == &session;
    auto& inventory = session.UiFrame().native.inventory;
    const std::int32_t pendingSceneType = session.WorldScene().pendingSceneType;
    if (pendingSceneType >= 0) {
        session.WorldScene().sceneType = pendingSceneType;
        const std::int32_t pendingSceneState = session.WorldScene().pendingSceneState;
        session.WorldScene().pendingSceneType = -1;
        session.WorldScene().sceneState = pendingSceneState;
    }

    const std::int32_t sceneType = session.WorldScene().sceneType;
    session.WorldScene().map.surfaceCacheEnabled = 0;
    switch (sceneType) {
    case 1: {
        const ui::LegacyUiLayoutScope layout;
        session.WorldScene().frameBackgroundMode = 0;
        input::ProcessKeyInput_4596D0(session);
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
        cg::yobi::YobiUpdateAll_404C50(session);
        detail::RenderChara_521F20(session);
        detail::RenderInput_4590E0(session);
        cg::input::DrawAndAdvanceInputCaret_44A410(session);
        return;
    }
    case 2: {
        const ui::LegacyUiLayoutScope layout;
        session.WorldScene().frameBackgroundMode = 0;
        login::ProcessScene_45FEA0(session);
        cg::yobi::YobiUpdateAll_404C50(session);
        detail::RenderChara_521F20(session);
        return;
    }
    case 3: {
        const ui::LegacyUiLayoutScope layout;
        session.WorldScene().frameBackgroundMode = 0;
        character_select::ProcessScene_460FF0(session.Connection());
        cg::yobi::YobiUpdateAll_404C50(session);
        detail::RenderChara_521F20(session);
        return;
    }
    case 4:
        session.WorldScene().frameBackgroundMode = 0;
        input::ProcessKeyInput_4596D0(session);
        character_select::detail::ProcessCreationScene_4626A0(session.Connection(), 1);
        cg::yobi::YobiUpdateAll_404C50(session);
        detail::RenderChara_521F20(session);
        detail::RenderInput_4590E0(session);
        cg::input::DrawAndAdvanceInputCaret_44A410(session);
        return;
    case 5:
        session.WorldScene().frameBackgroundMode = 0;
        login::detail::BindSessionSavedAccount(session);
        static_cast<void>(platform::ResetImeOpenStatus_4590A0(session));
        detail::InitializeChatInputAndLog_449DA0(session);
        detail::ResetPendingPathEvent_46A9E0(session);
        detail::InitializeWorldMapState_467F30(session);
        detail::InitializePlayerRuntimeState_5199A0(session.Connection());
        detail::ResetWorldObjectTable_446470(session);
        detail::ResetCurrentDialogType_4D3170(session);
        detail::InitializeWorldSceneFlags_455BD0(session);
        detail::InitializeMapEffectPool_46E2A0(session);
        ResetMapDrawCacheCounts_46BD40();
        session.WorldScene().battle.entryPending = 0;
        session.WorldScene().gameplayRepliesSuppressed = 0;
        detail::InitializeGameplayNotifications_504A30(session);
        detail::ResetBattleCommandSelection_43C8F0(session);
        detail::ResetSceneUiState_504100(session);
        detail::ClearContactAndGuildMailStorage_518C80(session);
        detail::LoadContactAndGuildMailFiles_518CB0(session);
        session.WorldScene().pendingMapBgmIndex = 0xFFFFFFFFu;
        session.WorldScene().mapBgmChangePending = 0;
        session.WorldScene().logoutToTownPending = 0;
        cg::core::NullSub_509080();
        detail::LoadAlbumData_518780(session);
        ui::WindowRuntime_ProcessScene(session);
        cg::yobi::ResetObservationGroupList_43F220(session);
        session.WorldScene().observationTravelTransitionCount = 0;
        ClearMapName_9DAD30(session);
        SetCurrentSceneType(session, 6);
        cg::audio::BeginBgmFadeOut_52B990(session);
        inventory.slotUseCounts.fill(0);
        return;
    case 6:
        session.WorldScene().frameBackgroundMode = 0;
        login::ProcessState_45E960(session.Connection());
        goto sceneSixTail;
    case 7:
        detail::ProcessLogoutScene_45EBB0(session);
        cg::yobi::YobiUpdateAll_404C50(session);
        return;
    case 8:
        return;
    case 9:
        detail::DrawNormalScene_51C7D0(session);
        return;
    case 10:
        detail::DrawBattleScene_43CBB0(session);
        return;
    case 11: {
        const std::int32_t sceneState = session.WorldScene().sceneState;
        if (sceneState == 0) {
            detail::DestroyPlayerMapPresence_519D50(session);
            detail::ResetWorldObjectTable_446470(session);
            cg::yobi::YobiClearUp_404D30(session);
            ui::WindowRuntime_ProcessScene(session);
            cg::yobi::ResetObservationGroupList_43F220(session);
            BeginSceneTransition(session);
            detail::CaptureSceneAndSelectEvenVariant_51D750(session);
            session.WorldScene().frameBackgroundMode = 2;
            cg::audio::BeginBgmFadeOut_52B990(session);
            const std::uint64_t tick = platform::ReadTickCount64();
            if (submitPixels) {
                platform::gLastRenderDt = static_cast<double>(tick);
            }
            ++session.WorldScene().sceneState;
        } else if (sceneState == 1
            && detail::ProcessSceneTransitionEffect_51FDE0(session, 2, 0, 0x3F19999Au) == 1) {
            session.WorldScene().frameBackgroundMode = 0;
            ++session.WorldScene().sceneState;
        }

        if (detail::UpdateDisconnectDialog_455C90(session) == 0) {
            goto sceneSixTail;
        }
        cg::login::detail::SaveCurrentClientPreferences_520160(session.Connection());
        detail::DestroyPlayerMapPresence_519D50(session);
        detail::ResetWorldObjectTable_446470(session);
        session.Connection().Shutdown();
        session.Connection().DisconnectState() = 0;
        const std::int32_t capability = resource::ProbePaletteCapability_45C970();
        const std::int32_t paletteIndex = capability == 3 ? 27 : 0;
        render::gPaletteCapability = capability;
        render::SelectSessionPalette(session, paletteIndex, 0);
        SetCurrentSceneType(session, 2);
        cg::yobi::YobiClearUp_404D30(session);
        session.Connection().DisconnectState() = 0;
        return;
    }
    case 12:
        return;
    case 13:
        session.WorldScene().frameBackgroundMode = 0;
        input::ProcessKeyInput_4596D0(session);
        if (render::gPaletteCapability < 3) {
            character_select::ProcessLegacyVersionScene_45E3D0(session.Connection());
        } else {
            character_select::ProcessScene_462330(session.Connection());
        }
        cg::yobi::YobiUpdateAll_404C50(session);
        detail::RenderChara_521F20(session);
        detail::RenderInput_4590E0(session);
        cg::input::DrawAndAdvanceInputCaret_44A410(session);
        return;
    default:
        return;
    }

// 原函数：0x00404C50。
sceneSixTail:
    cg::yobi::YobiUpdateAll_404C50(session);
    detail::RenderChara_521F20(session);
}

void BeginSceneTransition(session::ClientSession& session)
{
    auto& scene = session.WorldScene();
    scene.transitionResetPending = 1;
    scene.skipTransitionPixels = cg::session::GetClientHost().CurrentSession() != &session;
    scene.transitionCompletionPending = false;
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
            session, dialog.x, dialog.y,
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
    hovered = cg::ui::HitTestMouseRect_5049A0(session,
        left, top, right, bottom, -1) != 0;
    if (hovered
        && (static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(session)) & 1U) != 0) {
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

// 原sub_51FDE0@[0x51FDE0,0x5200D0)：当前保原分派；后台只跳过程，仍走目标完成尾。
std::int32_t ProcessSceneTransitionEffect_51FDE0(
    session::ClientSession& session, std::int32_t selector, std::int32_t value, std::uint32_t payloadBits,
    const bool completionPolledLater)
{
    auto& scene = session.WorldScene();
    if (completionPolledLater) {
        scene.transitionCompletionPending = false;
    }
    if (scene.transitionCompletionPending) {
        scene.transitionCompletionPending = false;
        scene.frameBackgroundMode = 0;
        return 1;
    }
    const bool completeImmediately = scene.skipTransitionPixels
        || cg::session::GetClientHost().CurrentSession() != &session;
    scene.skipTransitionPixels = false;
    if (completeImmediately && static_cast<std::uint32_t>(selector) < 36U) {
        scene.transitionCompletionPending = completionPolledLater;
        scene.frameBackgroundMode = 0;
    }
    switch (selector) {
    case 0: return ProcessSceneSlideOut_51D790(session, 0, value, payloadBits, completeImmediately);
    case 1: return ProcessSceneSlideIn_51E0D0(session, 0, value, payloadBits, completeImmediately);
    case 2: return ProcessSceneSlideOut_51D790(session, 1, value, payloadBits, completeImmediately);
    case 3: return ProcessSceneSlideIn_51E0D0(session, 1, value, payloadBits, completeImmediately);
    case 4: return ProcessSceneSlideOut_51D790(session, 2, value, payloadBits, completeImmediately);
    case 5: return ProcessSceneSlideIn_51E0D0(session, 2, value, payloadBits, completeImmediately);
    case 6: return ProcessSceneSlideOut_51D790(session, 3, value, payloadBits, completeImmediately);
    case 7: return ProcessSceneSlideIn_51E0D0(session, 3, value, payloadBits, completeImmediately);
    case 8: return ProcessSceneSlideOut_51D790(session, 4, value, payloadBits, completeImmediately);
    case 9: return ProcessSceneSlideIn_51E0D0(session, 4, value, payloadBits, completeImmediately);
    case 10: return ProcessSceneSlideOut_51D790(session, 5, value, payloadBits, completeImmediately);
    case 11: return ProcessSceneSlideIn_51E0D0(session, 5, value, payloadBits, completeImmediately);
    case 12: return ProcessSceneSlideOut_51D790(session, 6, value, payloadBits, completeImmediately);
    case 13: return ProcessSceneSlideIn_51E0D0(session, 6, value, payloadBits, completeImmediately);
    case 14: return ProcessSceneSlideOut_51D790(session, 7, value, payloadBits, completeImmediately);
    case 15: return ProcessSceneSlideIn_51E0D0(session, 7, value, payloadBits, completeImmediately);
    case 16: return ProcessSceneSlideOut_51D790(session, 8, value, payloadBits, completeImmediately);
    case 17: return ProcessSceneSlideIn_51E0D0(session, 8, value, payloadBits, completeImmediately);
    case 18: return ProcessSceneQuadrantTransition_51EC10(session, 0, value, completeImmediately);
    case 19: return ProcessSceneQuadrantTransition_51EC10(session, 1, value, completeImmediately);
    case 20: return ProcessSceneRandomTileTransition_51EF80(session, 0, value, 0, completeImmediately);
    case 21: return ProcessSceneRandomTileTransition_51EF80(session, 1, value, 0, completeImmediately);
    case 22: return ProcessSceneRandomTileTransition_51EF80(session, 2, value, payloadBits, completeImmediately);
    case 23: return ProcessSceneRandomTileTransition_51EF80(session, 3, value, payloadBits, completeImmediately);
    case 24:
    case 25: {
        const std::int32_t signedLow =
            std::bit_cast<std::int8_t>(static_cast<std::uint8_t>(value));
        const std::int32_t selected = signedLow != 0
            ? signedLow : (selector == 24 ? -1 : 1);
        return ProcessSceneGridScaleTransition_51F610(session, selected, completeImmediately);
    }
    case 26:
    case 28: return ProcessTopScanlineTransition_51F860(session, -1, value, completeImmediately);
    case 27:
    case 33: return ProcessTopScanlineTransition_51F860(session, 1, value, completeImmediately);
    case 29:
    case 31: return ProcessBottomScanlineTransition_51FA70(session, 1, value, completeImmediately);
    case 30:
    case 32: return ProcessBottomScanlineTransition_51FA70(session, -1, value, completeImmediately);
    case 34: return ProcessSceneCenterScanlineTransition_51FC90(session, 0, value, completeImmediately);
    case 35: return ProcessSceneCenterScanlineTransition_51FC90(session, 1, value, completeImmediately);
    default: return 1;
    }
}

} // namespace detail

} // namespace cg::world
