#include "cg/CharacterListRequest.hpp"

#include "CharacterListRequestDependencies.hpp"

#include "cg/LoginRequest.hpp"
#include "cg/platform/MainLoop.hpp"

#include <cstddef>
#include <cstring>

namespace {

constexpr std::uint32_t kRequestTimeoutMilliseconds = 600000;
constexpr char kRequestTimeoutMessage[] = "取得角色列表逾時。";

static_assert(cg::login::CharacterRecordCount == 2);
static_assert(sizeof(cg::login::CharacterRecordBytes) == 0x94);
static_assert(sizeof(cg::login::CharacterRecordArray) == 2 * 0x94);
static_assert(sizeof(kRequestTimeoutMessage) == 28);
static_assert(sizeof(kRequestTimeoutMessage) <= cg::login::RequestMessageCapacity);

} // namespace

namespace cg::login {

std::uint16_t gCharacterListReplyCode = 0;

std::uint32_t StartCharacterListRequest()
{
    gCharacterRecords[0].fill(std::byte{0});
    gCharacterRecords[1].fill(std::byte{0});

    const std::uintptr_t socket = platform::gConnectionSocket;
    gCharacterListReplyCode = 0;
    character_list_request_detail::Sub_512B90(socket);
    gRequestStatus = 1;
    const std::uint32_t tick = platform::ReadTickCount32();
    gRequestStartTick = tick;
    return tick;
}

std::int32_t PollCharacterListRequest()
{
    if (gRequestStatus == 2) {
        const std::uint16_t replyCode = gCharacterListReplyCode;
        gRequestStatus = 0;
        if (replyCode == 1) {
            return 1;
        }
        if (replyCode == 2) {
            return -3;
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
