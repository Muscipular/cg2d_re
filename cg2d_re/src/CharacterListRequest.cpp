#include "cg/CharacterListRequest.hpp"

#include "cg/LoginRequest.hpp"
#include "cg/Input.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "platform/LegacyFormat.hpp"

#include <cstddef>
#include <cstring>

namespace {

constexpr std::uint32_t kRequestTimeoutMilliseconds = 600000;
// 原 0x58AF94“取得角色列表逾時。”的原字节，正文 18B，含隐式 NUL 共 19B。
constexpr char kRequestTimeoutMessage[] = "\xA8\xFA\xB1\x6F\xA8\xA4\xA6\xE2\xA6\x43\xAA\xED\xB9\x4F\xAE\xC9\xA1\x43";
constexpr char kOutOfServiceResult[] = "OUTOFSERVICE";
constexpr char kInvalidCharacterName[] = "???";
constexpr std::size_t kOuterTokenCapacity = 1024;
constexpr std::size_t kJobTokenCapacity = 64;

static_assert(cg::login::CharacterRecordCount == 2);
static_assert(sizeof(cg::login::CharacterRecordBytes) == 0x94);
static_assert(sizeof(cg::login::CharacterRecordArray) == 2 * 0x94);
static_assert(sizeof(kRequestTimeoutMessage) == 19);
static_assert(sizeof(kRequestTimeoutMessage) <= cg::login::RequestMessageCapacity);
static_assert(sizeof(kOutOfServiceResult) == 13);
static_assert(sizeof(kInvalidCharacterName) == 4);
static_assert(0x54 + kJobTokenCapacity == cg::login::CharacterRecordSize);

std::int32_t CompareUnsignedBytes(const char* left, const char* right)
{
    for (;;) {
        const auto leftByte = static_cast<unsigned char>(*left);
        const auto rightByte = static_cast<unsigned char>(*right);
        if (leftByte != rightByte) {
            return leftByte < rightByte ? -1 : 1;
        }
        if (leftByte == 0) {
            return 0;
        }
        ++left;
        ++right;
    }
}

void WriteRecordInt32(
    cg::login::CharacterRecordBytes& record,
    const std::size_t offset,
    const std::int32_t value)
{
    std::memcpy(record.data() + offset, &value, sizeof(value));
}

// 原 sub_51B1B0@[0x51B1B0,0x51B3A2)；唯一调用者为 sub_5089B0。
std::int32_t WriteCharacterRecord_51B1B0(
    const char* legacyName,
    char* mutableLegacyDetail)
{
    static_cast<void>(cg::input::UnescapeLegacyTextAndNotify_52D2F0(
        mutableLegacyDetail));
    const std::int32_t slot =
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 1);
    if (slot < 0 || slot >= static_cast<std::int32_t>(cg::login::CharacterRecordCount)) {
        return -1;
    }

    cg::login::CharacterRecordBytes& record = cg::login::gCharacterRecords[
        static_cast<std::size_t>(slot)];
    record.fill(std::byte{0});

    const std::size_t nameLength = std::strlen(legacyName);
    if (nameLength <= 16) {
        std::memcpy(record.data(), legacyName, nameLength + 1);
    } else {
        std::memcpy(record.data(), kInvalidCharacterName, sizeof(kInvalidCharacterName));
    }

    WriteRecordInt32(
        record, 0x18,
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 2));

    const std::int32_t token3 =
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 3);
    const std::uint16_t token3Low = static_cast<std::uint16_t>(
        static_cast<std::uint32_t>(token3));
    std::memcpy(record.data() + 0x12, &token3Low, sizeof(token3Low));

    WriteRecordInt32(
        record, 0x28,
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 4));
    WriteRecordInt32(
        record, 0x2C,
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 5));
    WriteRecordInt32(
        record, 0x30,
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 6));
    WriteRecordInt32(
        record, 0x34,
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 7));
    WriteRecordInt32(
        record, 0x38,
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 8));

    WriteRecordInt32(
        record, 0x3C,
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 9) / 10);
    WriteRecordInt32(
        record, 0x40,
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 10) / 10);
    WriteRecordInt32(
        record, 0x44,
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 11) / 10);
    WriteRecordInt32(
        record, 0x48,
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 12) / 10);

    WriteRecordInt32(
        record, 0x14,
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 13));
    WriteRecordInt32(
        record, 0x4C,
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 15));
    WriteRecordInt32(
        record, 0x50,
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 17));
    WriteRecordInt32(
        record, 0x1C,
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 18));
    WriteRecordInt32(
        record, 0x20,
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 19));

    static_cast<void>(cg::network::ExtractDelimitedField_52CED0(
        mutableLegacyDetail,
        '|',
        16,
        static_cast<std::int32_t>(kJobTokenCapacity - 1),
        reinterpret_cast<char*>(record.data() + 0x54)));
    return 0;
}

} // namespace

namespace cg::login {

std::uint16_t gCharacterListReplyCode = 0;

std::uint32_t StartCharacterListRequest()
{
    static_cast<void>(Sub_51B3B0(0));
    static_cast<void>(Sub_51B3B0(1));

    const std::uintptr_t socket = platform::gConnectionSocket;
    gCharacterListReplyCode = 0;
    static_cast<void>(network::Sub_512B90(socket));
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

    static_cast<void>(platform::FormatLegacyString_53B304(
        gRequestMessage, kRequestTimeoutMessage));
    return -1;
}

std::int32_t HandleCharacterListResponse_5089B0(
    const std::uintptr_t,
    const std::int32_t result,
    const char* const data)
{
    if (gRequestStatus != 1) {
        return 1;
    }

    const bool succeeded = result == 0;
    gRequestStatus = 2;
    if (!succeeded) {
        const std::int32_t comparison = CompareUnsignedBytes(data, kOutOfServiceResult);
        if (comparison == 0) {
            gCharacterListReplyCode = 2;
        }
        return comparison;
    }

    gCharacterListReplyCode = 1;

    char legacyName[kOuterTokenCapacity];
    char mutableLegacyDetail[kOuterTokenCapacity];
    // 原每轮先分别复制593850现场前缀，不固定为空串或清缓冲后缀。
    std::strcpy(legacyName, input::LegacyText593850());
    std::strcpy(mutableLegacyDetail, input::LegacyText593850());
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        data, '|', 1, static_cast<std::int32_t>(kOuterTokenCapacity - 1),
        legacyName));
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        data, '|', 2, static_cast<std::int32_t>(kOuterTokenCapacity - 1),
        mutableLegacyDetail));
    std::int32_t writeResult = WriteCharacterRecord_51B1B0(
        legacyName,
        mutableLegacyDetail);

    // 原每轮先分别复制593850现场前缀，不固定为空串或清缓冲后缀。
    std::strcpy(legacyName, input::LegacyText593850());
    std::strcpy(mutableLegacyDetail, input::LegacyText593850());
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        data, '|', 3, static_cast<std::int32_t>(kOuterTokenCapacity - 1),
        legacyName));
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        data, '|', 4, static_cast<std::int32_t>(kOuterTokenCapacity - 1),
        mutableLegacyDetail));
    writeResult = WriteCharacterRecord_51B1B0(legacyName, mutableLegacyDetail);
    return writeResult;
}

} // namespace cg::login
