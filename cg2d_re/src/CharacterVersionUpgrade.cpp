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
// BRT01：原未装载零前缀，两发送路径只读首NUL；1B为已证正常读足迹。
// 原未使用预留尾部不猜容量，完整合同见版本正文SPEC。
namespace detail {
const char gVersionPayload9A5C10[1] {}; // 原全局：0x009A5C10。
} // namespace detail

namespace {

// 原 0x55AC58/0x55C8F0/0x55C900/0x55C8E0，供两条版本场景复用原字节。
constexpr char kUpgradeProgressText[] =
    "\xB0\xE6\xB1\xBE\xC9\xFD\xBC\x89\xD6\xD0\xA1\xA4\xA1\xA4\xA1\xA4";
constexpr char kUpgradeSucceededText[] =
    "\xB0\xE6\xB1\xBE\xD2\xD1\xBD\x9B\xC9\xFD\xBC\x89\xA1\xA3";
constexpr char kUpgradeFailedText[] =
    "\xB0\xE6\xB1\xBE\xC9\xFD\xBC\x89\xCA\xA7\x94\xA1\xA1\xA3";
constexpr char kCannotUpgradeText[] =
    "\x9F\x6F\xB7\xA8\xB8\xFC\xD0\xC2\xB0\xE6\xB1\xBE\xA1\xA3";
constexpr char kLegacyVersionRequest[] = "abcdefg"; // 原 0x55C910。

static_assert(sizeof(kUpgradeProgressText) == 17);
static_assert(sizeof(kUpgradeSucceededText) == 15);
static_assert(sizeof(kUpgradeFailedText) == 15);
static_assert(sizeof(kCannotUpgradeText) == 15);
static_assert(sizeof(kLegacyVersionRequest) == 8);

// 原 0x9A6EB4，映像初值为空，只由 sub_462330 直接持有。
login::YobiAction* gVersionUpgradeAction = nullptr;
// 原 0x9A6E68，仅 sub_45E3D0 直接读写，与 0x9A6EB4 分槽。
login::YobiAction* gLegacyVersionUpgradeAction = nullptr;

std::uint32_t UnsignedBits(std::int32_t value)
{
    return std::bit_cast<std::uint32_t>(value);
}

} // namespace

// 原函数：0x0045E3D0。
std::int32_t ProcessLegacyVersionScene_45E3D0()
{
    switch (UnsignedBits(world::gCurrentSceneState)) {
    case 0:
        gLegacyVersionUpgradeAction = nullptr;
        [[fallthrough]];
    case 1:
        world::gCurrentSceneState = 2;
        break;
    case 2:
        if (render::gPaletteCapability >= 2) {
            const std::uint8_t pukVersion = state::gPukVersion;
            if (pukVersion == 0 || pukVersion == 2) {
                static_cast<void>(detail::Sub_512D30(
                    platform::gConnectionSocket,
                    2,
                    kLegacyVersionRequest));
                world::gCurrentSceneState = 3;
            } else {
                world::gCurrentSceneState = 202;
            }
        }
        break;
    case 3: {
        gLegacyVersionUpgradeAction = nullptr;
        std::int32_t columns =
            (login::detail::TextMeasure(kUpgradeProgressText, 0) + 111) / 64;
        if (columns < 2) {
            columns = 2;
        }
        gLegacyVersionUpgradeAction = login::detail::Sub_5044E0(
            ((10 - columns) * 64) / 2,
            192,
            columns * 64,
            96,
            1,
            1);
        world::gCurrentSceneState = 4;
        break;
    }
    case 4: {
        login::YobiAction* const action = gLegacyVersionUpgradeAction;
        if (action != nullptr
            && login::detail::IsYobiActionStateEqual(
                action,
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
    }
    case 100: {
        login::YobiAction* const action = gLegacyVersionUpgradeAction;
        if (action != nullptr) {
            login::detail::MarkDeath(action);
        }
        world::gCurrentSceneState = 101;
        login::ResetLoginMessage();
        break;
    }
    case 101:
        if (login::UpdateLoginMessage(kUpgradeSucceededText) != 0) {
            world::SetCurrentSceneType(3);
        }
        break;
    case 200: {
        login::YobiAction* const action = gLegacyVersionUpgradeAction;
        if (action != nullptr) {
            login::detail::MarkDeath(action);
        }
        login::ResetLoginMessage();
        world::gCurrentSceneState = 201;
        break;
    }
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

    return login::detail::DrawGraphic(320, 240, 3, 22030, 0, nullptr);
}

// 原函数：0x00462330。
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
    case 4: {
        login::YobiAction* const action = gVersionUpgradeAction;
        if (action != nullptr
            && login::detail::IsYobiActionStateEqual(
                action,
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
    }
    case 100: {
        login::YobiAction* const action = gVersionUpgradeAction;
        if (action != nullptr) {
            login::detail::MarkDeath(action);
        }
        world::gCurrentSceneState = 101;
        login::ResetLoginMessage();
        break;
    }
    case 101:
        if (login::UpdateLoginMessage(kUpgradeSucceededText) != 0) {
            login::detail::YobiClearUp_404D30();
            if (state::gPreviewActions[0] != nullptr) {
                state::gPreviewActions[0] = nullptr;
            }
            if (state::gPreviewActions[1] != nullptr) {
                state::gPreviewActions[1] = nullptr;
            }
            world::SetCurrentSceneType(3);
        }
        break;
    case 200: {
        login::YobiAction* const action = gVersionUpgradeAction;
        if (action != nullptr) {
            login::detail::MarkDeath(action);
        }
        login::ResetLoginMessage();
        world::gCurrentSceneState = 201;
        break;
    }
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
