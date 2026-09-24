#include "cg/world/SceneState.hpp"

#include "../platform/ImeLifecycle.hpp"
#include "WorldUpdateDependencies.hpp"

#include "cg/CharacterSelectScene.hpp"
#include "cg/GraphicInfo.hpp"
#include "cg/Input.hpp"
#include "cg/LoginScene.hpp"
#include "cg/LoginState.hpp"
#include "cg/Palette.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/MainLoop.hpp"

#include <bit>

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
        detail::Sub_43F220();
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
            detail::Sub_43F220();
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

std::int32_t Sub_45EF80()
{
    constexpr double kFrameStep = std::bit_cast<double>(
        std::uint64_t {0x4030AAAAAAAAAAABULL});
    const double threshold = platform::gLastRenderDt + kFrameStep;
    const std::uint32_t tick = platform::ReadTickCount32();
    return static_cast<double>(tick) > threshold ? 1 : 0;
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
