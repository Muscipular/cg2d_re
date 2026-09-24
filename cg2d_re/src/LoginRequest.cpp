#include "cg/LoginRequest.hpp"

#include "LoginRequestDependencies.hpp"

#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/MainLoop.hpp"

#include <cstring>

namespace {

constexpr std::size_t kRequestField18Offset = 0x18;
constexpr std::size_t kRequestField4COffset = 0x4C;
constexpr std::size_t kRequestField50Offset = 0x50;
constexpr std::uint32_t kRequestTimeoutMilliseconds = 600000;
// 0x55C9F8 原文：處理登入逾時。
constexpr char kRequestTimeoutMessage[] = "\xB3\x42\xB2\x7A\xB5\x6E\xA4\x4A\xB9\x4F\xAE\xC9\xA1\x43";
// 0x55CA18 原文：處理登出逾時。
constexpr char kLogoutTimeoutMessage[] = "\xB3\x42\xB2\x7A\xB5\x6E\xA5\x58\xB9\x4F\xAE\xC9\xA1\x43";
constexpr char kSuccessfulResult[] = "successful";

static_assert(kRequestField18Offset + sizeof(std::int32_t) <= cg::login::CharacterRecordSize);
static_assert(kRequestField4COffset + sizeof(std::int32_t) <= cg::login::CharacterRecordSize);
static_assert(kRequestField50Offset + sizeof(std::int32_t) <= cg::login::CharacterRecordSize);
static_assert(sizeof(kRequestTimeoutMessage) == 15);
static_assert(sizeof(kLogoutTimeoutMessage) == 15);
static_assert(sizeof(kRequestTimeoutMessage) <= cg::login::RequestMessageCapacity);
static_assert(sizeof(kLogoutTimeoutMessage) <= cg::login::RequestMessageCapacity);
static_assert(sizeof(kSuccessfulResult) == 11);

} // namespace

namespace cg::login {

CharacterRecordArray gCharacterRecords {};
std::int16_t gSelectedCharacterIndex = 0;

std::uint16_t gRequestReplyCode = 0;
std::int32_t gRequestStatus = 0;
std::uint32_t gRequestStartTick = 0;
std::uint8_t gRequestPollingEnabled = 0;
std::int32_t gRequestOutcomeFlag = 0;
char gRequestMessage[RequestMessageCapacity] {};

static_assert(sizeof(gCharacterRecords) == 0x128);
static_assert(sizeof(gRequestMessage) == RequestMessageCapacity);

std::int32_t ReadRequestOutcomeFlag()
{
    return gRequestOutcomeFlag;
}

std::uint32_t StartLoginRequest()
{
    const std::int16_t selected = gSelectedCharacterIndex;
    gRequestReplyCode = 0;

    const CharacterRecordBytes* const record = gCharacterRecords.data() + selected;
    std::int32_t field18;
    std::int32_t field50;
    std::int32_t field4C;
    std::memcpy(&field18, record->data() + kRequestField18Offset, sizeof(field18));
    std::memcpy(&field50, record->data() + kRequestField50Offset, sizeof(field50));
    std::memcpy(&field4C, record->data() + kRequestField4COffset, sizeof(field4C));

    network::Sub_512B00(
        platform::gConnectionSocket,
        field4C,
        field50,
        field18);
    gRequestStatus = 1;
    const std::uint32_t tick = platform::ReadTickCount32();
    gRequestStartTick = tick;
    return tick;
}

std::int32_t PollLoginRequest()
{
    const std::uint8_t pollingEnabled = gRequestPollingEnabled;
    if (pollingEnabled == 0) {
        return 0;
    }

    if (gRequestStatus == 2) {
        gRequestStatus = 0;
        if (gRequestReplyCode == 1) {
            gRequestOutcomeFlag = 1;
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

// 原 [0x508BC0,0x508C39)：状态先置 2，只有完整匹配才发送计时命令。
void HandleLoginResponse_508BC0(
    const std::uintptr_t socket,
    const char* const result,
    const char*)
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

    gRequestReplyCode = 1;
    gRequestOutcomeFlag = 1;
    const std::uint32_t tick = platform::ReadTickCount32();
    static_cast<void>(network::Sub_512DF0(socket, tick));
}

} // namespace cg::login

namespace cg::login::detail {

// 原 0xDA3B2E，W16，独立于相邻的 0xDA3B2C 登录回复码。
std::uint16_t gReplyCodeDA3B2E = 0;

// 原 sub_508C40@[0x508C40,0x508C81)：登出请求启动。
std::uint32_t Sub_508C40()
{
    const std::uintptr_t socket = platform::gConnectionSocket;
    gReplyCodeDA3B2E = 0;
    static_cast<void>(network::Sub_512BD0(socket));
    network::WriteValueD9F2D0(0);
    gRequestOutcomeFlag = 0;
    gRequestStatus = 1;
    const std::uint32_t tick = platform::ReadTickCount32();
    gRequestStartTick = tick;
    return tick;
}

// 原 sub_508C90@[0x508C90,0x508CF0)：登出回复及超时轮询。
std::int32_t Sub_508C90()
{
    if (gRequestStatus == 2) {
        const std::uint16_t replyCode = gReplyCodeDA3B2E;
        gRequestStatus = 0;
        if (replyCode == 1) {
            return 1;
        }
        return replyCode == 2 ? 2 : -2;
    }

    const std::uint32_t elapsed =
        platform::ReadTickCount32() - gRequestStartTick;
    if (elapsed <= kRequestTimeoutMilliseconds) {
        return 0;
    }

    std::memcpy(gRequestMessage, kLogoutTimeoutMessage,
        sizeof(kLogoutTimeoutMessage));
    return -1;
}

} // namespace cg::login::detail
