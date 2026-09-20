#include "cg/CharacterSelectScene.hpp"

#include "CharacterSelectSceneDependencies.hpp"
#include "CharacterSelectState.hpp"

#include "cg/LoginMessage.hpp"
#include "cg/Palette.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/GraphicCacheState.hpp"
#include "cg/world/SceneState.hpp"

#include <bit>
#include <cstdint>

namespace cg::character_select {
namespace {

constexpr char kUpgradeProgressText[] = "版本升級中‧‧‧";
constexpr char kUpgradeSucceededText[] = "版本已經升級。";
constexpr char kUpgradeFailedText[] = "版本升級失敗。";
constexpr char kCannotUpgradeText[] = "無法更新版本。";

// 原 0x9A6EB4，映像初值为空，只由 sub_462330 直接持有。
login::YobiAction* gVersionUpgradeAction = nullptr;

std::uint32_t UnsignedBits(std::int32_t value)
{
    return std::bit_cast<std::uint32_t>(value);
}

} // namespace

std::int32_t ProcessScene_462330()
{
    switch (UnsignedBits(world::gCurrentSceneState)) {
    case 0:
        gVersionUpgradeAction = nullptr;
        [[fallthrough]];
    case 1:
        world::gCurrentSceneState = 2;
        break;
    case 2:
        if (render::gPaletteCapability >= 2) {
            const std::uint8_t pukVersion = state::gPukVersion;
            switch (pukVersion) {
            case 7:
                detail::Sub_512D30(
                    platform::gConnectionSocket,
                    5,
                    detail::gVersionPayload9A5C10);
                world::gCurrentSceneState = 3;
                break;
            case 5:
            case 6:
                detail::Sub_512D30(
                    platform::gConnectionSocket,
                    4,
                    state::kVersion30);
                world::gCurrentSceneState = 3;
                break;
            case 0:
            case 2:
                detail::Sub_512D30(
                    platform::gConnectionSocket,
                    2,
                    state::kVersion20);
                world::gCurrentSceneState = 3;
                break;
            default:
                world::gCurrentSceneState = 202;
                break;
            }
        }
        break;
    case 3: {
        gVersionUpgradeAction = nullptr;
        std::int32_t columns =
            (login::detail::TextMeasure(kUpgradeProgressText, 0) + 111) / 64;
        if (columns < 2) {
            columns = 2;
        }
        gVersionUpgradeAction = login::detail::Sub_5044E0(
            ((10 - columns) * 64) / 2,
            192,
            columns * 64,
            96,
            4,
            1);
        world::gCurrentSceneState = 4;
        break;
    }
    case 4:
        if (gVersionUpgradeAction != nullptr
            && login::detail::IsYobiActionStateEqual(
                gVersionUpgradeAction,
                1)) {
            const std::int32_t width =
                login::detail::TextMeasure(kUpgradeProgressText, 0);
            login::detail::DrawText(
                static_cast<std::int16_t>(320 - width / 2),
                232,
                2,
                0,
                0,
                kUpgradeProgressText,
                0,
                0);
        }
        break;
    case 100:
        if (gVersionUpgradeAction != nullptr) {
            login::detail::MarkDeath(gVersionUpgradeAction);
        }
        world::gCurrentSceneState = 101;
        login::ResetLoginMessage();
        break;
    case 101:
        if (login::UpdateLoginMessage(kUpgradeSucceededText) != 0) {
            detail::YobiClearUp_404D30();
            if (state::gPreviewActions[0] != nullptr) {
                state::gPreviewActions[0] = nullptr;
            }
            if (state::gPreviewActions[1] != nullptr) {
                state::gPreviewActions[1] = nullptr;
            }
            world::SetCurrentSceneType(3);
        }
        break;
    case 200:
        if (gVersionUpgradeAction != nullptr) {
            login::detail::MarkDeath(gVersionUpgradeAction);
        }
        login::ResetLoginMessage();
        world::gCurrentSceneState = 201;
        break;
    case 201:
        if (login::UpdateLoginMessage(kUpgradeFailedText) != 0) {
            world::gCurrentSceneState = 1;
        }
        break;
    case 202:
        world::gCurrentSceneState = 203;
        login::ResetLoginMessage();
        break;
    case 203:
        if (login::UpdateLoginMessage(kCannotUpgradeText) != 0) {
            world::gCurrentSceneState = 1;
        }
        break;
    default:
        break;
    }

    render::gGraphicCache4Queue[3] = 0;
    static_cast<void>(ProcessCharacterSlot_461B30(0, 1));
    static_cast<void>(ProcessCharacterSlot_461B30(1, 1));
    return detail::Sub_45F090(0, 0, 3, 245060, 0, -1);
}

} // namespace cg::character_select
