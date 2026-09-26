#include "cg/world/SceneState.hpp"

#include "../LoginMessageDependencies.hpp"
#include "../platform/ImeLifecycle.hpp"
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

#include <bit>
#include <cstdio>

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

    gValueC82204 = 0;
    const std::int32_t sceneType = gCurrentSceneType;
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
        detail::Sub_4626A0();
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
        detail::Sub_503E80();
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
            detail::Sub_503E80();
            login::detail::Sub_43F220();
            gValue58B6D0 = 1;
            detail::Sub_51D750();
            platform::gValue73ECC0 = 2;
            detail::Sub_52B990();
            const std::uint32_t tick = platform::ReadTickCount32();
            platform::gLastRenderDt = static_cast<double>(tick);
            gCurrentSceneState = std::bit_cast<std::int32_t>(
                std::bit_cast<std::uint32_t>(gCurrentSceneState) + 1u);
        } else if (sceneState == 1
            && detail::Sub_51FDE0(2, 0, 0x3F19999Au) == 1) {
            platform::gValue73ECC0 = 0;
            gCurrentSceneState = std::bit_cast<std::int32_t>(
                std::bit_cast<std::uint32_t>(gCurrentSceneState) + 1u);
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

sceneSixTail:
    login::detail::YobiUpdateAll_404C50();
    detail::RenderChara_521F20();
}

namespace detail {

char gDisconnectText9A4D10[16] {};

namespace {

login::YobiAction* gDisconnectAction62B5B4 = nullptr;
std::int32_t gDisconnectState62B3A8 = 0;
std::int32_t gDisconnectWidth62B32C = 0;
std::int32_t gDisconnectHeight62B304 = 0;
std::int32_t gDisconnectX62B390 = 0;
std::int32_t gDisconnectY62B2EC = 0;

constexpr char kErrorFormat[] =
    "\xA6\xF8\xAA\x41\xBE\xB9\xB3\x73\xBD\x75\xA4\xA4"
    "\xC2\x5F\x28\x25\x64\x29";
constexpr char kTryOtherServer[] =
    "\xBD\xD0\xB9\xC1\xB8\xD5\xA8\xE4\xA5\x4C\xAA\xBA"
    "\xA6\xF8\xAA\x41\xBE\xB9\xA1\x43";
constexpr char kDisconnected[] =
    "\xBB\x50\xA6\xF8\xAA\x41\xBE\xB9\xAA\xBA\xB3\x73"
    "\xBD\x75\xB3\x51\xA4\xC1\xC2\x5F\xA4\x46\xA1\x43";
constexpr char kReturnToServerSelect[] =
    "\xA6\x5E\xA8\xEC\xA6\xF8\xAA\x41\xBE\xB9\xBF\xEF"
    "\xBE\xDC\xB5\x65\xAD\xB1\xA1\x43";

static_assert(sizeof(kErrorFormat) == 19);
static_assert(sizeof(kTryOtherServer) == 21);
static_assert(sizeof(kDisconnected) == 25);
static_assert(sizeof(kReturnToServerSelect) == 21);

} // namespace

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
    if (gDisconnectAction62B5B4 == nullptr) {
        if (audio::Sub_5043D0(2) != 0) {
            static_cast<void>(audio::Sub_5042D0(100));
        }
        gDisconnectState62B3A8 = -2;
        gDisconnectWidth62B32C = 380;
        gDisconnectHeight62B304 = 144;
        gDisconnectX62B390 = 130;
        gDisconnectY62B2EC = 156;
        gDisconnectAction62B5B4 =
            login::detail::Sub_5044E0(130, 156, 380, 144, 4, 1);
        if (gDisconnectAction62B5B4 == nullptr) {
            return 0;
        }
    }
    if (!login::detail::IsYobiActionReady(gDisconnectAction62B5B4, 1)) {
        return 0;
    }

    const std::int32_t left =
        gDisconnectX62B390 + (gDisconnectWidth62B32C - 66) / 2;
    const std::int32_t top =
        gDisconnectY62B2EC + (gDisconnectHeight62B304 * 4) / 5 + 1;
    hovered = login::detail::Sub_5049A0(
        left, top, left + 66, top + 17, -1) != 0;
    if (hovered
        && (static_cast<std::uint8_t>(login::detail::gValueD68D9C) & 1U) != 0) {
        login::detail::MarkDeath(gDisconnectAction62B5B4);
        gDisconnectAction62B5B4 = nullptr;
        clicked = 1;
    }

    const std::int32_t row = gDisconnectHeight62B304 / 5;
    std::int32_t width = login::detail::TextMeasure(gDisconnectText9A4D10, 2);
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(
            gDisconnectX62B390 + (gDisconnectWidth62B32C - width) / 2),
        static_cast<std::int16_t>(gDisconnectY62B2EC + row - 4),
        2, 2, 0, gDisconnectText9A4D10, 0, 0));

    const char* secondText;
    const char* thirdText;
    char errorText[256];
    if (login::gValue593FC0 != 0) {
        std::sprintf(errorText, kErrorFormat, network::ReadNetworkErrorD9F2CC());
        secondText = errorText;
        thirdText = kTryOtherServer;
    } else {
        secondText = kDisconnected;
        thirdText = kReturnToServerSelect;
    }

    width = login::detail::TextMeasure(secondText, 0);
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(
            gDisconnectX62B390 + (gDisconnectWidth62B32C - width) / 2),
        static_cast<std::int16_t>(gDisconnectY62B2EC + 2 * row - 4),
        2, 0, 0, secondText, 0, 0));

    width = login::detail::TextMeasure(thirdText, 0);
    static_cast<void>(render::DrawText_456780(
        static_cast<std::int16_t>(
            gDisconnectX62B390 + (gDisconnectWidth62B32C - width) / 2),
        static_cast<std::int16_t>(gDisconnectY62B2EC + 3 * row - 4),
        2, 0, 0, thirdText, 0, 0));

    static_cast<void>(login::detail::DrawGraphic(
        gDisconnectX62B390 + gDisconnectWidth62B32C / 2,
        gDisconnectY62B2EC + 4 * row + 9,
        0xDF,
        hovered ? 243050 : 243048,
        0,
        &descriptor));
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
