#include "cg/scene/login/CharacterListRequest.hpp"
#include "cg/text/Encoding.hpp"

#include "cg/scene/login/LoginRequest.hpp"
#include "cg/input/Input.hpp"
#include "cg/network/NetworkRuntime.hpp"
#include "cg/platform/MainLoop.hpp"

#include <bit>
#include <cstddef>
#include <cstring>
#include <cstdio> // 原格式化／扫描接口：0x0053B304；直接沿MSVC CRT。

namespace {

constexpr std::uint32_t kRequestTimeoutMilliseconds = 600000;
constexpr char kInvalidCharacterName[] = "???";
constexpr std::size_t kOuterTokenCapacity = 1024;

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

    cg::login::CharacterRecord& record = cg::login::gCharacterRecords[
        static_cast<std::size_t>(slot)];
    record = {};

    const std::size_t nameLength = std::strlen(legacyName);
    if (nameLength <= 16) {
        std::memcpy(record.name.data(), legacyName, nameLength + 1);
    } else {
        std::memcpy(record.name.data(), kInvalidCharacterName, sizeof(kInvalidCharacterName));
    }

    record.graphicNo =
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 2);

    const std::int32_t token3 =
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 3);
    const std::uint16_t token3Low = static_cast<std::uint16_t>(
        static_cast<std::uint32_t>(token3));
    record.value12 = std::bit_cast<std::int16_t>(token3Low);

    record.value28 =
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 4);
    record.value2C =
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 5);
    record.value30 =
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 6);
    record.value34 =
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 7);
    record.value38 =
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 8);

    record.value3C =
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 9) / 10;
    record.value40 =
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 10) / 10;
    record.value44 =
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 11) / 10;
    record.value48 =
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 12) / 10;

    record.value14 =
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 13);
    record.registNumber =
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 15);
    record.previewState =
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 17);
    record.value1C =
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 18);
    record.deleteFlag =
        cg::network::ParseDelimitedInteger_52CFA0(mutableLegacyDetail, '|', 19);

    static_cast<void>(cg::network::ExtractDelimitedField_52CED0(
        mutableLegacyDetail,
        '|',
        16,
        static_cast<std::int32_t>(record.jobText.size() - 1),
        record.jobText.data()));
    return 0;
}

} // namespace

namespace cg::login {

std::uint16_t gCharacterListReplyCode = 0; // 原全局：0x00DA3B26。

// 原函数：0x00508910。
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

// 原函数：0x00508950。
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

    static_cast<void>(gRequestMessage = L"取得角色列表逾时。" /* 原 0x58AF94“取得角色列表逾時。”的原字节，正文 18B，含隐式 NUL 共 19B。 */);
    return -1;
}

// 原函数：0x005089B0。
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
        const std::int32_t comparison = CompareUnsignedBytes(data, "OUTOFSERVICE");
        if (comparison == 0) {
            gCharacterListReplyCode = 2;
        }
        return comparison;
    }

    gCharacterListReplyCode = 1;

    char legacyName[kOuterTokenCapacity];
    char mutableLegacyDetail[kOuterTokenCapacity];
    // 原每轮分别复制593850；当前无写者且恒空，保留两次复制及缓冲后缀。
    std::strcpy(legacyName, "");
    std::strcpy(mutableLegacyDetail, "");
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        data, '|', 1, static_cast<std::int32_t>(kOuterTokenCapacity - 1),
        legacyName));
    static_cast<void>(network::ExtractDelimitedField_52CED0(
        data, '|', 2, static_cast<std::int32_t>(kOuterTokenCapacity - 1),
        mutableLegacyDetail));
    std::int32_t writeResult = WriteCharacterRecord_51B1B0(
        legacyName,
        mutableLegacyDetail);

    // 原每轮分别复制593850；当前无写者且恒空，保留两次复制及缓冲后缀。
    std::strcpy(legacyName, "");
    std::strcpy(mutableLegacyDetail, "");
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
