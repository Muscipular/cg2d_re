#include "cg/CharacterSelectScene.hpp"

#include "CharacterSelectSceneDependencies.hpp"
#include "CharacterSelectState.hpp"

#include "cg/GraphicInfo.hpp"
#include "cg/Input.hpp"
#include "cg/LoginMessage.hpp"
#include "cg/LoginRequest.hpp"
#include "cg/Palette.hpp"
#include "cg/YobiRuntime.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/KeyState.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/world/SceneState.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

namespace cg::character_select::detail {

void Sub_462870()
{
    login::detail::YobiClearUp_404D30();
    world::gCurrentSceneState = std::bit_cast<std::int32_t>(
        std::bit_cast<std::uint32_t>(world::gCurrentSceneState) + 1U);
}

std::int32_t Sub_464290(const std::int32_t* const rectangle)
{
    const std::int32_t top = rectangle[1];
    const std::int32_t left = rectangle[0];
    const std::int32_t height = rectangle[3];
    const std::int32_t width = rectangle[2];
    return login::detail::Sub_5049A0(
        left, top, left + width, top + height, -1);
}

char gText9A4EAC[128] {};

} // namespace cg::character_select::detail

namespace cg::character_select::state {

std::uint8_t gPukVersion = 0;
std::array<login::YobiAction*, 2> gPreviewActions {};
login::YobiAction* gCharacterModalAction = nullptr;
std::int16_t gPendingCharacterCount = 0;
std::int32_t gSelectedGraphicNo = 0;
std::uint16_t gCharacterEntryState = 0;
std::int32_t gPreviewToggleState = 0;
char gSelectedCharacterName[256] {};
char gDeleteCharacterName[132] {};

static_assert(sizeof(gPreviewActions) == 2 * sizeof(login::YobiAction*));
static_assert(sizeof(gSelectedCharacterName) == 256);
static_assert(sizeof(gDeleteCharacterName) == 132);

} // namespace cg::character_select::state

namespace cg::character_select {
namespace {

// 原 0x55CD68: 角色资料读取失败提示。
constexpr char kCharacterDataReadFailed[] =
    "\xA8\xA4\xA6\xE2\xB8\xEA\xAE\xC6\xC5\xAA\xA8\xFA\xA5\xA2\xB1\xD1\xA1\x43";
// 原 0x55CD4C: 服务器维护提示。
constexpr char kServerMaintenance[] =
    "\xA5\xD8\xAB\x65\xA6\xF8\xAA\x41\xBE\xB9\xA5\xBF\xA6\x62\xB0\xB1"
    "\xBE\xF7\xBA\xFB\xC5\x40\xA4\xA4\xA1\x43";
// 原 0x55C854: 删除角色确认提示。
constexpr char kDeleteQuestion[] =
    "\xAF\x75\xAA\xBA\xAD\x6E\xA7\x52\xB0\xA3\xB6\xDC\xA1\x48";
// 原 0x55CD94: 按住 CTRL 删除角色提示。
constexpr char kDeleteWithControl[] =
    "\xB7\x51\xAD\x6E\xA7\x52\xB0\xA3\xAE\xC9\xA1\x41\xBD\xD0\xAB\xF6"
    "\xB5\xDB\x20\x43\x54\x52\x4C\xC1\xE4\xA4\xA3\xA9\xF1";
// 原 0x55CD7C: 再次点击“是”的提示，保留八个尾空格。
constexpr char kClickYesAgain[] =
    "\xA6\x41\xC2\x49\xBF\xEF\xA1\x79\xAC\x4F\xA1\x7A\xA1\x43"
    "\x20\x20\x20\x20\x20\x20\x20\x20";
// 原 0x55CD38: 无法删除角色提示。
constexpr char kCannotDeleteCharacter[] =
    "\xB5\x4C\xAA\x6B\xA7\x52\xB0\xA3\xA6\xB9\xA8\xA4\xA6\xE2\xA1\x43";
// 原 0x55CD28: 更新确认提示。
constexpr char kUpdateQuestion[] =
    "\xAD\x6E\xB6\x69\xA6\xE6\xA7\xF3\xB7\x73\xB6\xDC\xA1\x48";
// 原 0x55C818: 资料片专用账号及客户端更新提示。
constexpr char kExpansionOnlyAccount[] =
    "\xB3\x6F\xAD\xD3\xB1\x62\xB8\xB9\xB6\xC8\xBE\x41\xA5\xCE\xA9\xF3"
    "\xB8\xEA\xAE\xC6\xA4\xF9\xAA\xA9\xA5\xBB\xA1\x41\xBD\xD0\xB1\x4E"
    "\xA5\xCE\xA4\xE1\xBA\xDD\xAA\xBA\xAA\xA9\xA5\xBB\xA7\xF3\xB7\x73"
    "\xA6\xDC\xB8\xEA\xAE\xC6\xA4\xF9\xA1\x43";

static_assert(sizeof(kCharacterDataReadFailed) == 19);
static_assert(sizeof(kServerMaintenance) == 27);
static_assert(sizeof(kDeleteQuestion) == 15);
static_assert(sizeof(kDeleteWithControl) == 30);
static_assert(sizeof(kClickYesAgain) == 23);
static_assert(sizeof(kCannotDeleteCharacter) == 17);
static_assert(sizeof(kUpdateQuestion) == 15);
static_assert(sizeof(kExpansionOnlyAccount) == 59);

constexpr std::size_t kNameOffset = 0x00;
constexpr std::size_t kGraphicNoOffset = 0x18;
constexpr std::size_t kDeleteFlagOffset = 0x20;
constexpr std::size_t kGraphicClassOffset = 0x24;
constexpr std::size_t kPreviewToggleOffset = 0x50;
constexpr std::size_t kLegacyNameCapacity = 20;

static_assert(kNameOffset == 0);
static_assert(kGraphicNoOffset + sizeof(std::int32_t) <= login::CharacterRecordSize);
static_assert(kDeleteFlagOffset + sizeof(std::int32_t) <= login::CharacterRecordSize);
static_assert(kGraphicClassOffset + sizeof(std::int32_t) <= login::CharacterRecordSize);
static_assert(kPreviewToggleOffset + sizeof(std::int32_t) <= login::CharacterRecordSize);

detail::CharacterSelectUiRecord gReturnUi {
    {534, 438, 64, 16, 245045, 245047, 245046},
};
detail::CharacterSelectUiRecord gUpdateUi {
    {296, 433, 49, 17, 244542, 244544, 244543},
};

// 原 0x9A6EAC 与 0x9A6EB0 只被本控制器直接持有。
std::int32_t gUpdateAvailable = 0;
login::YobiAction* gDeleteConfirmationAction = nullptr;

std::uint32_t UnsignedBits(std::int32_t value)
{
    return std::bit_cast<std::uint32_t>(value);
}

std::int32_t ReadRecordInt32(
    const login::CharacterRecordBytes& record,
    std::size_t offset)
{
    std::int32_t value;
    std::memcpy(&value, record.data() + offset, sizeof(value));
    return value;
}

void WriteRecordInt32(
    login::CharacterRecordBytes& record,
    std::size_t offset,
    std::int32_t value)
{
    std::memcpy(record.data() + offset, &value, sizeof(value));
}

const char* ReadRecordName(const login::CharacterRecordBytes& record)
{
    // 与原 strcpy 相同，调用前提是 +0x00 处存在适合 20 字节临时区的 NUL 文本。
    return reinterpret_cast<const char*>(record.data() + kNameOffset);
}

void CopyRecordName(char* destination, const login::CharacterRecordBytes& record)
{
    char name[kLegacyNameCapacity];
    std::strcpy(name, ReadRecordName(record));
    cg::NullSub_509080();
    std::strcpy(destination, name);
}

void ClearPreviewActions()
{
    if (state::gPreviewActions[0] != nullptr) {
        state::gPreviewActions[0] = nullptr;
    }
    if (state::gPreviewActions[1] != nullptr) {
        state::gPreviewActions[1] = nullptr;
    }
}

void UpdateCharacterGraphic(
    login::CharacterRecordBytes& record,
    std::int32_t action)
{
    const std::uint8_t pukVersion = state::gPukVersion;
    const std::int32_t imageNo = ReadRecordInt32(record, kGraphicNoOffset);

    if (pukVersion >= 7u) {
        if (imageNo == 33100 || imageNo == 33101) {
            return;
        }

        if (imageNo >= 30000 && imageNo < 98000) {
            WriteRecordInt32(record, kGraphicClassOffset, 0);
        } else if (imageNo >= 202800 && imageNo < 204200) {
            WriteRecordInt32(record, kGraphicClassOffset, 1);
        } else if (imageNo >= 200000 && imageNo < 202700) {
            WriteRecordInt32(record, kGraphicClassOffset, 1);
        } else if ((imageNo >= 240000 && imageNo <= 242800)
            || (imageNo >= 205000 && imageNo <= 205600)
            || (imageNo >= 270000 && imageNo <= 270200)) {
            WriteRecordInt32(record, kGraphicClassOffset, 2);
        }

        const std::int32_t graphicClass =
            ReadRecordInt32(record, kGraphicClassOffset);
        const std::uint32_t selectorBits =
            UnsignedBits(graphicClass) + static_cast<std::uint32_t>(2 * action - 7);
        const std::int32_t selector = std::bit_cast<std::int32_t>(selectorBits);

        if (graphicClass == 0) {
            if (selector == 1) {
                WriteRecordInt32(
                    record,
                    kGraphicNoOffset,
                    detail::Sub_51BB80(imageNo));
            } else {
                const std::int32_t first = detail::Sub_51BB80(imageNo);
                WriteRecordInt32(record, kGraphicNoOffset, first);
                WriteRecordInt32(
                    record,
                    kGraphicNoOffset,
                    detail::Sub_51BFF0(first));
            }
        } else if (graphicClass == 1) {
            if (selector == 2) {
                WriteRecordInt32(
                    record,
                    kGraphicNoOffset,
                    detail::Sub_51BFF0(imageNo));
            } else if (selector == 0) {
                WriteRecordInt32(
                    record,
                    kGraphicNoOffset,
                    imageNo >= 200000 && imageNo < 202700
                        ? detail::Sub_51BFF0(imageNo)
                        : detail::Sub_51BAC0(imageNo));
            }
        } else if (graphicClass == 2) {
            if (selector == 3 && imageNo >= 240000 && imageNo < 241400) {
                const std::int32_t first = detail::Sub_51BF00(imageNo);
                WriteRecordInt32(record, kGraphicNoOffset, first);
                WriteRecordInt32(
                    record,
                    kGraphicNoOffset,
                    detail::Sub_51BAC0(first));
            } else {
                WriteRecordInt32(
                    record,
                    kGraphicNoOffset,
                    detail::Sub_51BF00(imageNo));
            }
        }
        return;
    }

    if (pukVersion < 5u || imageNo == 33100 || imageNo == 33101) {
        return;
    }

    if (imageNo >= 30000 && imageNo < 98000) {
        WriteRecordInt32(record, kGraphicClassOffset, 0);
    } else if (imageNo >= 202800 && imageNo < 204200) {
        WriteRecordInt32(record, kGraphicClassOffset, 1);
    }

    const std::int32_t graphicClass = ReadRecordInt32(record, kGraphicClassOffset);
    if (graphicClass == 0) {
        WriteRecordInt32(
            record,
            kGraphicNoOffset,
            detail::Sub_51BB80(imageNo));
    } else if (graphicClass == 1) {
        WriteRecordInt32(
            record,
            kGraphicNoOffset,
            detail::Sub_51BAC0(imageNo));
    }
}

} // namespace

std::int32_t ProcessScene_460FF0()
{
    std::int32_t interactive = 1;

    const std::uint32_t mainState = UnsignedBits(world::gCurrentSceneState);
    if (mainState <= 21u) {
        switch (mainState) {
        case 0:
            world::gCurrentSceneState = 1;
            state::gPukVersion = 0;
            ResetCharacterListProgress_45DD50();
            gUpdateAvailable = 0;
            render::SelectPalette(0, 0);
            break;
        case 1: {
            const std::int32_t result = UpdateCharacterListProgress_45DD60();
            if (result == 1) {
                world::gCurrentSceneState = 9;
            } else if (result == -3) {
                world::gCurrentSceneState = 2;
                cg::network::ShutdownNetworkRuntime_5081A0();
            } else if (result == -4) {
                world::gCurrentSceneState = 7;
                cg::network::ShutdownNetworkRuntime_5081A0();
            } else if (result < 0) {
                world::gCurrentSceneState = 4;
                cg::network::ShutdownNetworkRuntime_5081A0();
            }
            break;
        }
        case 2:
            login::ResetLoginMessage();
            world::gCurrentSceneState = 6;
            break;
        case 3:
            break;
        case 4:
            login::ResetLoginMessage();
            world::gCurrentSceneState = 5;
            break;
        case 5:
            if (login::UpdateLoginMessage(kCharacterDataReadFailed) != 0) {
                world::SetCurrentSceneType(2);
            }
            break;
        case 6:
            if (login::UpdateLoginMessage(kServerMaintenance) != 0) {
                world::SetCurrentSceneType(2);
            }
            break;
        case 7:
            ResetAuthenticationFailure_45D7D0();
            world::gCurrentSceneState = 8;
            break;
        case 8:
            if (UpdateAuthenticationFailure_45D7E0() != 0) {
                world::SetCurrentSceneType(2);
            }
            break;
        case 9: {
            const std::int32_t capability = resource::ProbePaletteCapability_45C970();
            const std::uint8_t pukVersion = state::gPukVersion;
            render::gPaletteCapability = capability;
            if (pukVersion == 1u) {
                world::gCurrentSceneState = 10;
                break;
            }
            if ((capability < 2 && pukVersion >= 5u)
                || (capability < 1 && pukVersion >= 3u)
                || (capability < 3 && pukVersion >= 7u)
                || (capability < 4 && pukVersion >= 8u)) {
                world::gCurrentSceneState = 19;
                break;
            }

            if (capability == 1) {
                if (pukVersion == 0u || pukVersion == 2u) {
                    gUpdateAvailable = 1;
                }
            } else if (capability >= 2
                && (pukVersion == 0u || pukVersion == 2u)) {
                gUpdateAvailable = 1;
            } else if (capability >= 3
                && pukVersion >= 5u
                && pukVersion <= 6u) {
                gUpdateAvailable = 1;
            } else if (capability == 4 && pukVersion == 7u) {
                gUpdateAvailable = 1;
            }
            world::gCurrentSceneState = 10;
            break;
        }
        case 10:
            world::gCurrentSceneState = 11;
            ProcessCharacterPreview_460D00(0);
            ProcessCharacterPreview_460D00(1);
            break;
        case 11:
            interactive = 0;
            break;
        case 12: {
            const std::int32_t result =
                ProcessCharacterDeletionWarning_45E0C0(320, 240);
            if (result == 1) {
                world::gCurrentSceneState = 13;
                gDeleteConfirmationAction =
                    login::detail::Sub_5044E0(130, 300, 340, 52, 4, 1);
            } else if (result == 2) {
                world::gCurrentSceneState = 10;
            }
            break;
        }
        case 13: {
            const std::int32_t result =
                ProcessDefaultConfirmationDialog_45E090(
                    320,
                    240,
                    kDeleteQuestion,
                    0);
            if (login::detail::IsYobiActionReady(gDeleteConfirmationAction, 1)) {
                login::detail::DrawText3(166, 326, 2, 4, kDeleteWithControl, 0);
                login::detail::DrawText3(166, 346, 2, 4, kClickYesAgain, 0);
                if (result == 1) {
                    if ((platform::gKeyStates[17] & 2u) != 0) {
                        world::gCurrentSceneState = 14;
                        login::detail::PlaySe(284, 320, 240);
                        if (gDeleteConfirmationAction != nullptr) {
                            login::detail::MarkDeath(gDeleteConfirmationAction);
                            gDeleteConfirmationAction = nullptr;
                        }
                        ResetCharacterDeletionProgress_45DBF0();
                        world::gCurrentSceneState = 14;
                    }
                } else if (result == 2) {
                    world::gCurrentSceneState = 10;
                    if (gDeleteConfirmationAction != nullptr) {
                        login::detail::MarkDeath(gDeleteConfirmationAction);
                        gDeleteConfirmationAction = nullptr;
                    }
                }
            }
            break;
        }
        case 14: {
            const std::int32_t result = UpdateCharacterDeletionProgress_45DC00();
            if (result == 1) {
                detail::Sub_51B3B0(static_cast<std::uint32_t>(
                    static_cast<std::int32_t>(login::gSelectedCharacterIndex)));
                cg::NullSub_509080();
                detail::Sub_520200();
                if (state::gPendingCharacterCount > 0) {
                    --state::gPendingCharacterCount;
                }
                world::gCurrentSceneState = 10;
            } else if (result == -2) {
                world::gCurrentSceneState = 15;
            }
            break;
        }
        case 15:
            login::ResetLoginMessage();
            world::gCurrentSceneState = 16;
            break;
        case 16:
            if (login::UpdateLoginMessage(kCannotDeleteCharacter) != 0) {
                world::gCurrentSceneState = 0;
            }
            break;
        case 17:
            login::ResetLoginMessage();
            world::gCurrentSceneState = 18;
            break;
        case 18: {
            const std::int32_t result =
                ProcessConfirmationDialog_45DEE0(
                    320,
                    240,
                    kUpdateQuestion,
                    0,
                    384,
                    96);
            if (result == 1) {
                world::SetCurrentSceneType(13);
            } else if (result == 2) {
                world::gCurrentSceneState = 10;
            }
            break;
        }
        case 19:
            login::ResetLoginMessage();
            world::gCurrentSceneState = 20;
            break;
        case 20:
            if (login::UpdateLoginMessage(kExpansionOnlyAccount) != 0) {
                world::SetCurrentSceneType(2);
            }
            break;
        case 21:
            if (ProcessCharacterModal_462090(320, 240) == 1) {
                world::gCurrentSceneState = 10;
            }
            break;
        default:
            break;
        }
    }

    const std::uint32_t secondaryState = UnsignedBits(world::gCurrentSceneState);
    if (secondaryState <= 20u
        && (secondaryState <= 9u || secondaryState >= 19u)) {
        return detail::Sub_45F090(0, 0, 3, 245060, 0, -1);
    }

    for (std::uint32_t slot = 0; slot < login::CharacterRecordCount; ++slot) {
        login::CharacterRecordBytes& record =
            login::gCharacterRecords[static_cast<std::size_t>(slot)];
        const std::int32_t action =
            ProcessCharacterSlot_461B30(slot, interactive);
        const std::uint32_t actionIndex = UnsignedBits(action);
        if (actionIndex > 6u) {
            continue;
        }

        switch (actionIndex) {
        case 0:
            login::gSelectedCharacterIndex = static_cast<std::int16_t>(slot);
            detail::Sub_518BE0();
            detail::Sub_519290();
            detail::Sub_5194C0();
            detail::Sub_519300();
            detail::Sub_519550();
            world::SetCurrentSceneType(4);
            login::detail::PlaySe(53, 320, 240);
            login::detail::YobiClearUp_404D30();
            ClearPreviewActions();
            break;
        case 1:
            login::gSelectedCharacterIndex = static_cast<std::int16_t>(slot);
            CopyRecordName(state::gSelectedCharacterName, record);
            state::gSelectedGraphicNo = ReadRecordInt32(record, kGraphicNoOffset);
            world::SetCurrentSceneType(5);
            login::detail::PlaySe(59, 320, 240);
            state::gCharacterEntryState = 0;
            detail::Sub_52B990();
            login::detail::YobiClearUp_404D30();
            ClearPreviewActions();
            break;
        case 2: {
            login::gSelectedCharacterIndex = static_cast<std::int16_t>(slot);
            if (ReadRecordInt32(record, kDeleteFlagOffset) != 0) {
                char name[kLegacyNameCapacity];
                std::strcpy(name, ReadRecordName(record));
                cg::NullSub_509080();
                world::gCurrentSceneState = 12;
                std::strcpy(state::gDeleteCharacterName, name);
            } else {
                world::gCurrentSceneState = 21;
            }
            login::detail::PlaySe(55, 320, 240);
            break;
        }
        case 3:
        case 4:
            UpdateCharacterGraphic(record, static_cast<std::int32_t>(actionIndex));
            login::detail::PlaySe(51, 320, 240);
            break;
        case 5:
        case 6: {
            const std::int32_t value = ReadRecordInt32(record, kPreviewToggleOffset);
            if (value == 0) {
                WriteRecordInt32(record, kPreviewToggleOffset, 1);
            } else if (value == 1) {
                WriteRecordInt32(record, kPreviewToggleOffset, 0);
            }
            state::gPreviewToggleState =
                ReadRecordInt32(record, kPreviewToggleOffset);
            ProcessCharacterPreview_460D00(slot);
            login::detail::PlaySe(51, 320, 240);
            break;
        }
        default:
            break;
        }
    }

    std::int32_t uiStyle = interactive;
    if (gUpdateAvailable != 0) {
        if (uiStyle == 0
            && world::gCurrentSceneState == 11
            && (detail::Sub_45F100(&gUpdateUi) & 1) != 0) {
            world::SetCurrentSceneType(13);
            uiStyle = 1;
        }
        detail::Sub_45EFD0(&gUpdateUi, 5, 0, 0, uiStyle);
        detail::Sub_45F090(252, 392, 4, 245078, 0, -1);
    }

    if (uiStyle == 0
        && world::gCurrentSceneState == 11
        && (detail::Sub_45F100(&gReturnUi) & 1) != 0) {
        login::detail::YobiClearUp_404D30();
        ClearPreviewActions();
        cg::network::ShutdownNetworkRuntime_5081A0();
        world::SetCurrentSceneType(2);
        login::detail::PlaySe(54, 320, 240);
    }
    detail::Sub_45EFD0(&gReturnUi, 5, 0, 0, uiStyle);
    detail::Sub_45F090(0, 0, 3, 245060, 0, -1);

    std::int32_t result = gUpdateAvailable;
    if (gUpdateAvailable != 0) {
        const std::uint8_t pukVersion = state::gPukVersion;
        result = static_cast<std::int32_t>(pukVersion);
        switch (pukVersion) {
        case 0:
        case 2:
            result = detail::Sub_512D30(
                platform::gConnectionSocket, 2, state::kVersion20);
            break;
        case 5:
        case 6:
            result = detail::Sub_512D30(
                platform::gConnectionSocket, 4, state::kVersion30);
            break;
        case 7:
            result = detail::Sub_512D30(
                platform::gConnectionSocket, 5, detail::gVersionPayload9A5C10);
            break;
        default:
            break;
        }
        world::gCurrentSceneState = 0;
    }
    return result;
}

} // namespace cg::character_select
