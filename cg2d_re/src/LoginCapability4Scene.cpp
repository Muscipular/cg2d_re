#include "world/WorldUpdateDependencies.hpp"

#include "LoginCapability4Modal.hpp"
#include "LoginInputDependencies.hpp"
#include "LoginSceneDependencies.hpp"

#include "cg/Input.hpp"
#include "cg/LoginInput.hpp"
#include "cg/LoginMessage.hpp"
#include "cg/Palette.hpp"
#include "cg/platform/LegacyWindow.hpp"
#include "cg/platform/Startup.hpp"
#include "cg/render/GraphicPendingQueue.hpp"
#include "cg/world/SceneState.hpp"

#include <bit>
#include <cstdint>
#include <cstring>

namespace {

constexpr char kCapability4CredentialsPrompt[] =
    "請設定ID、PASSWORD。";
constexpr char kCapability4DeletePrompt[] =
    "確定要刪除這帳號記錄嗎？";
constexpr char kCapability4DeleteSucceeded[] =
    "帳號記錄已成功刪除。";

// sub_464840 的五个唯一私有 owner；原映像均为 virtual-only 零填充。
std::int32_t gValue9A4F2C = 0;
std::int32_t gValue9A4D00 = 0;
std::int32_t gValue9A4B7C = 0;
std::int32_t gValue9A509C = 0;
char gValue9A4A78[256] {};

static_assert(
    sizeof(kCapability4CredentialsPrompt) <= sizeof(gValue9A4A78));
static_assert(sizeof(kCapability4DeletePrompt) <= sizeof(gValue9A4A78));
static_assert(sizeof(kCapability4DeleteSucceeded) <= sizeof(gValue9A4A78));

std::int32_t AddW32(
    const std::int32_t left,
    const std::int32_t right) noexcept
{
    return std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(left)
        + std::bit_cast<std::uint32_t>(right));
}

std::int32_t DrawCapability4Base()
{
    return cg::render::Sub_45F090(0, 0, 3, 99657, 0, -1);
}

std::int32_t ProcessCapability4CommonTail()
{
    static_cast<void>(DrawCapability4Base());
    static_cast<void>(
        cg::render::Sub_45F090(69, 350, 206, 246680, 0, -1));
    return cg::login::ProcessCapability4SavedSlotSelector_465430();
}

} // namespace

namespace cg::world::detail {

std::int32_t Sub_464840()
{
    const std::uint32_t state =
        std::bit_cast<std::uint32_t>(world::gCurrentSceneState);
    if (state > 10U) {
        return world::gCurrentSceneState;
    }

    switch (state) {
    case 0:
        input::SetContext_44B330(nullptr);
        gValue9A4F2C = 0;
        gValue9A4D00 = 0;
        gValue9A4B7C = 0;
        world::gCurrentSceneState = 1;
        return login::detail::LoadAndPlayBGM(42);

    case 1: {
        const std::int32_t phase = gValue9A4B7C;
        gValue9A4F2C = AddW32(gValue9A4F2C, 1);

        std::int32_t result = phase;
        const std::uint32_t phaseBits =
            std::bit_cast<std::uint32_t>(phase);
        if (phaseBits <= 3U) {
            const std::uint32_t fadeBits =
                (std::bit_cast<std::uint32_t>(gValue9A4D00) << 24U)
                + 0x00FFFFFFU;
            const std::int32_t fade =
                std::bit_cast<std::int32_t>(fadeBits);

            if (phaseBits == 0U) {
                static_cast<void>(
                    render::Sub_45F090(0, 0, 3, 99653, 0, fade));
            } else if (phaseBits == 1U) {
                static_cast<void>(
                    render::Sub_45F090(0, 0, 3, 99653, 0, -1));
                static_cast<void>(
                    render::Sub_45F090(0, 0, 4, 99654, 0, fade));
            } else if (phaseBits == 2U) {
                static_cast<void>(
                    render::Sub_45F090(0, 0, 3, 99653, 0, -1));
                static_cast<void>(
                    render::Sub_45F090(0, 0, 4, 99654, 0, -1));
                static_cast<void>(
                    render::Sub_45F090(0, 0, 5, 99655, 0, fade));
            } else {
                static_cast<void>(
                    render::Sub_45F090(0, 0, 3, 99653, 0, -1));
                static_cast<void>(
                    render::Sub_45F090(0, 0, 4, 99654, 0, -1));
                static_cast<void>(
                    render::Sub_45F090(0, 0, 5, 99655, 0, -1));
                static_cast<void>(
                    render::Sub_45F090(0, 0, 6, 99656, 0, fade));
            }

            gValue9A4D00 = AddW32(gValue9A4D00, 6);
            result = gValue9A4D00;
            if (result >= 255) {
                if (phaseBits <= 2U) {
                    gValue9A4B7C = AddW32(phase, 1);
                    gValue9A4D00 = 0;
                } else {
                    world::gCurrentSceneState = 2;
                }
            }
        }

        const std::uint8_t edgeButtons =
            static_cast<std::uint8_t>(login::detail::gValueD68D9C);
        if ((edgeButtons & 1U) != 0U) {
            world::gCurrentSceneState = 2;
        }
        return result;
    }

    case 2:
        gValue9A4F2C = 0;
        gValue9A509C = 0;
        static_cast<void>(platform::InitializeSavedState_533300());
        login::InitializeCapability4Input_465320();
        world::gCurrentSceneState = 3;
        return DrawCapability4Base();

    case 3: {
        gValue9A4F2C = AddW32(gValue9A4F2C, 1);
        if (gValue9A4F2C > 256) {
            gValue9A4F2C = 0;
        }

        const std::int32_t result =
            login::ProcessCapability4Input_464E30();
        if (result == 1) {
            login::detail::Sub_533570(
                login::gInputContext9A4D20.text,
                login::detail::gSelectedSavedSlot105C394);
            login::detail::Sub_5335B0(
                login::gInputContext9A5960.text,
                login::detail::gSelectedSavedSlot105C394);
            login::detail::Sub_533640(
                login::gInputContext9A5678.text,
                login::detail::gSelectedSavedSlot105C394);
            login::detail::Sub_533730();
            static_cast<void>(login::detail::PlaySe(53, 320, 240));
            static_cast<void>(render::SelectPalette(30, 30));
            world::gCurrentSceneState = 4;
        } else if (result == 2) {
            static_cast<void>(login::detail::PlaySe(54, 320, 240));
            static_cast<void>(platform::detail::PostMessageA_54D2F0(
                platform::gLegacyWindow,
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
            static_cast<void>(platform::ShowLegacyCursor(
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
        std::memcpy(
            gValue9A4A78,
            kCapability4CredentialsPrompt,
            sizeof(kCapability4CredentialsPrompt));
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
        std::memcpy(
            gValue9A4A78,
            kCapability4DeletePrompt,
            sizeof(kCapability4DeletePrompt));
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
        std::memcpy(
            gValue9A4A78,
            kCapability4DeleteSucceeded,
            sizeof(kCapability4DeleteSucceeded));
        if (login::UpdateLoginMessage(gValue9A4A78) != 0) {
            world::gCurrentSceneState = 2;
        }
        return ProcessCapability4CommonTail();
    }

    return world::gCurrentSceneState;
}

} // namespace cg::world::detail
