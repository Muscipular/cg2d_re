#include "cg/CharacterListRequest.hpp"

#include "cg/LoginRequest.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/MainLoop.hpp"
#include "platform/LegacyFormat.hpp"

#include <bit>
#include <cstddef>
#include <cstring>

namespace {

constexpr std::uint32_t kRequestTimeoutMilliseconds = 600000;
// 原 0x58AF94“取得角色列表逾時。”的原字节，正文 18B，含隐式 NUL 共 19B。
constexpr char kRequestTimeoutMessage[] = "\xA8\xFA\xB1\x6F\xA8\xA4\xA6\xE2\xA6\x43\xAA\xED\xB9\x4F\xAE\xC9\xA1\x43";
constexpr char kOutOfServiceResult[] = "OUTOFSERVICE";
constexpr char kInvalidCharacterName[] = "???";
constexpr std::size_t kOuterTokenCapacity = 1024;
constexpr std::size_t kNumericTokenCapacity = 128;
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

std::int32_t ExtractLegacyToken(
    const char* source,
    std::int32_t tokenIndex,
    char* output,
    std::size_t outputCapacity)
{
    const std::int32_t requestedIndex = tokenIndex <= 1 ? 1 : tokenIndex;
    const char* current = source;
    std::int32_t currentIndex = 1;

    while (currentIndex < requestedIndex) {
        const auto currentByte = static_cast<unsigned char>(*current);
        if (currentByte == 0) {
            output[0] = '\0';
            return 1;
        }
        if (currentByte >= 0x80) {
            ++current;
            if (*current != '\0') {
                ++current;
            }
            continue;
        }
        if (currentByte == static_cast<unsigned char>('|')) {
            ++currentIndex;
        }
        ++current;
    }

    const std::size_t maximumBytes = outputCapacity - 1;
    std::size_t outputSize = 0;
    for (;;) {
        const auto currentByte = static_cast<unsigned char>(*current);
        if (currentByte == 0) {
            output[outputSize] = '\0';
            return 1;
        }
        if (currentByte == static_cast<unsigned char>('|')) {
            output[outputSize] = '\0';
            return 0;
        }
        if (outputSize == maximumBytes) {
            output[outputSize] = '\0';
            return 1;
        }

        output[outputSize++] = *current++;
        if (currentByte < 0x80) {
            continue;
        }
        if (*current == '\0') {
            output[outputSize] = '\0';
            return 1;
        }
        if (outputSize == maximumBytes) {
            output[outputSize] = '\0';
            return 1;
        }
        output[outputSize++] = *current++;
    }
}

void UnescapeLegacyDetail(char* text)
{
    char* read = text;
    char* write = text;
    for (;;) {
        const auto currentByte = static_cast<unsigned char>(*read);
        if (currentByte == 0) {
            *write = '\0';
            return;
        }
        if (currentByte >= 0x80) {
            *write++ = *read++;
            if (*read != '\0') {
                *write++ = *read++;
            }
            continue;
        }
        if (*read != '\\') {
            *write++ = *read++;
            continue;
        }

        ++read;
        const char escaped = *read;
        if (escaped == '\0') {
            *write = '\0';
            return;
        }
        ++read;
        switch (escaped) {
        case 'n':
            *write++ = '\n';
            break;
        case 'c':
            *write++ = ',';
            break;
        case 'z':
            *write++ = '|';
            break;
        case 'y':
            *write++ = '\\';
            break;
        default:
            *write++ = escaped;
            break;
        }
    }
}

bool IsAsciiWhitespace(const unsigned char value)
{
    return value == static_cast<unsigned char>(' ')
        || value == static_cast<unsigned char>('\t')
        || value == static_cast<unsigned char>('\n')
        || value == static_cast<unsigned char>('\v')
        || value == static_cast<unsigned char>('\f')
        || value == static_cast<unsigned char>('\r');
}

std::int32_t LegacyAtoi(const char* text)
{
    if (*text == '\0') {
        return -1;
    }

    const auto* current = reinterpret_cast<const unsigned char*>(text);
    while (IsAsciiWhitespace(*current)) {
        ++current;
    }

    bool negative = false;
    if (*current == static_cast<unsigned char>('+')) {
        ++current;
    } else if (*current == static_cast<unsigned char>('-')) {
        negative = true;
        ++current;
    }

    std::uint32_t bits = 0;
    bool hasDigit = false;
    while (*current >= static_cast<unsigned char>('0')
        && *current <= static_cast<unsigned char>('9')) {
        hasDigit = true;
        bits = bits * 10U
            + static_cast<std::uint32_t>(*current - static_cast<unsigned char>('0'));
        ++current;
    }
    if (!hasDigit) {
        return 0;
    }
    if (negative) {
        bits = 0U - bits;
    }
    return std::bit_cast<std::int32_t>(bits);
}

std::int32_t ReadLegacyNumber(const char* detail, const std::int32_t tokenIndex)
{
    char token[kNumericTokenCapacity];
    ExtractLegacyToken(detail, tokenIndex, token, sizeof(token));
    return LegacyAtoi(token);
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
    UnescapeLegacyDetail(mutableLegacyDetail);
    const std::int32_t slot = ReadLegacyNumber(mutableLegacyDetail, 1);
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

    WriteRecordInt32(record, 0x18, ReadLegacyNumber(mutableLegacyDetail, 2));

    const std::int32_t token3 = ReadLegacyNumber(mutableLegacyDetail, 3);
    const std::uint16_t token3Low = static_cast<std::uint16_t>(
        static_cast<std::uint32_t>(token3));
    std::memcpy(record.data() + 0x12, &token3Low, sizeof(token3Low));

    WriteRecordInt32(record, 0x28, ReadLegacyNumber(mutableLegacyDetail, 4));
    WriteRecordInt32(record, 0x2C, ReadLegacyNumber(mutableLegacyDetail, 5));
    WriteRecordInt32(record, 0x30, ReadLegacyNumber(mutableLegacyDetail, 6));
    WriteRecordInt32(record, 0x34, ReadLegacyNumber(mutableLegacyDetail, 7));
    WriteRecordInt32(record, 0x38, ReadLegacyNumber(mutableLegacyDetail, 8));

    WriteRecordInt32(record, 0x3C, ReadLegacyNumber(mutableLegacyDetail, 9) / 10);
    WriteRecordInt32(record, 0x40, ReadLegacyNumber(mutableLegacyDetail, 10) / 10);
    WriteRecordInt32(record, 0x44, ReadLegacyNumber(mutableLegacyDetail, 11) / 10);
    WriteRecordInt32(record, 0x48, ReadLegacyNumber(mutableLegacyDetail, 12) / 10);

    WriteRecordInt32(record, 0x14, ReadLegacyNumber(mutableLegacyDetail, 13));
    WriteRecordInt32(record, 0x4C, ReadLegacyNumber(mutableLegacyDetail, 15));
    WriteRecordInt32(record, 0x50, ReadLegacyNumber(mutableLegacyDetail, 17));
    WriteRecordInt32(record, 0x1C, ReadLegacyNumber(mutableLegacyDetail, 18));
    WriteRecordInt32(record, 0x20, ReadLegacyNumber(mutableLegacyDetail, 19));

    ExtractLegacyToken(
        mutableLegacyDetail,
        16,
        reinterpret_cast<char*>(record.data() + 0x54),
        kJobTokenCapacity);
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

    gRequestStatus = 2;
    if (result != 0) {
        const std::int32_t comparison = CompareUnsignedBytes(data, kOutOfServiceResult);
        if (comparison == 0) {
            gCharacterListReplyCode = 2;
        }
        return comparison;
    }

    gCharacterListReplyCode = 1;

    char legacyName[kOuterTokenCapacity];
    char mutableLegacyDetail[kOuterTokenCapacity];
    ExtractLegacyToken(data, 1, legacyName, sizeof(legacyName));
    ExtractLegacyToken(data, 2, mutableLegacyDetail, sizeof(mutableLegacyDetail));
    std::int32_t writeResult = WriteCharacterRecord_51B1B0(
        legacyName,
        mutableLegacyDetail);

    ExtractLegacyToken(data, 3, legacyName, sizeof(legacyName));
    ExtractLegacyToken(data, 4, mutableLegacyDetail, sizeof(mutableLegacyDetail));
    writeResult = WriteCharacterRecord_51B1B0(legacyName, mutableLegacyDetail);
    return writeResult;
}

} // namespace cg::login
