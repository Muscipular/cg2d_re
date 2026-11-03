#include "cg/network/CgConnection.hpp"
#include "cg/scene/login/CharacterCreationRequest.hpp"
#include "cg/text/Encoding.hpp"

#include "../character_select/CharacterSelectState.hpp"
#include "../character_select/CharacterSelectSceneState.hpp"

#include "cg/scene/login/LoginRequest.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/MainLoop.hpp"

#include <cstddef>
#include <cstdio> // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。

namespace cg::login {

static constexpr std::uint32_t kRequestTimeoutMilliseconds = 600000;

// 原函数：0x00508E00。
std::uint32_t StartCharacterCreationRequest(network::CgConnection& connection)
{
    auto& submission = connection.CharacterSelectUi().creationSubmission;
    const std::int32_t element3 = submission.element3DE0B04;
    const std::int32_t element2 = submission.element2F633F4;
    const std::int32_t element1 = submission.element1FF03D0;
    const std::int32_t element0 = submission.element0F7C948;
    const std::int32_t attribute4 = submission.attribute4F7CA00;
    const std::int32_t attribute3 = submission.attribute3DE0AF8;
    const std::int32_t attribute2 = submission.attribute2DE03F4;
    const std::int32_t attribute1 = submission.attribute1F633F0;
    const std::int32_t attribute0 = submission.attribute0F633EC;
    const std::int32_t graphic = connection.CharacterSelectUi().selectedGraphicNo;
    const std::int32_t motion = submission.motionImageDE0AFC;
    const std::int32_t selected = connection.LoginRequests().selectedCharacterIndex;
    auto& request = connection.LoginRequests();
    const auto networkSession = connection.CaptureSession();
    request.characterCreationReplyCode = 0;
    connection.SendCharacterCreationRequest(selected, submission.nameFF0D18,
        motion, graphic, attribute0, attribute1, attribute2, attribute3, attribute4,
        element0, element1, element2, element3);
    request.status = 1;
    const std::uint64_t tick = platform::ReadTickCount64();
    request.startTick = tick;
    return static_cast<std::uint32_t>(tick);
}

// 原函数：0x00508E90。
std::int32_t PollCharacterCreationRequest(network::CgConnection& connection)
{
    auto& request = connection.LoginRequests();
    if (request.status == 2) {
        const std::uint16_t replyCode = request.characterCreationReplyCode;
        request.status = 0;
        return replyCode == 1 ? 1 : -2;
    }

    const std::uint64_t now = platform::ReadTickCount64();
    const std::uint64_t start = request.startTick;
    const std::uint64_t elapsed = now - start;
    if (elapsed <= kRequestTimeoutMilliseconds) {
        return 0;
    }

    static_cast<void>(request.message = L"新建角色逾时。" /* 原 0x58AFC4：新建角色逾時。正文 14 字节，末尾唯一 NUL。 */);
    return -1;
}

// 原函数：0x00508EE0。
void HandleCharacterCreationResponse_508EE0(cg::network::CgConnection& connection,
    const char* const result,
    const std::int32_t registNumber,
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

    const std::int16_t selected = request.selectedCharacterIndex;
    request.characterCreationReplyCode = 1;
    const std::int32_t previewToggle =
        connection.CharacterSelectUi().previewToggleState;
    CharacterRecord* const record = request.characters.data() + selected;
    record->registNumber = registNumber;
    record->previewState = previewToggle;
}

} // namespace cg::login
