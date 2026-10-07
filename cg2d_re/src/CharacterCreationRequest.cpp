#include "cg/CharacterCreationRequest.hpp"

#include "CharacterSelectState.hpp"

#include "cg/LoginRequest.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "platform/LegacyFormat.hpp"

#include <cstddef>
#include <cstring>

namespace {

constexpr std::size_t kRegistNumberOffset = 0x4C;
constexpr std::size_t kPreviewToggleOffset = 0x50;
constexpr std::uint32_t kRequestTimeoutMilliseconds = 600000;
// 原 0x58AFC4：新建角色逾時。正文 14 字节，末尾唯一 NUL。
constexpr char kRequestTimeoutMessage[] =
    "\xB7\x73\xAB\xD8\xA8\xA4\xA6\xE2\xB9\x4F\xAE\xC9\xA1\x43";
constexpr char kSuccessfulResult[] = "successful";

static_assert(kRegistNumberOffset + sizeof(std::int32_t) <= cg::login::CharacterRecordSize);
static_assert(kPreviewToggleOffset + sizeof(std::int32_t) <= cg::login::CharacterRecordSize);
static_assert(sizeof(kRequestTimeoutMessage) == 15);
static_assert(sizeof(kRequestTimeoutMessage) <= cg::login::RequestMessageCapacity);
static_assert(sizeof(kSuccessfulResult) == 11);

} // namespace

namespace cg::login {

std::uint16_t gCharacterCreationReplyCode = 0;

std::uint32_t StartCharacterCreationRequest()
{
    const std::int32_t element3 = character_select::state::gValueDE0B04;
    const std::int32_t element2 = character_select::state::gValueF633F4;
    const std::int32_t element1 = character_select::state::gValueFF03D0;
    const std::int32_t element0 = character_select::state::gValueF7C948;
    const std::int32_t attribute4 = character_select::state::gValueF7CA00;
    const std::int32_t attribute3 = character_select::state::gValueDE0AF8;
    const std::int32_t attribute2 = character_select::state::gValueDE03F4;
    const std::int32_t attribute1 = character_select::state::gValueF633F0;
    const std::int32_t attribute0 = character_select::state::gValueF633EC;
    const std::int32_t graphic = character_select::state::gSelectedGraphicNo;
    const std::int32_t motion = character_select::state::gValueDE0AFC;
    const std::int32_t selected = gSelectedCharacterIndex;
    const std::uintptr_t socket = platform::gConnectionSocket;
    gCharacterCreationReplyCode = 0;
    network::Sub_5128C0(
        socket, selected, character_select::state::gCreationNameFF0D18.data(),
        motion, graphic, attribute0, attribute1, attribute2, attribute3, attribute4,
        element0, element1, element2, element3);
    gRequestStatus = 1;
    const std::uint32_t tick = platform::ReadTickCount32();
    gRequestStartTick = tick;
    return tick;
}

std::int32_t PollCharacterCreationRequest()
{
    if (gRequestStatus == 2) {
        const std::uint16_t replyCode = gCharacterCreationReplyCode;
        gRequestStatus = 0;
        return replyCode == 1 ? 1 : -2;
    }

    const std::uint32_t elapsed =
        platform::ReadTickCount32() - gRequestStartTick;
    if (elapsed <= kRequestTimeoutMilliseconds) {
        return 0;
    }

    static_cast<void>(platform::FormatLegacyString_53B304(
        gRequestMessage, kRequestTimeoutMessage));
    return -1;
}

void HandleCharacterCreationResponse_508EE0(
    const std::uintptr_t,
    const char* const result,
    const std::int32_t registNumber,
    const char* const)
{
    if (gRequestStatus != 1) {
        return;
    }

    gRequestStatus = 2;
    std::size_t index = 0;
    for (;;) {
        const auto received = static_cast<unsigned char>(result[index]);
        const auto expected = static_cast<unsigned char>(kSuccessfulResult[index]);
        if (received != expected) {
            return;
        }
        if (received == 0) {
            break;
        }
        ++index;
    }

    const std::int16_t selected = gSelectedCharacterIndex;
    gCharacterCreationReplyCode = 1;
    const std::int32_t previewToggle =
        character_select::state::gPreviewToggleState;
    CharacterRecordBytes* const record = gCharacterRecords.data() + selected;
    std::memcpy(
        record->data() + kRegistNumberOffset,
        &registNumber,
        sizeof(registNumber));
    std::memcpy(
        record->data() + kPreviewToggleOffset,
        &previewToggle,
        sizeof(previewToggle));
}

} // namespace cg::login
