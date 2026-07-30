#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::PlayerStatusFlagsUpdateRequest;

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
        packet.code == cg::packet::PacketCode("dcMv"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "flags", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == value, message);
    }
    return failures;
}

int TestSpecAndEncode()
{
    const PlayerStatusFlagsUpdateRequest request(125);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(
        PlayerStatusFlagsUpdateRequest::Code == "dcMv",
        "dcMv code");
    failures += Expect(
        PlayerStatusFlagsUpdateRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "dcMv direction");
    failures += Expect(
        PlayerStatusFlagsUpdateRequest::Fields.size() == 1U,
        "dcMv fields");
    failures += Expect(
        PlayerStatusFlagsUpdateRequest::Fields[0].name == "flags",
        "dcMv field name");
    failures += Expect(
        PlayerStatusFlagsUpdateRequest::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "dcMv field type");
    failures += Expect(
        PlayerStatusFlagsUpdateRequest::Spec.code ==
            PlayerStatusFlagsUpdateRequest::Code,
        "dcMv Spec code");
    failures += Expect(
        PlayerStatusFlagsUpdateRequest::Spec.direction ==
            PlayerStatusFlagsUpdateRequest::Direction,
        "dcMv Spec direction");
    failures += Expect(
        PlayerStatusFlagsUpdateRequest::Spec.fieldCount == 1U,
        "dcMv Spec field count");
    failures += Expect(request.flags == 125, "dcMv member");
    failures += ExpectPacket(packet, "125", "dcMv packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "dcMv Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "dcMv 21 ",
            "dcMv Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(
            "dcMv 21",
            PlayerStatusFlagsUpdateRequest::Spec);

    int failures = Expect(parsed.HasValue(), "dcMv generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(parsed.Value(), "125", "dcMv generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(
            parsed.Value(),
            PlayerStatusFlagsUpdateRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "dcMv generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(
            rebuilt.Value() == "dcMv 21 ",
            "dcMv generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int flags;
        std::string_view expected;
    };

    constexpr Case Cases[] = {
        {0, "dcMv 0 "},
        {1, "dcMv 1 "},
        {2, "dcMv 2 "},
        {64, "dcMv 12 "},
        {238328, "dcMv 1000 "},
        {-1, "dcMv -1 "},
        {std::numeric_limits<int>::min(), "dcMv -2lkCB2 "},
        {std::numeric_limits<int>::max(), "dcMv 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const PlayerStatusFlagsUpdateRequest request(testCase.flags);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.flags),
            "dcMv signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "dcMv signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "dcMv signed canonical");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket(
            "bad 21 ",
            PlayerStatusFlagsUpdateRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "dcMv wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "dcMv",
            PlayerStatusFlagsUpdateRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "dcMv",
        true,
        0U,
        "dcMv missing field");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "dcMv 21 1 ",
            PlayerStatusFlagsUpdateRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "dcMv",
        true,
        1U,
        "dcMv extra field");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "dcMv _ ",
            PlayerStatusFlagsUpdateRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "dcMv",
        true,
        0U,
        "dcMv invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "dcMv 2lkCB2 ",
            PlayerStatusFlagsUpdateRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "dcMv",
        true,
        0U,
        "dcMv positive i62 overflow");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "dcMv -2lkCB3 ",
            PlayerStatusFlagsUpdateRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "dcMv",
        true,
        0U,
        "dcMv negative i62 overflow");

    const std::string pollutedCode = "dcMv\x17" "21";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedCode,
            PlayerStatusFlagsUpdateRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "dcMv polluted code");

    const std::string pollutedField = "dcMv 21\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedField,
            PlayerStatusFlagsUpdateRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "dcMv",
        true,
        1U,
        "dcMv polluted field");
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
                  << " player status flags update request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
