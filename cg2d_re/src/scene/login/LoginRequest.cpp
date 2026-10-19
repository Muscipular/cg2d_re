#include "cg/scene/login/LoginRequest.hpp"
#include "cg/core/m_log.hpp"
#include "cg/text/Encoding.hpp"

#include "LoginRequestDependencies.hpp"

#include "cg/audio/LegacyAudio.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/LegacyCryptInt.hpp"
#include "cg/platform/MainLoop.hpp"
#include "cg/world/SceneState.hpp"
#include "../../world/WorldUpdateDependencies.hpp"

#include <cstdio> // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。

namespace cg::login {

static constexpr std::uint32_t kRequestTimeoutMilliseconds = 600000;
static constexpr char kSuccessfulResult[] = "successful";

CharacterRecordArray gCharacterRecords {}; // 原全局：0x00FF03E0。
std::int16_t gSelectedCharacterIndex = 0; // 原全局：0x00FF0C08。

std::uint16_t gRequestReplyCode = 0; // 原全局：0x00DA3B2C。
std::int32_t gRequestStatus = 0; // 原全局：0x00DA3B34。
std::uint64_t gRequestStartTick = 0; // 原全局：0x00DA3B38。
std::uint8_t gRequestPollingEnabled = 0; // 原全局：0x00D9F2C9。
std::wstring gRequestMessage {}; // 原全局：0x00DA34DC。

static_assert(sizeof(gCharacterRecords) == 0x128);

// 原函数：0x0051B3B0。
std::int32_t ClearCharacterSlot_51B3B0(const std::uint32_t selectedIndex)
{
    if (selectedIndex >= CharacterRecordCount) {
        return -1;
    }
    gCharacterRecords[selectedIndex] = {};
    return 0;
}

// 原函数：0x0051B110。
std::int32_t HasCharacterInSlot_51B110(const std::uint32_t selectedIndex)
{
    if (selectedIndex >= CharacterRecordCount) {
        return -1;
    }
    return gCharacterRecords[selectedIndex].name[0] != '\0' ? 1 : 0;
}

// 原函数：0x0051B140。
std::int32_t CharacterNameExists_51B140(const char* const name)
{
    for (const CharacterRecord& record : gCharacterRecords) {
        const auto* const storedName =
            reinterpret_cast<const unsigned char*>(record.name.data());
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

// 原函数：0x00508AF0。
std::uint32_t StartLoginRequest()
{
    const std::int16_t selected = gSelectedCharacterIndex;
    gRequestReplyCode = 0;

    const CharacterRecord* const record = gCharacterRecords.data() + selected;
    const std::int32_t field18 = record->graphicNo;
    const std::int32_t field50 = record->previewState;
    const std::int32_t field4C = record->registNumber;

    [[maybe_unused]] const std::int32_t queued = network::Sub_512B00(
        platform::gConnectionSocket,
        field4C,
        field50,
        field18);
    m_log_i("CHAR LOGIN QUEUE RESULT=%d POLLING=%u\n", queued,
        static_cast<unsigned int>(gRequestPollingEnabled));
    gRequestStatus = 1;
    const std::uint64_t tick = platform::ReadTickCount64();
    gRequestStartTick = tick;
    return static_cast<std::uint32_t>(tick);
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
            return 1;
        }
        return -2;
    }

    const std::uint64_t now = platform::ReadTickCount64();
    const std::uint64_t elapsed = now - gRequestStartTick;
    if (elapsed <= kRequestTimeoutMilliseconds) {
        return 0;
    }

    static_cast<void>(gRequestMessage = L"处理登入逾时。" /* 0x55C9F8 原文：處理登入逾時。 */);
    return -1;
}

// 原 [0x508BC0,0x508C39)：状态先置 2，只有完整匹配才发送计时命令。
void HandleLoginResponse_508BC0(
    const std::uintptr_t socket,
    const char* const result,
    const char*)
{
    if (gRequestStatus != 1) {
        m_log_i("CHAR LOGIN REPLY IGNORED\n");
        return;
    }

    gRequestStatus = 2;
    std::size_t index = 0;
    for (;;) {
        const auto received = static_cast<unsigned char>(result[index]);
        const auto expected = static_cast<unsigned char>(kSuccessfulResult[index]);
        if (received != expected) {
            m_log_i("CHAR LOGIN REPLY FAILED\n");
            return;
        }
        if (received == 0) {
            break;
        }
        ++index;
    }

    gRequestReplyCode = 1;
    m_log_i("CHAR LOGIN REPLY SUCCESSFUL\n");
    const std::uint64_t tick = platform::ReadTickCount64();
    static_cast<void>(network::Sub_512DF0(socket, static_cast<std::uint32_t>(tick)));
}

} // namespace cg::login

namespace cg::login::detail {

// 原 0xDA3B2E，W16，独立于相邻的 0xDA3B2C 登录回复码。
std::uint16_t gReplyCodeDA3B2E = 0; // 原全局：0x00DA3B2E。

// 原 sub_508C40@[0x508C40,0x508C81)：登出请求启动。
std::uint32_t BeginLogoutRequest_508C40()
{
    const std::uintptr_t socket = platform::gConnectionSocket;
    gReplyCodeDA3B2E = 0;
    static_cast<void>(network::Sub_512BD0(socket));
    gRequestStatus = 1;
    const std::uint64_t tick = platform::ReadTickCount64();
    gRequestStartTick = tick;
    return static_cast<std::uint32_t>(tick);
}

// 原 sub_508C90@[0x508C90,0x508CF0)：登出回复及超时轮询。
std::int32_t PollLogoutReply_508C90()
{
    if (gRequestStatus == 2) {
        const std::uint16_t replyCode = gReplyCodeDA3B2E;
        gRequestStatus = 0;
        if (replyCode == 1) {
            return 1;
        }
        return replyCode == 2 ? 2 : -2;
    }

    const std::uint64_t now = platform::ReadTickCount64();
    const std::uint64_t start = gRequestStartTick;
    const std::uint64_t elapsed = now - start;
    if (elapsed <= kRequestTimeoutMilliseconds) {
        return 0;
    }

    static_cast<void>(gRequestMessage = L"处理登出逾时。" /* 0x55CA18 原文：處理登出逾時。 */);
    return -1;
}

// 原函数：0x00508CF0。
std::int32_t HandleLogoutReply_508CF0(
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
    world::gSceneTransitionValue = std::int32_t{0};
    static_cast<void>(world::SetPendingSceneType(7));
    return audio::PlaySe_52C450(60, 320);
}

} // namespace cg::login::detail
