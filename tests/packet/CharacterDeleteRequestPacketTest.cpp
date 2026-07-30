#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::CharacterDeleteRequest;

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
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

int ExpectPacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode("Godf"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "registNumber", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == value, message);
    }
    return failures;
}

int TestSpecAndEncode()
{
    const CharacterDeleteRequest request(10);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(CharacterDeleteRequest::Code == "Godf", "Godf code");
    failures += Expect(
        CharacterDeleteRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "Godf direction");
    failures += Expect(
        CharacterDeleteRequest::Fields.size() == 1U,
        "Godf fields");
    failures += Expect(
        CharacterDeleteRequest::Fields[0].name == "registNumber",
        "Godf field name");
    failures += Expect(
        CharacterDeleteRequest::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "Godf field type");
    failures += Expect(
        CharacterDeleteRequest::Spec.code == CharacterDeleteRequest::Code,
        "Godf Spec code");
    failures += Expect(
        CharacterDeleteRequest::Spec.direction ==
            CharacterDeleteRequest::Direction,
        "Godf Spec direction");
    failures += Expect(
        CharacterDeleteRequest::Spec.fieldCount == 1U,
        "Godf Spec field count");
    failures += Expect(request.registNumber == 10, "Godf member");
    failures += ExpectPacket(packet, "10", "Godf packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "Godf Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "Godf a ",
            "Godf canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(
            "Godf a",
            CharacterDeleteRequest::Spec);

    int failures = Expect(parsed.HasValue(), "Godf generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(
        parsed.Value(),
        "10",
        "Godf generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(
            parsed.Value(),
            CharacterDeleteRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "Godf generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(
            rebuilt.Value() == "Godf a ",
            "Godf generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int registNumber;
        std::string_view expected;
    };

    // 负数与 int 边界仅证明 cg3a codec，不表示 Server 业务有效。
    constexpr Case Cases[] = {
        {0, "Godf 0 "},
        {10, "Godf a "},
        {-1, "Godf -1 "},
        {100, "Godf 1C "},
        {std::numeric_limits<int>::min(), "Godf -2lkCB2 "},
        {std::numeric_limits<int>::max(), "Godf 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const CharacterDeleteRequest request(testCase.registNumber);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.registNumber),
            "Godf signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "Godf signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "Godf signed canonical");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket(
            "bad a ",
            CharacterDeleteRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "Godf wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "Godf",
            CharacterDeleteRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Godf",
        true,
        0U,
        "Godf missing field");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "Godf a 0 ",
            CharacterDeleteRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Godf",
        true,
        1U,
        "Godf extra field");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "Godf _ ",
            CharacterDeleteRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Godf",
        true,
        0U,
        "Godf invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "Godf 2lkCB2 ",
            CharacterDeleteRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Godf",
        true,
        0U,
        "Godf positive overflow");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "Godf -2lkCB3 ",
            CharacterDeleteRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Godf",
        true,
        0U,
        "Godf negative overflow");

    const std::string pollutedCode = "Godf\x17" "x";
    const std::string pollutedWire = pollutedCode + " a";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedWire,
            CharacterDeleteRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "Godf polluted code");

    const std::string pollutedField = "Godf a\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedField,
            CharacterDeleteRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Godf",
        true,
        1U,
        "Godf polluted field");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndEncode()
        + TestGenericRoundtrip()
        + TestSignedAndBoundaryValues()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures
                  << " character delete request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
