#include "cg/CharacterDeletionRequest.hpp"

#include "CharacterDeletionRequestDependencies.hpp"

#include "cg/LoginRequest.hpp"
#include "cg/platform/MainLoop.hpp"

#include <cstddef>
#include <cstring>

namespace {

constexpr std::size_t kRegistNumberOffset = 0x4C;
constexpr std::uint32_t kRequestTimeoutMilliseconds = 600000;
constexpr char kRequestTimeoutMessage[] = "刪除角色逾時。";

static_assert(kRegistNumberOffset + sizeof(std::int32_t) <= cg::login::CharacterRecordSize);
static_assert(sizeof(kRequestTimeoutMessage) == 22);
static_assert(sizeof(kRequestTimeoutMessage) <= cg::login::RequestMessageCapacity);

} // namespace

namespace cg::login {

std::uint16_t gCharacterDeletionReplyCode = 0;

std::uint32_t StartCharacterDeletionRequest()
{
    const std::int16_t selected = gSelectedCharacterIndex;
    gCharacterDeletionReplyCode = 0;

    const std::uintptr_t socket = platform::gConnectionSocket;
    const CharacterRecordBytes* const record = gCharacterRecords.data() + selected;
    std::int32_t registNumber;
    std::memcpy(
        &registNumber,
        record->data() + kRegistNumberOffset,
        sizeof(registNumber));

    character_deletion_request_detail::Sub_512AB0(socket, registNumber);
    gRequestStatus = 1;
    const std::uint32_t tick = platform::ReadTickCount32();
    gRequestStartTick = tick;
    return tick;
}

std::int32_t PollCharacterDeletionRequest()
{
    if (gRequestStatus == 2) {
        const std::uint16_t replyCode = gCharacterDeletionReplyCode;
        gRequestStatus = 0;
        if (replyCode != 0) {
            return 1;
        }
        return -2;
    }

    const std::uint32_t now = platform::ReadTickCount32();
    const std::uint32_t elapsed = now - gRequestStartTick;
    if (elapsed <= kRequestTimeoutMilliseconds) {
        return 0;
    }

    std::memcpy(gRequestMessage, kRequestTimeoutMessage, sizeof(kRequestTimeoutMessage));
    return -1;
}

} // namespace cg::login
