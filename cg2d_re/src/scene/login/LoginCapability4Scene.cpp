#include "cg/session/ClientSession.hpp"
#include "LoginSavedState.hpp"
#include "../../platform/StartupNative.hpp"
#include <string>
#include "cg/text/Encoding.hpp"

#include "LoginCapability4Modal.hpp"
#include "LoginInputDependencies.hpp"
#include "LoginSceneDependencies.hpp"

#include "cg/input/Input.hpp"
#include "cg/scene/login/LoginInput.hpp"
#include "cg/scene/login/LoginMessage.hpp"
#include "cg/network/CgConnection.hpp"
#include "LoginSceneState.hpp"
#include "cg/render/Palette.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/world/SceneState.hpp"
#include "cg/audio/LegacyAudio.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

#include <bit>
#include <cstdint>
#include <cstring>

namespace cg::world {

static std::int32_t DrawCapability4Base()
{
    return cg::render::StretchGraphicToScreen(cg::render::QueueOriginalPaletteGraphic_45F090(0, 0, 3, 99657, 0, -1));
}

static std::int32_t ProcessCapability4CommonTail(network::CgConnection& connection)
{
    static_cast<void>(DrawCapability4Base());
    static_cast<void>(
        cg::render::QueueOriginalPaletteGraphic_45F090(69, 350, 206, 246680, 0, -1));
    return cg::login::ProcessCapability4SavedSlotSelector_465430(connection);
}

// 原函数：0x00464840。
std::int32_t UpdateCapability4LoginScene_464840(network::CgConnection& connection)
{
    auto& sceneUi = connection.LoginSceneUi();
    const std::int32_t sampledState = connection.Owner().WorldScene().sceneState;
    const std::uint32_t state =
        std::bit_cast<std::uint32_t>(sampledState);
    if (state > 10U) {
        return sampledState;
    }

    switch (state) {
    case 0:
        input::SetContext_44B330(connection.Owner(), nullptr);
        sceneUi.capability4FrameCounter = 0;
        sceneUi.capability4IntroAlpha = 0;
        sceneUi.capability4IntroPhase = 0;
        connection.Owner().WorldScene().sceneState = 1;
        return cg::audio::LoadAndPlayBGM(connection.Owner(), 42);

    case 1: {
        const std::int32_t frameCounter = sceneUi.capability4FrameCounter;
        const std::int32_t phase = sceneUi.capability4IntroPhase;
        sceneUi.capability4FrameCounter = frameCounter + 1;

        std::int32_t result = phase;
        const std::uint32_t phaseBits =
            std::bit_cast<std::uint32_t>(phase);
        if (phaseBits <= 3U) {
            if (phaseBits == 0U) {
                const std::uint32_t fadeBits =
                    (std::bit_cast<std::uint32_t>(sceneUi.capability4IntroAlpha) << 24U)
                    + 0x00FFFFFFU;
                const std::int32_t fade =
                    std::bit_cast<std::int32_t>(fadeBits);
                static_cast<void>(
                    cg::render::StretchGraphicToScreen(cg::render::QueueOriginalPaletteGraphic_45F090(0, 0, 3, 99653, 0, fade)));
            } else if (phaseBits == 1U) {
                static_cast<void>(
                    cg::render::StretchGraphicToScreen(cg::render::QueueOriginalPaletteGraphic_45F090(0, 0, 3, 99653, 0, -1)));
                const std::uint32_t fadeBits =
                    (std::bit_cast<std::uint32_t>(sceneUi.capability4IntroAlpha) << 24U)
                    + 0x00FFFFFFU;
                const std::int32_t fade =
                    std::bit_cast<std::int32_t>(fadeBits);
                static_cast<void>(
                    cg::render::StretchGraphicToScreen(cg::render::QueueOriginalPaletteGraphic_45F090(0, 0, 4, 99654, 0, fade)));
            } else if (phaseBits == 2U) {
                static_cast<void>(
                    cg::render::StretchGraphicToScreen(cg::render::QueueOriginalPaletteGraphic_45F090(0, 0, 3, 99653, 0, -1)));
                static_cast<void>(
                    cg::render::StretchGraphicToScreen(cg::render::QueueOriginalPaletteGraphic_45F090(0, 0, 4, 99654, 0, -1)));
                const std::uint32_t fadeBits =
                    (std::bit_cast<std::uint32_t>(sceneUi.capability4IntroAlpha) << 24U)
                    + 0x00FFFFFFU;
                const std::int32_t fade =
                    std::bit_cast<std::int32_t>(fadeBits);
                static_cast<void>(
                    cg::render::StretchGraphicToScreen(cg::render::QueueOriginalPaletteGraphic_45F090(0, 0, 5, 99655, 0, fade)));
            } else {
                static_cast<void>(
                    cg::render::StretchGraphicToScreen(cg::render::QueueOriginalPaletteGraphic_45F090(0, 0, 3, 99653, 0, -1)));
                static_cast<void>(
                    cg::render::StretchGraphicToScreen(cg::render::QueueOriginalPaletteGraphic_45F090(0, 0, 4, 99654, 0, -1)));
                static_cast<void>(
                    cg::render::StretchGraphicToScreen(cg::render::QueueOriginalPaletteGraphic_45F090(0, 0, 5, 99655, 0, -1)));
                const std::uint32_t fadeBits =
                    (std::bit_cast<std::uint32_t>(sceneUi.capability4IntroAlpha) << 24U)
                    + 0x00FFFFFFU;
                const std::int32_t fade =
                    std::bit_cast<std::int32_t>(fadeBits);
                static_cast<void>(
                    cg::render::StretchGraphicToScreen(cg::render::QueueOriginalPaletteGraphic_45F090(0, 0, 6, 99656, 0, fade)));
            }

            sceneUi.capability4IntroAlpha = sceneUi.capability4IntroAlpha + 6;
            result = sceneUi.capability4IntroAlpha;
            if (result >= 255) {
                if (phaseBits <= 2U) {
                    sceneUi.capability4IntroPhase = phase + 1;
                    sceneUi.capability4IntroAlpha = 0;
                } else {
                    connection.Owner().WorldScene().sceneState = 2;
                }
            }
        }

        const std::uint8_t edgeButtons =
            static_cast<std::uint8_t>(platform::ReadSessionMouseEdgeButtons(connection.Owner()));
        if ((edgeButtons & 1U) != 0U) {
            connection.Owner().WorldScene().sceneState = 2;
        }
        return result;
    }

    case 2:
        sceneUi.capability4FrameCounter = 0;
        static_cast<void>(cg::login::detail::InitializeSavedState_533300(connection.Owner()));
        login::InitializeCapability4Input_465320(connection);
        connection.Owner().WorldScene().sceneState = 3;
        return DrawCapability4Base();

    case 3: {
        sceneUi.capability4FrameCounter = sceneUi.capability4FrameCounter + 1;
        if (sceneUi.capability4FrameCounter > 256) {
            sceneUi.capability4FrameCounter = 0;
        }

        const std::int32_t result =
            login::ProcessCapability4Input_464E30(connection);
        if (result == 1) {
            if (!login::SaveCapability4InputSlot(connection)) {
                return DrawCapability4Base();
            }
            login::detail::SaveCredentialRecordsWithoutPasswords_533730(connection.Owner());
            static_cast<void>(login::detail::PlaySe(connection.Owner(), 53, 320, 240));
            static_cast<void>(render::SelectSessionPalette(connection.Owner(), 30, 30));
            connection.Owner().WorldScene().sceneState = 4;
        } else if (result == 2) {
            static_cast<void>(login::detail::PlaySe(connection.Owner(), 54, 320, 240));
            static_cast<void>(::PostMessageW(
                reinterpret_cast<HWND>(platform::gLegacyWindow),
                0x10,
                0,
                0));
        } else if (result == -1) {
            connection.Owner().WorldScene().sceneState = 5;
        } else if (result == -2) {
            const std::int32_t selected =
                connection.Owner().SavedLogin().selectedSlot;
            if (selected == 0) {
                connection.Owner().WorldScene().sceneState = 5;
            } else {
                const std::int32_t newPhysical =
                    login::detail::ReadNewSavedPhysicalSlot105C368(connection.Owner());
                const std::int32_t mappedPhysical =
                    login::detail::ReadMappedSavedPhysicalSlot105C36C(
                        connection.Owner(),
                        selected);
                connection.Owner().WorldScene().sceneState =
                    newPhysical == mappedPhysical ? 5 : 7;
            }
        }
        return ProcessCapability4CommonTail(connection);
    }

    case 4: {
        const std::uint32_t previousShow = platform::gNativeCursorEnabled_D67AE4;
        static_cast<void>(login::detail::LoadSelectedLoginRecord_5336D0(connection.Owner()));
        login::detail::RestoreCapability4CurrentSavedRecord(connection.Owner());
        const std::uint32_t currentShow = platform::gNativeCursorEnabled_D67AE4;
        if (currentShow != previousShow) {
            static_cast<void>(::ShowCursor(
                std::bit_cast<std::int32_t>(currentShow)));
        }
        return world::SetCurrentSceneType(connection.Owner(), 2);
    }

    case 5: {
        login::ResetLoginMessage(connection);
        static_cast<void>(login::detail::PlaySe(connection.Owner(), 56, 320, 240));
        const std::int32_t result = DrawCapability4Base();
        connection.Owner().WorldScene().sceneState = 6;
        return result;
    }

    case 6:
        sceneUi.capability4Message = L"请设定ID、PASSWORD。" /* 原55CA8C「請設定ID、PASSWORD。」；20B正文，字面量隐式追加NUL。 */;
        if (login::UpdateLoginMessage(connection, sceneUi.capability4Message) != 0) {
            connection.Owner().WorldScene().sceneState = 2;
        }
        return ProcessCapability4CommonTail(connection);

    case 7: {
        login::detail::ResetCapability4Modal_464440(connection);
        static_cast<void>(login::detail::PlaySe(connection.Owner(), 56, 320, 240));
        const std::int32_t result = DrawCapability4Base();
        connection.Owner().WorldScene().sceneState = 8;
        return result;
    }

    case 8: {
        sceneUi.capability4Message = L"确定要删除这帐号记录吗？" /* 原55CE4C「確定要刪除這帳號記錄嗎？」；24B正文，字面量隐式追加NUL。 */;
        const std::int32_t result =
            login::detail::UpdateCapability4Modal_464450(connection, sceneUi.capability4Message);
        if (result == 1) {
            connection.Owner().WorldScene().sceneState = 9;
        } else if (result == 2) {
            connection.Owner().WorldScene().sceneState = 2;
        }
        return ProcessCapability4CommonTail(connection);
    }

    case 9: {
        const std::int32_t selected =
            connection.Owner().SavedLogin().selectedSlot;
        const std::int32_t physicalSlot =
            login::detail::ReadMappedSavedPhysicalSlot105C36C(connection.Owner(), selected);
        static_cast<void>(login::detail::DeleteSavedAccountFiles_533860(connection.Owner(), physicalSlot));
        login::ResetLoginMessage(connection);
        static_cast<void>(login::detail::PlaySe(connection.Owner(), 56, 320, 240));
        const std::int32_t result = DrawCapability4Base();
        connection.Owner().WorldScene().sceneState = 10;
        return result;
    }

    case 10:
        sceneUi.capability4Message = L"帐号记录已成功删除。" /* 原55CE34「帳號記錄已成功刪除。」；20B正文，字面量隐式追加NUL。 */ /* 原常量参考：0x00CEEDED。 */;
        if (login::UpdateLoginMessage(connection, sceneUi.capability4Message) != 0) {
            connection.Owner().WorldScene().sceneState = 2;
        }
        return ProcessCapability4CommonTail(connection);
    }

    return sampledState;
}

} // namespace cg::world
