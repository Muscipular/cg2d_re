#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::GuildMemberListRequest;

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
    failures += Expect(packet.code == cg::packet::PacketCode("CZ"), message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "time", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == value, message);
    }
    return failures;
}

int TestSpecAndEncode()
{
    const GuildMemberListRequest request(10);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(GuildMemberListRequest::Code == "CZ", "CZ code");
    failures += Expect(
        GuildMemberListRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "CZ direction");
    failures += Expect(GuildMemberListRequest::Fields.size() == 1U, "CZ fields");
    failures += Expect(
        GuildMemberListRequest::Fields[0].name == "time",
        "CZ field name");
    failures += Expect(
        GuildMemberListRequest::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "CZ field type");
    failures += Expect(
        GuildMemberListRequest::Spec.code == GuildMemberListRequest::Code,
        "CZ Spec code");
    failures += Expect(
        GuildMemberListRequest::Spec.direction == GuildMemberListRequest::Direction,
        "CZ Spec direction");
    failures += Expect(
        GuildMemberListRequest::Spec.fieldCount == 1U,
        "CZ Spec field count");
    failures += Expect(request.time == 10, "CZ member");
    failures += ExpectPacket(packet, "10", "CZ packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "CZ Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "CZ a ", "CZ Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("CZ a", GuildMemberListRequest::Spec);

    int failures = Expect(parsed.HasValue(), "CZ generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(parsed.Value(), "10", "CZ generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), GuildMemberListRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "CZ generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "CZ a ", "CZ generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int time;
        std::string_view expected;
    };

    constexpr Case Cases[] = {
        {0, "CZ 0 "},
        {-1, "CZ -1 "},
        {std::numeric_limits<int>::min(), "CZ -2lkCB2 "},
        {std::numeric_limits<int>::max(), "CZ 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const GuildMemberListRequest request(testCase.time);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.time),
            "CZ signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "CZ signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(encoded.Value() == testCase.expected, "CZ signed canonical");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a ", GuildMemberListRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "CZ wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("CZ", GuildMemberListRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "CZ",
        true,
        0U,
        "CZ missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("CZ a 1 ", GuildMemberListRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "CZ",
        true,
        1U,
        "CZ extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("CZ _ ", GuildMemberListRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "CZ",
        true,
        0U,
        "CZ invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("CZ 2lkCB2 ", GuildMemberListRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "CZ",
        true,
        0U,
        "CZ i62 overflow");
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
        std::cerr << failures << " guild member list request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
