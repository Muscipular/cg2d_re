#include "cg/LoginRequest.hpp"

#include "LoginRequestDependencies.hpp"

#include "cg/platform/MainLoop.hpp"

#include <cstring>

namespace {

constexpr std::size_t kRequestField18Offset = 0x18;
constexpr std::size_t kRequestField4COffset = 0x4C;
constexpr std::size_t kRequestField50Offset = 0x50;
constexpr std::uint32_t kRequestTimeoutMilliseconds = 600000;
constexpr char kRequestTimeoutMessage[] = "處理登入逾時。";

static_assert(kRequestField18Offset + sizeof(std::int32_t) <= cg::login::CharacterRecordSize);
static_assert(kRequestField4COffset + sizeof(std::int32_t) <= cg::login::CharacterRecordSize);
static_assert(kRequestField50Offset + sizeof(std::int32_t) <= cg::login::CharacterRecordSize);
static_assert(sizeof(kRequestTimeoutMessage) == 22);
static_assert(sizeof(kRequestTimeoutMessage) <= cg::login::RequestMessageCapacity);

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

    request_detail::Sub_512B00(
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

} // namespace cg::login
