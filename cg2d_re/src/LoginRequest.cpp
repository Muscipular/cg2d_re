#include "cg/LoginRequest.hpp"

#include "LoginRequestDependencies.hpp"
#include "world/WorldUpdateDependencies.hpp"

#include "cg/LegacyAudio.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/platform/MainLoop.hpp"
#include "platform/LegacyFormat.hpp"
#include "cg/world/SceneState.hpp"

#include <cstring>
#include <cstdio> // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。

namespace {

constexpr std::size_t kRequestField18Offset = 0x18;
constexpr std::size_t kRequestField4COffset = 0x4C;
constexpr std::size_t kRequestField50Offset = 0x50;
constexpr std::uint32_t kRequestTimeoutMilliseconds = 600000;
// 0x55C9F8 原文：處理登入逾時。
constexpr char kRequestTimeoutMessage[] = "\xCC\x8E\xC0\xED\xB5\xC7\xC8\xEB\xD3\xE2\x95\x72\xA1\xA3";
// 0x55CA18 原文：處理登出逾時。
constexpr char kLogoutTimeoutMessage[] = "\xCC\x8E\xC0\xED\xB5\xC7\xB3\xF6\xD3\xE2\x95\x72\xA1\xA3";
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

CharacterRecordArray gCharacterRecords {}; // 原全局：0x00FF03E0。
std::int16_t gSelectedCharacterIndex = 0; // 原全局：0x00FF0C08。

std::uint16_t gRequestReplyCode = 0; // 原全局：0x00DA3B2C。
std::int32_t gRequestStatus = 0; // 原全局：0x00DA3B34。
std::uint32_t gRequestStartTick = 0; // 原全局：0x00DA3B38。
std::uint8_t gRequestPollingEnabled = 0; // 原全局：0x00D9F2C9。
std::int32_t gRequestOutcomeFlag = 0; // 原全局：0x00DA3B30。
char gRequestMessage[RequestMessageCapacity] {}; // 原全局：0x00DA34DC。

static_assert(sizeof(gCharacterRecords) == 0x128);
static_assert(sizeof(gRequestMessage) == RequestMessageCapacity);

// 原函数：0x0051B3B0。
std::int32_t Sub_51B3B0(const std::uint32_t selectedIndex)
{
    if (selectedIndex >= CharacterRecordCount) {
        return -1;
    }
    gCharacterRecords[selectedIndex].fill(std::byte{0});
    return 0;
}

// 原函数：0x0051B110。
std::int32_t Sub_51B110(const std::uint32_t selectedIndex)
{
    if (selectedIndex >= CharacterRecordCount) {
        return -1;
    }
    return gCharacterRecords[selectedIndex][0] != std::byte{0} ? 1 : 0;
}

// 原函数：0x0051B140。
std::int32_t CharacterNameExists_51B140(const char* const name)
{
    for (const CharacterRecordBytes& record : gCharacterRecords) {
        const auto* const storedName =
            reinterpret_cast<const unsigned char*>(record.data());
        std::size_t index = 0;
        for (;;) {
            const auto inputByte = static_cast<unsigned char>(name[index]);
            const unsigned char storedByte = storedName[index];
            if (inputByte != storedByte) {
                break;
            }
            if (inputByte == 0) {
                return 1;
            }
            ++index;
        }
    }
    return 0;
}

// 原函数：0x00510140。
std::int32_t ReadRequestOutcomeFlag()
{
    return gRequestOutcomeFlag;
}

// 原函数：0x00508AF0。
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

// 原函数：0x00508B50。
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

    static_cast<void>(std::sprintf(
        gRequestMessage, kRequestTimeoutMessage));
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

    gRequestReplyCode = 1;
    gRequestOutcomeFlag = 1;
    const std::uint32_t tick = platform::ReadTickCount32();
    static_cast<void>(network::Sub_512DF0(socket, tick));
}

} // namespace cg::login

namespace cg::login::detail {

// 原 0xDA3B2E，W16，独立于相邻的 0xDA3B2C 登录回复码。
std::uint16_t gReplyCodeDA3B2E = 0; // 原全局：0x00DA3B2E。

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

    const std::uint32_t now = platform::ReadTickCount32();
    const std::uint32_t start = gRequestStartTick;
    const std::uint32_t elapsed = now - start;
    if (elapsed <= kRequestTimeoutMilliseconds) {
        return 0;
    }

    static_cast<void>(std::sprintf(
        gRequestMessage, kLogoutTimeoutMessage));
    return -1;
}

// 原函数：0x00508CF0。
std::int32_t Sub_508CF0(
    const std::uintptr_t, const char* const result, const char*)
{
    const bool waitingForReply = gRequestStatus == 1;
    if (waitingForReply) {
        gRequestStatus = 2;
    }

    // 原两路均按 result、常量逐字节读取；不匹配返回原无符号比较的 -1/1。
    std::size_t index = 0;
    for (;;) {
        const auto received = static_cast<unsigned char>(result[index]);
        const auto expected = static_cast<unsigned char>(kSuccessfulResult[index]);
        if (received != expected) {
            return received < expected ? -1 : 1;
        }
        if (received == 0) {
            break;
        }
        ++index;
    }
    if (waitingForReply) {
        gReplyCodeDA3B2E = 1;
        return 0;
    }

    gRequestStatus = 2;
    gReplyCodeDA3B2E = 2;
    // 原先取齐 +8/+4/+C，再按相同顺序发布；目的虚表与保留字节不写。
    // 现由整数赋值统一载荷发布：一次RNG，保留目标token与reserved。
    *world::detail::BorrowValue73ECD0Storage() = std::int32_t{0};
    static_cast<void>(world::SetPendingSceneType(7));
    return audio::PlaySe_52C450(60, 320);
}

} // namespace cg::login::detail
