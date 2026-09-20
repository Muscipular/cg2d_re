#include "cg/CharacterSelectScene.hpp"

#include "CharacterSelectSceneDependencies.hpp"
#include "CharacterSelectState.hpp"

#include "cg/LoginMessage.hpp"
#include "cg/LoginRequest.hpp"
#include "cg/Palette.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/world/SceneState.hpp"

#include <array>
#include <bit>
#include <cstddef>
#include <cstdint>
#include <cstring>

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

constexpr char kCharacterDataReadFailed[] = "角色資料讀取失敗。";
constexpr char kServerMaintenance[] = "目前伺服器正在停機維護中。";
constexpr char kDeleteQuestion[] = "真的要刪除嗎？";
constexpr char kDeleteWithControl[] = "想要刪除時，請按著 CTRL鍵不放";
constexpr char kClickYesAgain[] = "再點選『是』。        ";
constexpr char kCannotDeleteCharacter[] = "無法刪除此角色。";
constexpr char kUpdateQuestion[] = "要進行更新嗎？";
constexpr char kExpansionOnlyAccount[] =
    "這個帳號僅適用於資料片版本，請將用戶端的版本更新至資料片。";

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
    detail::NullSub_509080();
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
                login::detail::Sub_5081A0();
            } else if (result == -4) {
                world::gCurrentSceneState = 7;
                login::detail::Sub_5081A0();
            } else if (result < 0) {
                world::gCurrentSceneState = 4;
                login::detail::Sub_5081A0();
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
            detail::Sub_45D7D0();
            world::gCurrentSceneState = 8;
            break;
        case 8:
            if (detail::Sub_45D7E0() != 0) {
                world::SetCurrentSceneType(2);
            }
            break;
        case 9: {
            const std::int32_t capability = detail::Sub_45C970();
            render::gPaletteCapability = capability;
            const std::uint8_t pukVersion = state::gPukVersion;
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
            const std::int32_t result = detail::Sub_45E0C0(320, 240);
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
                detail::Sub_45E090(320, 240, kDeleteQuestion, 0);
            if (login::detail::IsYobiActionReady(gDeleteConfirmationAction, 1)) {
                login::detail::DrawText3(166, 326, 2, 4, kDeleteWithControl, 0);
                login::detail::DrawText3(166, 346, 2, 4, kClickYesAgain, 0);
                if (result == 1) {
                    if ((detail::gKeyStates[17] & 2u) != 0) {
                        world::gCurrentSceneState = 14;
                        login::detail::PlaySe(284, 320, 240);
                        if (gDeleteConfirmationAction != nullptr) {
                            login::detail::MarkDeath(gDeleteConfirmationAction);
                            gDeleteConfirmationAction = nullptr;
                        }
                        detail::Sub_45DBF0();
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
            const std::int32_t result = detail::Sub_45DC00();
            if (result == 1) {
                detail::Sub_51B3B0(static_cast<std::uint32_t>(
                    static_cast<std::int32_t>(login::gSelectedCharacterIndex)));
                detail::NullSub_509080();
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
                detail::Sub_45DEE0(320, 240, kUpdateQuestion, 0, 384, 96);
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
            detail::YobiClearUp_404D30();
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
            detail::YobiClearUp_404D30();
            ClearPreviewActions();
            break;
        case 2: {
            login::gSelectedCharacterIndex = static_cast<std::int16_t>(slot);
            if (ReadRecordInt32(record, kDeleteFlagOffset) != 0) {
                char name[kLegacyNameCapacity];
                std::strcpy(name, ReadRecordName(record));
                detail::NullSub_509080();
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
        detail::YobiClearUp_404D30();
        ClearPreviewActions();
        login::detail::Sub_5081A0();
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
