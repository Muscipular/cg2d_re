#include "cg/network/CgConnection.hpp"
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

// 原函数：0x0051B3B0。
std::int32_t ClearCharacterSlot_51B3B0(network::CgConnection& connection, const std::uint32_t selectedIndex)
{
    if (selectedIndex >= CharacterRecordCount) {
        return -1;
    }
    connection.LoginRequests().characters[selectedIndex] = {};
    return 0;
}

// 原函数：0x0051B110。
std::int32_t HasCharacterInSlot_51B110(network::CgConnection& connection, const std::uint32_t selectedIndex)
{
    if (selectedIndex >= CharacterRecordCount) {
        return -1;
    }
    return connection.LoginRequests().characters[selectedIndex].name[0] != '\0' ? 1 : 0;
}

// 原函数：0x0051B140。
std::int32_t CharacterNameExists_51B140(network::CgConnection& connection, const std::wstring_view name)
{
    for (const CharacterRecord& record : connection.LoginRequests().characters) {
        if (record.name == name) { return 1; }
    }
    return 0;
}

// 原函数：0x00508AF0。
std::uint32_t StartLoginRequest(network::CgConnection& connection)
{
    auto& request = connection.LoginRequests();
    const std::int16_t selected = request.selectedCharacterIndex;
    request.loginReplyCode = 0;

    const CharacterRecord* const record = request.characters.data() + selected;
    const std::int32_t field18 = record->graphicNo;
    const std::int32_t field50 = record->previewState;
    const std::int32_t field4C = record->registNumber;

    [[maybe_unused]] const auto queued = connection.SendCharacterLoginRequest(field4C,
        field50,
        field18);
    m_log_iw(L"CHAR LOGIN QUEUE RESULT=%d POLLING=%u\n", static_cast<std::int32_t>(queued),
        static_cast<unsigned int>(connection.IoEnabled()));
    request.status = 1;
    const std::uint64_t tick = platform::ReadTickCount64();
    request.startTick = tick;
    return static_cast<std::uint32_t>(tick);
}

// 原函数：0x00508B50。
std::int32_t PollLoginRequest(network::CgConnection& connection)
{
    auto& request = connection.LoginRequests();
    const std::uint8_t pollingEnabled = connection.IoEnabled();
    if (pollingEnabled == 0) {
        return 0;
    }

    if (request.status == 2) {
        request.status = 0;
        if (request.loginReplyCode == 1) {
            return 1;
        }
        return -2;
    }

    const std::uint64_t now = platform::ReadTickCount64();
    const std::uint64_t elapsed = now - request.startTick;
    if (elapsed <= kRequestTimeoutMilliseconds) {
        return 0;
    }

    static_cast<void>(request.message = L"处理登入逾时。" /* 0x55C9F8 原文：處理登入逾時。 */);
    return -1;
}

// 原 [0x508BC0,0x508C39)：状态先置 2，只有完整匹配才发送计时命令。
void HandleLoginResponse_508BC0(cg::network::CgConnection& networkConnection,
    const char* const result,
    const char*)
{
    auto& request = networkConnection.LoginRequests();
    if (request.status != 1) {
        m_log_iw(L"CHAR LOGIN REPLY IGNORED\n");
        return;
    }

    request.status = 2;
    std::size_t index = 0;
    for (;;) {
        const auto received = static_cast<unsigned char>(result[index]);
        const auto expected = static_cast<unsigned char>(kSuccessfulResult[index]);
        if (received != expected) {
            m_log_iw(L"CHAR LOGIN REPLY FAILED\n");
            return;
        }
        if (received == 0) {
            break;
        }
        ++index;
    }

    request.loginReplyCode = 1;
    m_log_iw(L"CHAR LOGIN REPLY SUCCESSFUL\n");
    const std::uint64_t tick = platform::ReadTickCount64();
    static_cast<void>(networkConnection.SendLoginTimestamp(static_cast<std::uint32_t>(tick)));
}

} // namespace cg::login

namespace cg::login::detail {

// 原 sub_508C40@[0x508C40,0x508C81)：登出请求启动。
std::uint32_t BeginLogoutRequest_508C40(cg::network::CgConnection& connection)
{
    auto& request = connection.LoginRequests();
    const auto networkSession = connection.CaptureSession();
    request.logoutReplyCode = 0;
    static_cast<void>(connection.SendLogoutRequest());
    request.status = 1;
    const std::uint64_t tick = platform::ReadTickCount64();
    request.startTick = tick;
    return static_cast<std::uint32_t>(tick);
}

// 原 sub_508C90@[0x508C90,0x508CF0)：登出回复及超时轮询。
std::int32_t PollLogoutReply_508C90(cg::network::CgConnection& connection)
{
    auto& request = connection.LoginRequests();
    if (request.status == 2) {
        const std::uint16_t replyCode = request.logoutReplyCode;
        request.status = 0;
        if (replyCode == 1) {
            return 1;
        }
        return replyCode == 2 ? 2 : -2;
    }

    const std::uint64_t now = platform::ReadTickCount64();
    const std::uint64_t start = request.startTick;
    const std::uint64_t elapsed = now - start;
    if (elapsed <= kRequestTimeoutMilliseconds) {
        return 0;
    }

    static_cast<void>(request.message = L"处理登出逾时。" /* 0x55CA18 原文：處理登出逾時。 */);
    return -1;
}

// 原函数：0x00508CF0。
std::int32_t HandleLogoutReply_508CF0(cg::network::CgConnection& connection, const char* const result, const char*)
{
    auto& request = connection.LoginRequests();
    const bool waitingForReply = request.status == 1;
    if (waitingForReply) {
        request.status = 2;
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
        request.logoutReplyCode = 1;
        return 0;
    }

    request.status = 2;
    request.logoutReplyCode = 2;
    // 原先取齐 +8/+4/+C，再按相同顺序发布；目的虚表与保留字节不写。
    // 现由整数赋值统一载荷发布：一次RNG，保留目标token与reserved。
    connection.Owner().WorldScene().map.crypt->sceneTransitionValue = std::int32_t{0};
    static_cast<void>(world::SetPendingSceneType(connection.Owner(), 7));
    return audio::PlaySe_52C450(60, 320);
}

} // namespace cg::login::detail
