#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::GuildDissolveRequest;

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
    failures += Expect(packet.code == cg::packet::PacketCode("nkda"), message);
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

int TestSpecAndTen()
{
    const GuildDissolveRequest request(10);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(GuildDissolveRequest::Code == "nkda", "nkda code");
    failures += Expect(
        GuildDissolveRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "nkda direction");
    failures += Expect(GuildDissolveRequest::Fields.size() == 1U, "nkda fields");
    failures += Expect(
        GuildDissolveRequest::Fields[0].name == "guildId",
        "nkda field name");
    failures += Expect(
        GuildDissolveRequest::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "nkda field type");
    failures += Expect(
        GuildDissolveRequest::Spec.code == GuildDissolveRequest::Code,
        "nkda Spec code");
    failures += Expect(
        GuildDissolveRequest::Spec.direction == GuildDissolveRequest::Direction,
        "nkda Spec direction");
    failures += Expect(
        GuildDissolveRequest::Spec.fieldCount == 1U,
        "nkda Spec field count");
    failures += Expect(request.guildId == 10, "nkda member");
    failures += ExpectPacket(packet, "10", "nkda packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "nkda Encode ten");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "nkda a ", "nkda ten canonical");
    }
    return failures;
}

int TestSignedAndExtremeValues()
{
    struct Case
    {
        int guildId;
        std::string_view expected;
    };

    constexpr Case Cases[] = {
        {-1, "nkda -1 "},
        {std::numeric_limits<int>::min(), "nkda -2lkCB2 "},
        {std::numeric_limits<int>::max(), "nkda 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const GuildDissolveRequest request(testCase.guildId);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.guildId),
            "nkda signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "nkda signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(encoded.Value() == testCase.expected, "nkda signed canonical");
        }
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("nkda a", GuildDissolveRequest::Spec);

    int failures = Expect(parsed.HasValue(), "nkda generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(parsed.Value(), "10", "nkda generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), GuildDissolveRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "nkda generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "nkda a ", "nkda generic canonical");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a ", GuildDissolveRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "nkda wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("nkda", GuildDissolveRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nkda",
        true,
        0U,
        "nkda missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("nkda a 1 ", GuildDissolveRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nkda",
        true,
        1U,
        "nkda extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("nkda _ ", GuildDissolveRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "nkda",
        true,
        0U,
        "nkda invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("nkda 2lkCB2 ", GuildDissolveRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "nkda",
        true,
        0U,
        "nkda i62 overflow");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndTen()
        + TestSignedAndExtremeValues()
        + TestGenericRoundtrip()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " guild dissolve request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
