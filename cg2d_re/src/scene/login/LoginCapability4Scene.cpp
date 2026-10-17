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

// sub_464840 的五个唯一私有 owner；原映像均为 virtual-only 零填充。
static std::int32_t gValue9A4F2C = 0; // 原全局：0x009A4F2C。
static std::int32_t gValue9A4D00 = 0; // 原全局：0x009A4D00。
static std::int32_t gValue9A4B7C = 0; // 原全局：0x009A4B7C。
static std::int32_t gValue9A509C = 0; // 原全局：0x009A509C。
static std::wstring gValue9A4A78 {}; // 原全局：0x009A4A78。

static std::int32_t DrawCapability4Base()
{
    return cg::render::StretchGraphicToScreen(cg::render::Sub_45F090(0, 0, 3, 99657, 0, -1));
}

static std::int32_t ProcessCapability4CommonTail()
{
    static_cast<void>(DrawCapability4Base());
    static_cast<void>(
        cg::render::Sub_45F090(69, 350, 206, 246680, 0, -1));
    return cg::login::ProcessCapability4SavedSlotSelector_465430();
}

// 原函数：0x00464840。
std::int32_t Sub_464840()
{
    const std::int32_t sampledState = world::gCurrentSceneState;
    const std::uint32_t state =
        std::bit_cast<std::uint32_t>(sampledState);
    if (state > 10U) {
        return sampledState;
    }

    switch (state) {
    case 0:
        input::SetContext_44B330(nullptr);
        gValue9A4F2C = 0;
        gValue9A4D00 = 0;
        gValue9A4B7C = 0;
        world::gCurrentSceneState = 1;
        return cg::audio::LoadAndPlayBGM(42);

    case 1: {
        const std::int32_t frameCounter = gValue9A4F2C;
        const std::int32_t phase = gValue9A4B7C;
        gValue9A4F2C = frameCounter + 1;

        std::int32_t result = phase;
        const std::uint32_t phaseBits =
            std::bit_cast<std::uint32_t>(phase);
        if (phaseBits <= 3U) {
            if (phaseBits == 0U) {
                const std::uint32_t fadeBits =
                    (std::bit_cast<std::uint32_t>(gValue9A4D00) << 24U)
                    + 0x00FFFFFFU;
                const std::int32_t fade =
                    std::bit_cast<std::int32_t>(fadeBits);
                static_cast<void>(
                    cg::render::StretchGraphicToScreen(cg::render::Sub_45F090(0, 0, 3, 99653, 0, fade)));
            } else if (phaseBits == 1U) {
                static_cast<void>(
                    cg::render::StretchGraphicToScreen(cg::render::Sub_45F090(0, 0, 3, 99653, 0, -1)));
                const std::uint32_t fadeBits =
                    (std::bit_cast<std::uint32_t>(gValue9A4D00) << 24U)
                    + 0x00FFFFFFU;
                const std::int32_t fade =
                    std::bit_cast<std::int32_t>(fadeBits);
                static_cast<void>(
                    cg::render::StretchGraphicToScreen(cg::render::Sub_45F090(0, 0, 4, 99654, 0, fade)));
            } else if (phaseBits == 2U) {
                static_cast<void>(
                    cg::render::StretchGraphicToScreen(cg::render::Sub_45F090(0, 0, 3, 99653, 0, -1)));
                static_cast<void>(
                    cg::render::StretchGraphicToScreen(cg::render::Sub_45F090(0, 0, 4, 99654, 0, -1)));
                const std::uint32_t fadeBits =
                    (std::bit_cast<std::uint32_t>(gValue9A4D00) << 24U)
                    + 0x00FFFFFFU;
                const std::int32_t fade =
                    std::bit_cast<std::int32_t>(fadeBits);
                static_cast<void>(
                    cg::render::StretchGraphicToScreen(cg::render::Sub_45F090(0, 0, 5, 99655, 0, fade)));
            } else {
                static_cast<void>(
                    cg::render::StretchGraphicToScreen(cg::render::Sub_45F090(0, 0, 3, 99653, 0, -1)));
                static_cast<void>(
                    cg::render::StretchGraphicToScreen(cg::render::Sub_45F090(0, 0, 4, 99654, 0, -1)));
                static_cast<void>(
                    cg::render::StretchGraphicToScreen(cg::render::Sub_45F090(0, 0, 5, 99655, 0, -1)));
                const std::uint32_t fadeBits =
                    (std::bit_cast<std::uint32_t>(gValue9A4D00) << 24U)
                    + 0x00FFFFFFU;
                const std::int32_t fade =
                    std::bit_cast<std::int32_t>(fadeBits);
                static_cast<void>(
                    cg::render::StretchGraphicToScreen(cg::render::Sub_45F090(0, 0, 6, 99656, 0, fade)));
            }

            gValue9A4D00 = gValue9A4D00 + 6;
            result = gValue9A4D00;
            if (result >= 255) {
                if (phaseBits <= 2U) {
                    gValue9A4B7C = phase + 1;
                    gValue9A4D00 = 0;
                } else {
                    world::gCurrentSceneState = 2;
                }
            }
        }

        const std::uint8_t edgeButtons =
            static_cast<std::uint8_t>(cg::platform::gLegacyMouseState.edgeButtons);
        if ((edgeButtons & 1U) != 0U) {
            world::gCurrentSceneState = 2;
        }
        return result;
    }

    case 2:
        gValue9A4F2C = 0;
        gValue9A509C = 0;
        static_cast<void>(cg::login::detail::InitializeSavedState_533300());
        login::InitializeCapability4Input_465320();
        world::gCurrentSceneState = 3;
        return DrawCapability4Base();

    case 3: {
        gValue9A4F2C = gValue9A4F2C + 1;
        if (gValue9A4F2C > 256) {
            gValue9A4F2C = 0;
        }

        const std::int32_t result =
            login::ProcessCapability4Input_464E30();
        if (result == 1) {
            if (!login::SaveCapability4InputSlot()) {
                return DrawCapability4Base();
            }
            login::detail::Sub_533730();
            static_cast<void>(login::detail::PlaySe(53, 320, 240));
            static_cast<void>(render::SelectPalette(30, 30));
            world::gCurrentSceneState = 4;
        } else if (result == 2) {
            static_cast<void>(login::detail::PlaySe(54, 320, 240));
            static_cast<void>(::PostMessageW(
                reinterpret_cast<HWND>(platform::gLegacyWindow),
                0x10,
                0,
                0));
        } else if (result == -1) {
            world::gCurrentSceneState = 5;
        } else if (result == -2) {
            const std::int32_t selected =
                login::detail::gSelectedSavedSlot105C394;
            if (selected == 0) {
                world::gCurrentSceneState = 5;
            } else {
                const std::int32_t newPhysical =
                    login::detail::ReadNewSavedPhysicalSlot105C368();
                const std::int32_t mappedPhysical =
                    login::detail::ReadMappedSavedPhysicalSlot105C36C(
                        selected);
                world::gCurrentSceneState =
                    newPhysical == mappedPhysical ? 5 : 7;
            }
        }
        return ProcessCapability4CommonTail();
    }

    case 4: {
        const std::uint32_t previousShow = platform::gValueD67AE4;
        static_cast<void>(login::detail::Sub_5336D0());
        login::detail::RestoreCapability4CurrentSavedRecord();
        const std::uint32_t currentShow = platform::gValueD67AE4;
        if (currentShow != previousShow) {
            static_cast<void>(::ShowCursor(
                std::bit_cast<std::int32_t>(currentShow)));
        }
        return world::SetCurrentSceneType(2);
    }

    case 5: {
        login::ResetLoginMessage();
        static_cast<void>(login::detail::PlaySe(56, 320, 240));
        const std::int32_t result = DrawCapability4Base();
        world::gCurrentSceneState = 6;
        return result;
    }

    case 6:
        gValue9A4A78 = L"请设定ID、PASSWORD。" /* 原55CA8C「請設定ID、PASSWORD。」；20B正文，字面量隐式追加NUL。 */;
        if (login::UpdateLoginMessage(gValue9A4A78) != 0) {
            world::gCurrentSceneState = 2;
        }
        return ProcessCapability4CommonTail();

    case 7: {
        login::detail::ResetCapability4Modal_464440();
        static_cast<void>(login::detail::PlaySe(56, 320, 240));
        const std::int32_t result = DrawCapability4Base();
        world::gCurrentSceneState = 8;
        return result;
    }

    case 8: {
        gValue9A4A78 = L"确定要删除这帐号记录吗？" /* 原55CE4C「確定要刪除這帳號記錄嗎？」；24B正文，字面量隐式追加NUL。 */;
        const std::int32_t result =
            login::detail::UpdateCapability4Modal_464450(gValue9A4A78);
        if (result == 1) {
            world::gCurrentSceneState = 9;
        } else if (result == 2) {
            world::gCurrentSceneState = 2;
        }
        return ProcessCapability4CommonTail();
    }

    case 9: {
        const std::int32_t selected =
            login::detail::gSelectedSavedSlot105C394;
        const std::int32_t physicalSlot =
            login::detail::ReadMappedSavedPhysicalSlot105C36C(selected);
        static_cast<void>(login::detail::Sub_533860(physicalSlot));
        login::ResetLoginMessage();
        static_cast<void>(login::detail::PlaySe(56, 320, 240));
        const std::int32_t result = DrawCapability4Base();
        world::gCurrentSceneState = 10;
        return result;
    }

    case 10:
        gValue9A4A78 = L"帐号记录已成功删除。" /* 原55CE34「帳號記錄已成功刪除。」；20B正文，字面量隐式追加NUL。 */ /* 原常量参考：0x00CEEDED。 */;
        if (login::UpdateLoginMessage(gValue9A4A78) != 0) {
            world::gCurrentSceneState = 2;
        }
        return ProcessCapability4CommonTail();
    }

    return sampledState;
}

} // namespace cg::world
