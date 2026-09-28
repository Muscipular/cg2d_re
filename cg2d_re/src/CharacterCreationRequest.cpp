#include "cg/CharacterCreationRequest.hpp"

#include "CharacterSelectState.hpp"

#include "cg/LoginRequest.hpp"
#include "cg/platform/MainLoop.hpp"

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

    std::memcpy(
        gRequestMessage,
        kRequestTimeoutMessage,
        sizeof(kRequestTimeoutMessage));
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
        const char expected = kSuccessfulResult[index];
        if (result[index] != expected) {
            return;
        }
        if (expected == '\0') {
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
