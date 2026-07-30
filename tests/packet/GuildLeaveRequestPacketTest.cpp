#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::GuildLeaveRequest;

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
    failures += Expect(packet.code == cg::packet::PacketCode("XIsR"), message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "guildId", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == value, message);
    }
    return failures;
}

int TestSpecAndEncode()
{
    const GuildLeaveRequest request(10);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(GuildLeaveRequest::Code == "XIsR", "XIsR code");
    failures += Expect(
        GuildLeaveRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "XIsR direction");
    failures += Expect(GuildLeaveRequest::Fields.size() == 1U, "XIsR fields");
    failures += Expect(
        GuildLeaveRequest::Fields[0].name == "guildId",
        "XIsR field name");
    failures += Expect(
        GuildLeaveRequest::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "XIsR field type");
    failures += Expect(
        GuildLeaveRequest::Spec.code == GuildLeaveRequest::Code,
        "XIsR Spec code");
    failures += Expect(
        GuildLeaveRequest::Spec.direction == GuildLeaveRequest::Direction,
        "XIsR Spec direction");
    failures += Expect(
        GuildLeaveRequest::Spec.fieldCount == 1U,
        "XIsR Spec field count");
    failures += Expect(request.guildId == 10, "XIsR member");
    failures += ExpectPacket(packet, "10", "XIsR packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "XIsR Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "XIsR a ",
            "XIsR Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("XIsR a", GuildLeaveRequest::Spec);

    int failures = Expect(parsed.HasValue(), "XIsR generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(parsed.Value(), "10", "XIsR generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), GuildLeaveRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "XIsR generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(
            rebuilt.Value() == "XIsR a ",
            "XIsR generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int guildId;
        std::string_view expected;
    };

    constexpr Case Cases[] = {
        {0, "XIsR 0 "},
        {-1, "XIsR -1 "},
        {std::numeric_limits<int>::min(), "XIsR -2lkCB2 "},
        {std::numeric_limits<int>::max(), "XIsR 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const GuildLeaveRequest request(testCase.guildId);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.guildId),
            "XIsR signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "XIsR signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "XIsR signed canonical");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a ", GuildLeaveRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "XIsR wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("XIsR", GuildLeaveRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "XIsR",
        true,
        0U,
        "XIsR missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("XIsR a 1 ", GuildLeaveRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "XIsR",
        true,
        1U,
        "XIsR extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("XIsR _ ", GuildLeaveRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "XIsR",
        true,
        0U,
        "XIsR invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("XIsR 2lkCB2 ", GuildLeaveRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "XIsR",
        true,
        0U,
        "XIsR i62 overflow");
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
        std::cerr << failures << " guild leave request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
