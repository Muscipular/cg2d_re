#include "cg/network/CgConnection.hpp"
#include "cg/scene/login/CharacterDeletionRequest.hpp"
#include "cg/text/Encoding.hpp"

#include "cg/scene/login/LoginRequest.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/MainLoop.hpp"

#include <cstddef>
#include <cstdio> // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。

namespace cg::login {

static constexpr std::uint32_t kRequestTimeoutMilliseconds = 600000;

// 原函数：0x00508F70。
std::uint32_t StartCharacterDeletionRequest(network::CgConnection& connection)
{
    const std::int16_t selected = connection.LoginRequests().selectedCharacterIndex;
    auto& request = connection.LoginRequests();
    request.characterDeletionReplyCode = 0;
    const auto networkSession = connection.CaptureSession();
    const CharacterRecord* const record = request.characters.data() + selected;
    const std::int32_t registNumber = record->registNumber;

    connection.SendCharacterDeletionRequest(registNumber);
    request.status = 1;
    const std::uint64_t tick = platform::ReadTickCount64();
    request.startTick = tick;
    return static_cast<std::uint32_t>(tick);
}

// 原函数：0x00508FC0。
std::int32_t PollCharacterDeletionRequest(network::CgConnection& connection)
{
    auto& request = connection.LoginRequests();
    if (request.status == 2) {
        const std::uint16_t replyCode = request.characterDeletionReplyCode;
        request.status = 0;
        if (replyCode != 0) {
            return 1;
        }
        return -2;
    }

    const std::uint64_t now = platform::ReadTickCount64();
    const std::uint64_t elapsed = now - request.startTick;
    if (elapsed <= kRequestTimeoutMilliseconds) {
        return 0;
    }

    static_cast<void>(request.message = L"删除角色逾时。" /* 原 sub_508FC0 超时文案“刪除角色逾時。”的原字节，正文 14B，含隐式 NUL 共 15B。 */);
    return -1;
}

// 原函数：0x00509020。
void HandleCharacterDeletionResponse_509020(cg::network::CgConnection& connection,
    const char* const result,
    const char* const)
{
    auto& request = connection.LoginRequests();
    if (request.status != 1) {
        return;
    }

    request.status = 2;

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

    request.characterDeletionReplyCode = 1;
}

} // namespace cg::login
