#include "cg/world/SceneState.hpp"

#include "../CharacterSelectSceneDependencies.hpp"
#include "../LoginMessageDependencies.hpp"
#include "../platform/ImeLifecycle.hpp"
#include "../platform/LegacyFormat.hpp"
#include "WorldUpdateDependencies.hpp"

#include "cg/CharacterSelectScene.hpp"
#include "cg/GraphicInfo.hpp"
#include "cg/Input.hpp"
#include "cg/LoginConnection.hpp"
#include "cg/LoginScene.hpp"
#include "cg/LoginState.hpp"
#include "cg/Palette.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/ui/WindowRuntime.hpp"

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
            detail::Sub_45F190();
            break;
        case 4:
            detail::Sub_464840();
            break;
        default:
            break;
        }
        login::detail::YobiUpdateAll_404C50();
        detail::RenderChara_521F20();
        detail::RenderInput_4590E0();
        detail::Sub_44A410();
        return;
    }
    case 2:
        platform::gValue73ECC0 = 0;
        login::ProcessScene_45FEA0();
        login::detail::YobiUpdateAll_404C50();
        detail::RenderChara_521F20();
        return;
    case 3:
        platform::gValue73ECC0 = 0;
        character_select::ProcessScene_460FF0();
        login::detail::YobiUpdateAll_404C50();
        detail::RenderChara_521F20();
        return;
    case 4:
        platform::gValue73ECC0 = 0;
        input::ProcessKeyInput_4596D0();
        character_select::detail::ProcessCreationScene_4626A0(1);
        login::detail::YobiUpdateAll_404C50();
        detail::RenderChara_521F20();
        detail::RenderInput_4590E0();
        detail::Sub_44A410();
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
        detail::Sub_46BD40();
        gValue5ABFC4 = 0;
        gValue9A6E0C = 0;
        detail::Sub_504A30();
        detail::Sub_43C8F0();
        detail::Sub_504100();
        detail::Sub_518C80();
        detail::Sub_518CB0();
        gValueFF0D5C = 0xFFFFFFFFu;
        gValueFF0D58 = 0;
        gValueD67E78 = 0;
        cg::NullSub_509080();
        detail::Sub_518780();
        ui::ProcessSceneWindows_503E80();
        login::detail::Sub_43F220();
        gValueFF0D74 = 0;
        gValue9DAD30 = 0;
        SetCurrentSceneType(6);
        detail::Sub_52B990();
        gValueD67B08.fill(0);
        return;
    case 6:
        platform::gValue73ECC0 = 0;
        login::ProcessState_45E960();
        goto sceneSixTail;
    case 7:
        detail::Sub_45EBB0();
        login::detail::YobiUpdateAll_404C50();
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
            login::detail::YobiClearUp_404D30();
            ui::ProcessSceneWindows_503E80();
            login::detail::Sub_43F220();
            gValue58B6D0 = 1;
            detail::Sub_51D750();
            platform::gValue73ECC0 = 2;
            detail::Sub_52B990();
            const std::uint32_t tick = platform::ReadTickCount32();
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
        detail::Sub_520160();
        detail::Sub_519D50();
        detail::Sub_446470();
        cg::network::ShutdownNetworkRuntime_5081A0();
        const std::int32_t capability = resource::ProbePaletteCapability_45C970();
        const std::int32_t paletteIndex = capability == 3 ? 27 : 0;
        render::gPaletteCapability = capability;
        render::SelectPalette(paletteIndex, 0);
        SetCurrentSceneType(2);
        login::detail::YobiClearUp_404D30();
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
        login::detail::YobiUpdateAll_404C50();
        detail::RenderChara_521F20();
        detail::RenderInput_4590E0();
        detail::Sub_44A410();
        return;
    default:
        return;
    }

// 原函数：0x00404C50。
sceneSixTail:
    login::detail::YobiUpdateAll_404C50();
    detail::RenderChara_521F20();
}

namespace detail {

char gDisconnectText9A4D10[16] {}; // 原全局：0x009A4D10。

namespace {

login::YobiAction* gDisconnectAction62B5B4 = nullptr; // 原全局：0x0062B5B4。
std::int32_t gDisconnectState62B3A8 = 0; // 原全局：0x0062B3A8。
std::int32_t gDisconnectWidth62B32C = 0; // 原全局：0x0062B32C。
std::int32_t gDisconnectHeight62B304 = 0; // 原全局：0x0062B304。
std::int32_t gDisconnectX62B390 = 0; // 原全局：0x0062B390。
std::int32_t gDisconnectY62B2EC = 0; // 原全局：0x0062B2EC。

constexpr char kErrorFormat[] =
    "\xCB\xC5\xB7\xFE\xC6\xF7\xDF\x42\xBE\x80\xD6\xD0"
    "\x94\xE0\x28\x25\x64\x29";
constexpr char kTryOtherServer[] =
    "\xD5\x88\x87\x4C\xD4\x87\xC6\xE4\xCB\xFB\xB5\xC4"
    "\xCB\xC5\xB7\xFE\xC6\xF7\xA1\xA3";
constexpr char kDisconnected[] =
    "\xC5\x63\xCB\xC5\xB7\xFE\xC6\xF7\xB5\xC4\xDF\x42"
    "\xBE\x80\xB1\xBB\xC7\xD0\x94\xE0\xC1\xCB\xA1\xA3";
constexpr char kReturnToServerSelect[] =
    "\xBB\xD8\xB5\xBD\xCB\xC5\xB7\xFE\xC6\xF7\xDF\x78"
    "\x93\xF1\xAE\x8B\xC3\xE6\xA1\xA3";

static_assert(sizeof(kErrorFormat) == 19);
static_assert(sizeof(kTryOtherServer) == 21);
static_assert(sizeof(kDisconnected) == 25);
static_assert(sizeof(kReturnToServerSelect) == 21);

} // namespace

// 原函数：0x00455C90。
std::int32_t Sub_455C90()
{
    login::detail::GraphicDescriptor16 descriptor {{
        0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00,
        0xFF, 0xFF, 0xFF, 0xFF,
        0x00, 0x04, 0x00, 0x00,
    }};
    std::int32_t clicked = 0;
    bool hovered = false;
    login::YobiAction* action = gDisconnectAction62B5B4;
    if (action == nullptr) {
        if (audio::Sub_5043D0(2) != 0) {
            static_cast<void>(audio::Sub_5042D0(100));
        }
        gDisconnectState62B3A8 = -2;
        gDisconnectWidth62B32C = 380;
        gDisconnectHeight62B304 = 144;
        gDisconnectX62B390 = 130;
        gDisconnectY62B2EC = 156;
        action = login::detail::Sub_5044E0(130, 156, 380, 144, 4, 1);
        gDisconnectAction62B5B4 = action;
        if (action == nullptr) {
            return 0;
        }
    }
    if (!login::detail::IsYobiActionReady(action, 1)) {
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
    hovered = login::detail::Sub_5049A0(
        left, top, right, bottom, -1) != 0;
    if (hovered
        && (static_cast<std::uint8_t>(login::detail::gValueD68D9C) & 1U) != 0) {
        login::detail::MarkDeath(gDisconnectAction62B5B4);
        gDisconnectAction62B5B4 = nullptr;
        clicked = 1;
    }

    const std::int32_t row = gDisconnectHeight62B304 / 5;
    std::int32_t width = login::detail::TextMeasure(gDisconnectText9A4D10, 2);
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

    const char* secondText;
    const char* thirdText;
    char errorText[256];
    const bool connectionError = login::gValue593FC0 != 0;
    if (connectionError) {
        std::sprintf(errorText, kErrorFormat, network::ReadNetworkErrorD9F2CC());
        secondText = errorText;
        thirdText = kTryOtherServer;
    } else {
        secondText = kDisconnected;
        thirdText = kReturnToServerSelect;
    }

    width = login::detail::TextMeasure(secondText, 0);
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

    width = login::detail::TextMeasure(thirdText, 0);
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
    static_cast<void>(login::detail::DrawGraphic(
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
