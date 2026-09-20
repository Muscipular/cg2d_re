#include "cg/world/SceneState.hpp"

#include "WorldUpdateDependencies.hpp"

#include "cg/CharacterSelectScene.hpp"
#include "cg/GraphicInfo.hpp"
#include "cg/LoginScene.hpp"
#include "cg/LoginState.hpp"
#include "cg/Palette.hpp"
#include "cg/YobiRuntime.hpp"
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
        detail::Sub_4596D0();
        const std::int32_t capability = render::gPaletteCapability;
        switch (capability) {
        case 0:
        case 1:
            detail::Sub_45C5A0();
            break;
        case 2:
            detail::Sub_45C2D0();
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
        detail::Sub_4596D0();
        detail::Sub_4626A0();
        login::detail::YobiUpdateAll_404C50();
        detail::RenderChara_521F20();
        detail::RenderInput_4590E0();
        detail::Sub_44A410();
        return;
    case 5:
        platform::gValue73ECC0 = 0;
        detail::Sub_4590A0();
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
        detail::NullSub_509080();
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
        detail::Sub_5081A0();
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
        detail::Sub_4596D0();
        if (render::gPaletteCapability < 3) {
            detail::Sub_45E3D0();
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

} // namespace cg::world
