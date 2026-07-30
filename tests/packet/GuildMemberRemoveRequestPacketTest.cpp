#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::GuildMemberRemoveRequest;

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
    failures += Expect(packet.code == cg::packet::PacketCode("dbQC"), message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "index", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == value, message);
    }
    return failures;
}

int TestSpecAndEncode()
{
    const GuildMemberRemoveRequest request(10);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(
        GuildMemberRemoveRequest::Code == "dbQC",
        "dbQC code");
    failures += Expect(
        GuildMemberRemoveRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "dbQC direction");
    failures += Expect(
        GuildMemberRemoveRequest::Fields.size() == 1U,
        "dbQC fields");
    failures += Expect(
        GuildMemberRemoveRequest::Fields[0].name == "index",
        "dbQC field name");
    failures += Expect(
        GuildMemberRemoveRequest::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "dbQC field type");
    failures += Expect(
        GuildMemberRemoveRequest::Spec.code == GuildMemberRemoveRequest::Code,
        "dbQC Spec code");
    failures += Expect(
        GuildMemberRemoveRequest::Spec.direction ==
            GuildMemberRemoveRequest::Direction,
        "dbQC Spec direction");
    failures += Expect(
        GuildMemberRemoveRequest::Spec.fieldCount == 1U,
        "dbQC Spec field count");
    failures += Expect(request.index == 10, "dbQC member");
    failures += ExpectPacket(packet, "10", "dbQC packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "dbQC Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "dbQC a ",
            "dbQC Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("dbQC a", GuildMemberRemoveRequest::Spec);

    int failures = Expect(parsed.HasValue(), "dbQC generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(parsed.Value(), "10", "dbQC generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), GuildMemberRemoveRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "dbQC generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(
            rebuilt.Value() == "dbQC a ",
            "dbQC generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int index;
        std::string_view expected;
    };

    constexpr Case Cases[] = {
        {0, "dbQC 0 "},
        {-1, "dbQC -1 "},
        {std::numeric_limits<int>::min(), "dbQC -2lkCB2 "},
        {std::numeric_limits<int>::max(), "dbQC 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const GuildMemberRemoveRequest request(testCase.index);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.index),
            "dbQC signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "dbQC signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "dbQC signed canonical");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a ", GuildMemberRemoveRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "dbQC wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("dbQC", GuildMemberRemoveRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "dbQC",
        true,
        0U,
        "dbQC missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("dbQC a 1 ", GuildMemberRemoveRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "dbQC",
        true,
        1U,
        "dbQC extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("dbQC _ ", GuildMemberRemoveRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "dbQC",
        true,
        0U,
        "dbQC invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("dbQC 2lkCB2 ", GuildMemberRemoveRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "dbQC",
        true,
        0U,
        "dbQC i62 overflow");
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
        std::cerr
            << failures
            << " guild member remove request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
