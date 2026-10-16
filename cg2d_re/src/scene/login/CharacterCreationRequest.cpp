#include "cg/scene/login/CharacterCreationRequest.hpp"
#include "cg/text/Encoding.hpp"

#include "../character_select/CharacterSelectState.hpp"

#include "cg/scene/login/LoginRequest.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/MainLoop.hpp"

#include <cstddef>
#include <cstdio> // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。

namespace {

constexpr std::uint32_t kRequestTimeoutMilliseconds = 600000;

} // namespace

namespace cg::login {

std::uint16_t gCharacterCreationReplyCode = 0; // 原全局：0x00DA3B2A。

// 原函数：0x00508E00。
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
        socket, selected, cg::text::FromGbkWide(character_select::state::gCreationNameFF0D18.data()),
        motion, graphic, attribute0, attribute1, attribute2, attribute3, attribute4,
        element0, element1, element2, element3);
    gRequestStatus = 1;
    const std::uint32_t tick = platform::ReadTickCount32();
    gRequestStartTick = tick;
    return tick;
}

// 原函数：0x00508E90。
std::int32_t PollCharacterCreationRequest()
{
    if (gRequestStatus == 2) {
        const std::uint16_t replyCode = gCharacterCreationReplyCode;
        gRequestStatus = 0;
        return replyCode == 1 ? 1 : -2;
    }

    const std::uint32_t now = platform::ReadTickCount32();
    const std::uint32_t start = gRequestStartTick;
    const std::uint32_t elapsed = now - start;
    if (elapsed <= kRequestTimeoutMilliseconds) {
        return 0;
    }

    static_cast<void>(gRequestMessage = L"新建角色逾時。" /* 原 0x58AFC4：新建角色逾時。正文 14 字节，末尾唯一 NUL。 */);
    return -1;
}

// 原函数：0x00508EE0。
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
        const auto expected = static_cast<unsigned char>("successful"[index]);
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
    CharacterRecord* const record = gCharacterRecords.data() + selected;
    record->registNumber = registNumber;
    record->previewState = previewToggle;
}

} // namespace cg::login
