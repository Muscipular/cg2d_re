#include "cg/network/CgConnection.hpp"
#include "cg/render/TextPendingQueue.hpp"
#include "cg/render/Font.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/scene/character_select/CharacterSelectScene.hpp"

#include "CharacterSelectSceneDependencies.hpp"
#include "CharacterSelectSceneState.hpp"
#include "CharacterSelectState.hpp"

#include "cg/scene/login/LoginMessage.hpp"
#include "cg/render/Palette.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/render/GraphicCacheState.hpp"
#include "cg/world/SceneState.hpp"

#include <cstdint>

namespace cg::character_select {
// BRT01：原未装载零前缀，两发送路径只读首NUL；1B为已证正常读足迹。
// 原未使用预留尾部不猜容量，完整合同见版本正文SPEC。
namespace detail {
const wchar_t gVersionPayload9A5C10[1] {}; // 原全局：0x009A5C10。
} // namespace detail

// 原 0x55AC58/0x55C8F0/0x55C900/0x55C8E0，供两条版本场景复用原字节。
static constexpr wchar_t kUpgradeProgressText[] =
    L"版本升级中···";
static constexpr wchar_t kUpgradeSucceededText[] =
    L"版本已经升级。";
static constexpr wchar_t kUpgradeFailedText[] =
    L"版本升级失败。";
static constexpr wchar_t kCannotUpgradeText[] =
    L"无法更新版本。";
// 原函数：0x0045E3D0。
std::int32_t ProcessLegacyVersionScene_45E3D0(network::CgConnection& connection)
{
    auto& scene = connection.CharacterSelectUi();
    switch (static_cast<std::uint32_t>(connection.Owner().WorldScene().sceneState)) {
    case 0:
        scene.legacyVersionUpgradeAction = nullptr;
        [[fallthrough]];
    case 1:
        connection.Owner().WorldScene().sceneState = 2;
        break;
    case 2:
        if (render::gPaletteCapability >= 2) {
            const std::uint8_t pukVersion = connection.PukVersion();
            if (pukVersion == 0 || pukVersion == 2) {
                static_cast<void>(connection.SendVersionUpgradeRequest(2,
                    L"abcdefg" /* 原 0x55C910。 */));
                connection.Owner().WorldScene().sceneState = 3;
            } else {
                connection.Owner().WorldScene().sceneState = 202;
            }
        }
        break;
    case 3: {
        scene.legacyVersionUpgradeAction = nullptr;
        std::int32_t columns =
            (cg::render::TextMeasure(kUpgradeProgressText, 0) + 111) / 64;
        if (columns < 2) {
            columns = 2;
        }
        scene.legacyVersionUpgradeAction = cg::yobi::CreateModalAction_5044E0(
            connection.Owner(), ((10 - columns) * 64) / 2,
            192,
            columns * 64,
            96,
            1,
            1);
        connection.Owner().WorldScene().sceneState = 4;
        break;
    }
    case 4: {
        cg::yobi::YobiAction* const action = scene.legacyVersionUpgradeAction;
        if (action != nullptr
            && cg::yobi::IsYobiActionStateEqual(
                action,
                1)) {
            const std::int32_t width =
                cg::render::TextMeasure(kUpgradeProgressText, 0);
            cg::render::DrawText_456780(
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
        cg::yobi::YobiAction* const action = scene.legacyVersionUpgradeAction;
        if (action != nullptr) {
            cg::yobi::MarkDeath(action);
        }
        connection.Owner().WorldScene().sceneState = 101;
        login::ResetLoginMessage(connection);
        break;
    }
    case 101:
        if (login::UpdateLoginMessage(connection, kUpgradeSucceededText) != 0) {
            world::SetCurrentSceneType(connection.Owner(), 3);
        }
        break;
    case 200: {
        cg::yobi::YobiAction* const action = scene.legacyVersionUpgradeAction;
        if (action != nullptr) {
            cg::yobi::MarkDeath(action);
        }
        login::ResetLoginMessage(connection);
        connection.Owner().WorldScene().sceneState = 201;
        break;
    }
    case 201:
        if (login::UpdateLoginMessage(connection, kUpgradeFailedText) != 0) {
            connection.Owner().WorldScene().sceneState = 1;
        }
        break;
    case 202:
        connection.Owner().WorldScene().sceneState = 203;
        login::ResetLoginMessage(connection);
        break;
    case 203:
        if (login::UpdateLoginMessage(connection, kCannotUpgradeText) != 0) {
            connection.Owner().WorldScene().sceneState = 1;
        }
        break;
    default:
        break;
    }

    return cg::render::DrawGraphic(320, 240, 3, 22030, 0, nullptr);
}

// 原函数：0x00462330。
std::int32_t ProcessScene_462330(network::CgConnection& connection)
{
    auto& scene = connection.CharacterSelectUi();
    switch (static_cast<std::uint32_t>(connection.Owner().WorldScene().sceneState)) {
    case 0:
        scene.versionUpgradeAction = nullptr;
        [[fallthrough]];
    case 1:
        connection.Owner().WorldScene().sceneState = 2;
        break;
    case 2:
        if (render::gPaletteCapability >= 2) {
            const std::uint8_t pukVersion = connection.PukVersion();
            switch (pukVersion) {
            case 7:
                connection.SendVersionUpgradeRequest(5,
                    detail::gVersionPayload9A5C10);
                connection.Owner().WorldScene().sceneState = 3;
                break;
            case 5:
            case 6:
                connection.SendVersionUpgradeRequest(4,
                    state::kVersion30);
                connection.Owner().WorldScene().sceneState = 3;
                break;
            case 0:
            case 2:
                connection.SendVersionUpgradeRequest(2,
                    state::kVersion20);
                connection.Owner().WorldScene().sceneState = 3;
                break;
            default:
                connection.Owner().WorldScene().sceneState = 202;
                break;
            }
        }
        break;
    case 3: {
        scene.versionUpgradeAction = nullptr;
        std::int32_t columns =
            (cg::render::TextMeasure(kUpgradeProgressText, 0) + 111) / 64;
        if (columns < 2) {
            columns = 2;
        }
        scene.versionUpgradeAction = cg::yobi::CreateModalAction_5044E0(
            connection.Owner(), ((10 - columns) * 64) / 2,
            192,
            columns * 64,
            96,
            4,
            1);
        connection.Owner().WorldScene().sceneState = 4;
        break;
    }
    case 4: {
        cg::yobi::YobiAction* const action = scene.versionUpgradeAction;
        if (action != nullptr
            && cg::yobi::IsYobiActionStateEqual(
                action,
                1)) {
            const std::int32_t width =
                cg::render::TextMeasure(kUpgradeProgressText, 0);
            cg::render::DrawText_456780(
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
        cg::yobi::YobiAction* const action = scene.versionUpgradeAction;
        if (action != nullptr) {
            cg::yobi::MarkDeath(action);
        }
        connection.Owner().WorldScene().sceneState = 101;
        login::ResetLoginMessage(connection);
        break;
    }
    case 101:
        if (login::UpdateLoginMessage(connection, kUpgradeSucceededText) != 0) {
            cg::yobi::YobiClearUp_404D30(connection.Owner());
            if (scene.previewActions[0] != nullptr) {
                scene.previewActions[0] = nullptr;
            }
            if (scene.previewActions[1] != nullptr) {
                scene.previewActions[1] = nullptr;
            }
            world::SetCurrentSceneType(connection.Owner(), 3);
        }
        break;
    case 200: {
        cg::yobi::YobiAction* const action = scene.versionUpgradeAction;
        if (action != nullptr) {
            cg::yobi::MarkDeath(action);
        }
        login::ResetLoginMessage(connection);
        connection.Owner().WorldScene().sceneState = 201;
        break;
    }
    case 201:
        if (login::UpdateLoginMessage(connection, kUpgradeFailedText) != 0) {
            connection.Owner().WorldScene().sceneState = 1;
        }
        break;
    case 202:
        connection.Owner().WorldScene().sceneState = 203;
        login::ResetLoginMessage(connection);
        break;
    case 203:
        if (login::UpdateLoginMessage(connection, kCannotUpgradeText) != 0) {
            connection.Owner().WorldScene().sceneState = 1;
        }
        break;
    default:
        break;
    }

    render::gGraphicCache4Queue[3] = 0;
    static_cast<void>(ProcessCharacterSlot_461B30(connection, 0, 1));
    static_cast<void>(ProcessCharacterSlot_461B30(connection, 1, 1));
    return cg::render::QueueOriginalPaletteGraphic_45F090(0, 0, 3, 245060, 0, -1);
}

} // namespace cg::character_select
