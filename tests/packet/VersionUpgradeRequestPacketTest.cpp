#include "packet/Send.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::VersionUpgradeRequest;

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
}

std::string Bytes(std::initializer_list<unsigned int> bytes)
{
    std::string value;
    value.reserve(bytes.size());
    for (const unsigned int byte : bytes) {
        value.push_back(static_cast<char>(byte));
    }
    return value;
}

template <typename T>
int ExpectError(
    const cg::packet::PacketResult<T>& result,
    cg::packet::PacketErrorCode code,
    std::string_view packetCode,
    bool hasFieldIndex,
    std::size_t fieldIndex,
    const char* message)
{
    int failures = Expect(result.HasError(), message);
    if (!result.HasError()) {
        return failures;
    }

    const cg::packet::PacketError& error = result.Error();
    failures += Expect(error.code == code, message);
    failures += Expect(error.packetCode.has_value(), message);
    if (error.packetCode.has_value()) {
        failures += Expect(error.packetCode->View() == packetCode, message);
    }
    failures += Expect(error.fieldIndex.has_value() == hasFieldIndex, message);
    if (hasFieldIndex && error.fieldIndex.has_value()) {
        failures += Expect(*error.fieldIndex == fieldIndex, message);
    }
    return failures;
}

int ExpectField(
    const cg::packet::Packet& packet,
    std::size_t index,
    std::string_view name,
    cg::packet::PacketFieldType type,
    std::string_view value,
    const char* message)
{
    int failures = Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }

    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(packet.fields[index].type == type, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int ExpectPacket(
    const cg::packet::Packet& packet,
    std::string_view mode,
    std::string_view productKey,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode("Xoee"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() == 2U) {
        failures += ExpectField(
            packet,
            0U,
            "mode",
            cg::packet::PacketFieldType::Int62,
            mode,
            message);
        failures += ExpectField(
            packet,
            1U,
            "productKey",
            cg::packet::PacketFieldType::EscapedString,
            productKey,
            message);
    }
    return failures;
}

int TestSpecAndEncode()
{
    const VersionUpgradeRequest request(2, "20031229_20");
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(VersionUpgradeRequest::Code == "Xoee", "Xoee code");
    failures += Expect(
        VersionUpgradeRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "Xoee direction");
    failures += Expect(
        VersionUpgradeRequest::Fields.size() == 2U,
        "Xoee fields");
    failures += Expect(
        VersionUpgradeRequest::Fields[0].name == "mode",
        "Xoee mode name");
    failures += Expect(
        VersionUpgradeRequest::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "Xoee mode type");
    failures += Expect(
        VersionUpgradeRequest::Fields[1].name == "productKey",
        "Xoee productKey name");
    failures += Expect(
        VersionUpgradeRequest::Fields[1].type ==
            cg::packet::PacketFieldType::EscapedString,
        "Xoee productKey type");
    failures += Expect(
        VersionUpgradeRequest::Spec.code == VersionUpgradeRequest::Code,
        "Xoee Spec code");
    failures += Expect(
        VersionUpgradeRequest::Spec.direction ==
            VersionUpgradeRequest::Direction,
        "Xoee Spec direction");
    failures += Expect(
        VersionUpgradeRequest::Spec.fieldCount == 2U,
        "Xoee Spec field count");
    failures += Expect(request.mode == 2, "Xoee mode member");
    failures += Expect(
        request.productKey == "20031229_20",
        "Xoee productKey member");
    failures += ExpectPacket(
        packet,
        "2",
        "20031229_20",
        "Xoee packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "Xoee Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "Xoee 2 20031229_20 ",
            "Xoee canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(
            "Xoee 2 20031229_20",
            VersionUpgradeRequest::Spec);

    int failures = Expect(parsed.HasValue(), "Xoee generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(
        parsed.Value(),
        "2",
        "20031229_20",
        "Xoee generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(
            parsed.Value(),
            VersionUpgradeRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "Xoee generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(
            rebuilt.Value() == "Xoee 2 20031229_20 ",
            "Xoee generic canonical");
    }
    return failures;
}

int TestSignedMode()
{
    struct Case
    {
        int mode;
        std::string_view expected;
    };

    // 负数与 int 边界仅证明 cg3a codec，不表示 Server 业务有效。
    constexpr Case Cases[] = {
        {0, "Xoee 0 key "},
        {2, "Xoee 2 key "},
        {4, "Xoee 4 key "},
        {5, "Xoee 5 key "},
        {-1, "Xoee -1 key "},
        {std::numeric_limits<int>::min(), "Xoee -2lkCB2 key "},
        {std::numeric_limits<int>::max(), "Xoee 2lkCB1 key "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const VersionUpgradeRequest request(testCase.mode, "key");
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.mode),
            "key",
            "Xoee signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "Xoee signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "Xoee signed canonical");
        }
    }
    return failures;
}

int TestEscapedProductKey()
{
    int failures = 0;

    const VersionUpgradeRequest empty(2, "");
    const cg::packet::PacketResult<std::string> emptyEncoded = empty.Encode();
    failures += Expect(emptyEncoded.HasValue(), "Xoee empty Encode");
    if (emptyEncoded.HasValue()) {
        failures += Expect(
            emptyEncoded.Value() == "Xoee 2  ",
            "Xoee empty canonical");
    }

    const std::string freeText = "A B\nC\rD\\E";
    const VersionUpgradeRequest freeTextRequest(2, freeText);
    const cg::packet::PacketResult<std::string> freeTextEncoded =
        freeTextRequest.Encode();
    failures += Expect(freeTextEncoded.HasValue(), "Xoee free text Encode");
    if (freeTextEncoded.HasValue()) {
        failures += Expect(
            freeTextEncoded.Value() == "Xoee 2 A\\SB\\nC\\rD\\\\E ",
            "Xoee free text canonical");
        const cg::packet::PacketResult<cg::packet::Packet> parsed =
            cg::packet::ParsePacket(
                freeTextEncoded.Value(),
                VersionUpgradeRequest::Spec);
        failures += Expect(parsed.HasValue(), "Xoee free text parse");
        if (parsed.HasValue()) {
            failures += ExpectPacket(
                parsed.Value(),
                "2",
                freeText,
                "Xoee free text roundtrip");
        }
    }

    const std::string gbkText = Bytes({0xD3U, 0xC2U, 0xD5U, 0xDFU});
    const VersionUpgradeRequest gbkRequest(2, gbkText);
    const cg::packet::PacketResult<std::string> gbkEncoded =
        gbkRequest.Encode();
    failures += Expect(gbkEncoded.HasValue(), "Xoee GBK Encode");
    if (gbkEncoded.HasValue()) {
        std::string expected = "Xoee 2 ";
        expected.append(gbkText);
        expected.push_back(' ');
        failures += Expect(
            gbkEncoded.Value() == expected,
            "Xoee GBK canonical");
        const cg::packet::PacketResult<cg::packet::Packet> parsed =
            cg::packet::ParsePacket(
                expected,
                VersionUpgradeRequest::Spec);
        failures += Expect(parsed.HasValue(), "Xoee GBK parse");
        if (parsed.HasValue()) {
            failures += ExpectPacket(
                parsed.Value(),
                "2",
                gbkText,
                "Xoee GBK roundtrip");
        }
    }

    const std::string gbkTrailPipe = Bytes({0x81U, 0x7CU});
    const VersionUpgradeRequest trailPipeRequest(2, gbkTrailPipe);
    const cg::packet::PacketResult<std::string> trailPipeEncoded =
        trailPipeRequest.Encode();
    failures += Expect(
        trailPipeEncoded.HasValue(),
        "Xoee GBK trail pipe Encode");
    if (trailPipeEncoded.HasValue()) {
        std::string expected = "Xoee 2 ";
        expected.append(gbkTrailPipe);
        expected.push_back(' ');
        failures += Expect(
            trailPipeEncoded.Value() == expected,
            "Xoee GBK trail pipe canonical");
        const cg::packet::PacketResult<cg::packet::Packet> parsed =
            cg::packet::ParsePacket(
                expected,
                VersionUpgradeRequest::Spec);
        failures += Expect(parsed.HasValue(), "Xoee GBK trail pipe parse");
        if (parsed.HasValue()) {
            failures += ExpectPacket(
                parsed.Value(),
                "2",
                gbkTrailPipe,
                "Xoee GBK trail pipe roundtrip");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket(
            "bad 2 key ",
            VersionUpgradeRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "Xoee wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "Xoee",
            VersionUpgradeRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Xoee",
        true,
        0U,
        "Xoee missing all fields");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "Xoee 2",
            VersionUpgradeRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Xoee",
        true,
        1U,
        "Xoee missing productKey");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "Xoee 2 key extra ",
            VersionUpgradeRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Xoee",
        true,
        2U,
        "Xoee extra field");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "Xoee _ key ",
            VersionUpgradeRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Xoee",
        true,
        0U,
        "Xoee invalid mode");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "Xoee 2lkCB2 key ",
            VersionUpgradeRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Xoee",
        true,
        0U,
        "Xoee positive mode overflow");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "Xoee -2lkCB3 key ",
            VersionUpgradeRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Xoee",
        true,
        0U,
        "Xoee negative mode overflow");

    std::string truncatedGbk = "Xoee 2 ";
    truncatedGbk.push_back(static_cast<char>(0x81U));
    failures += ExpectError(
        cg::packet::ParsePacket(
            truncatedGbk,
            VersionUpgradeRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Xoee",
        true,
        1U,
        "Xoee truncated GBK");

    const std::string pollutedCode = "Xoee\x17" "x";
    const std::string pollutedWire = pollutedCode + " 2 key";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedWire,
            VersionUpgradeRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "Xoee polluted code");

    const std::string pollutedMode = "Xoee 2\x17" "x key";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedMode,
            VersionUpgradeRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Xoee",
        true,
        2U,
        "Xoee polluted mode");

    const std::string pollutedProductKey = "Xoee 2 key\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedProductKey,
            VersionUpgradeRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Xoee",
        true,
        2U,
        "Xoee polluted productKey");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndEncode()
        + TestGenericRoundtrip()
        + TestSignedMode()
        + TestEscapedProductKey()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures
                  << " version upgrade request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
