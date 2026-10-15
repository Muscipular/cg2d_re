#include "cg/CharacterDeletionRequest.hpp"
#include "cg/text/Encoding.hpp"

#include "cg/LoginRequest.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "platform/LegacyFormat.hpp"

#include <cstddef>
#include <cstring>
#include <cstdio> // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。

namespace {

constexpr std::size_t kRegistNumberOffset = 0x4C;
constexpr std::uint32_t kRequestTimeoutMilliseconds = 600000;
static_assert(kRegistNumberOffset + sizeof(std::int32_t) <= cg::login::CharacterRecordSize);

} // namespace

namespace cg::login {

std::uint16_t gCharacterDeletionReplyCode = 0; // 原全局：0x00DA3B28。

// 原函数：0x00508F70。
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

    network::Sub_512AB0(socket, registNumber);
    gRequestStatus = 1;
    const std::uint32_t tick = platform::ReadTickCount32();
    gRequestStartTick = tick;
    return tick;
}

// 原函数：0x00508FC0。
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

    static_cast<void>(gRequestMessage = L"刪除角色逾時。" /* 原 sub_508FC0 超时文案“刪除角色逾時。”的原字节，正文 14B，含隐式 NUL 共 15B。 */);
    return -1;
}

// 原函数：0x00509020。
void HandleCharacterDeletionResponse_509020(
    const std::uintptr_t,
    const char* const result,
    const char* const)
{
    if (gRequestStatus != 1) {
        return;
    }

    gRequestStatus = 2;

    std::size_t index = 0;
    for (;;) {
        const auto received = static_cast<unsigned char>(result[index]);
        const auto expected = static_cast<unsigned char>("successful"[index]);
        if (received != expected) {
            return;
        }
        if (received == 0) {
            break;
        }
        ++index;
    }

    gCharacterDeletionReplyCode = 1;
}

} // namespace cg::login
