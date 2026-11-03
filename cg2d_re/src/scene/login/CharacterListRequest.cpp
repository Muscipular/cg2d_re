#include "cg/network/CgConnection.hpp"
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

namespace cg::login {

static constexpr std::uint32_t kRequestTimeoutMilliseconds = 600000;
static constexpr char kInvalidCharacterName[] = "???";
static constexpr std::size_t kOuterTokenCapacity = 1024;

static std::int32_t CompareUnsignedBytes(const char* left, const char* right)
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

// 原 sub_51B1B0@[0x51B1B0,0x51B3A2)；NSM01额外传入回调所属连接的记录数组。
static std::int32_t WriteCharacterRecord_51B1B0(
    CharacterRecordArray& characters,
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

    cg::login::CharacterRecord& record = characters[
        static_cast<std::size_t>(slot)];
    record = {};

    const std::size_t nameLength = std::strlen(legacyName);
    if (nameLength <= 16) {
        record.name = cg::text::FromGbkWide(legacyName);
    } else {
        record.name = cg::text::FromGbkWide(kInvalidCharacterName);
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

    char jobBytes[64]{};
    static_cast<void>(cg::network::ExtractDelimitedField_52CED0(
        mutableLegacyDetail, '|', 16, 63, jobBytes));
    record.jobText = cg::text::FromGbkWide(jobBytes);
    return 0;
}

// 原函数：0x00508910。
std::uint32_t StartCharacterListRequest()
{
    static_cast<void>(ClearCharacterSlot_51B3B0(0));
    static_cast<void>(ClearCharacterSlot_51B3B0(1));

    auto& connection = cg::network::GetGameConnection();
    auto& request = connection.LoginRequests();
    const auto networkSession = connection.CaptureSession();
    request.characterListReplyCode = 0;
    static_cast<void>(connection.SendCharacterListRequest());
    request.status = 1;
    const std::uint64_t tick = platform::ReadTickCount64();
    request.startTick = tick;
    return static_cast<std::uint32_t>(tick);
}

// 原函数：0x00508950。
std::int32_t PollCharacterListRequest()
{
    auto& request = cg::network::GetGameConnection().LoginRequests();
    if (request.status == 2) {
        const std::uint16_t replyCode = request.characterListReplyCode;
        request.status = 0;
        if (replyCode == 1) {
            return 1;
        }
        if (replyCode == 2) {
            return -3;
        }
        return -2;
    }

    const std::uint64_t now = platform::ReadTickCount64();
    const std::uint64_t elapsed = now - request.startTick;
    if (elapsed <= kRequestTimeoutMilliseconds) {
        return 0;
    }

    static_cast<void>(request.message = L"取得角色列表逾时。" /* 原 0x58AF94“取得角色列表逾時。”的原字节，正文 18B，含隐式 NUL 共 19B。 */);
    return -1;
}

// 原函数：0x005089B0。
std::int32_t HandleCharacterListResponse_5089B0(cg::network::CgConnection& connection,
    const std::int32_t result,
    const char* const data)
{
    auto& request = connection.LoginRequests();
    if (request.status != 1) {
        return 1;
    }

    const bool succeeded = result == 0;
    request.status = 2;
    if (!succeeded) {
        const std::int32_t comparison = CompareUnsignedBytes(data, "OUTOFSERVICE");
        if (comparison == 0) {
            request.characterListReplyCode = 2;
        }
        return comparison;
    }

    request.characterListReplyCode = 1;

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
        request.characters,
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
    writeResult = WriteCharacterRecord_51B1B0(request.characters, legacyName, mutableLegacyDetail);
    return writeResult;
}

} // namespace cg::login
