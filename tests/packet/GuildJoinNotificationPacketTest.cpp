#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::GuildJoinNotification;

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

int TestSpecAndToPacket()
{
    const GuildJoinNotification notification;
    const cg::packet::Packet packet = notification.ToPacket();

    int failures = 0;
    failures += Expect(GuildJoinNotification::Code == "GC", "GC code");
    failures += Expect(
        GuildJoinNotification::Direction == cg::packet::PacketDirection::ServerToClient,
        "GC direction");
    failures += Expect(GuildJoinNotification::Fields.empty(), "GC fields");
    failures += Expect(
        GuildJoinNotification::Spec.code == GuildJoinNotification::Code,
        "GC Spec code");
    failures += Expect(
        GuildJoinNotification::Spec.direction == GuildJoinNotification::Direction,
        "GC Spec direction");
    failures += Expect(
        GuildJoinNotification::Spec.fieldCount == 0U,
        "GC Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("GC"), "GC packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ServerToClient,
        "GC packet direction");
    failures += Expect(packet.fields.empty(), "GC packet fields");

    const cg::packet::PacketResult<GuildJoinNotification> converted =
        GuildJoinNotification::FromPacket(packet);
    failures += Expect(converted.HasValue(), "GC FromPacket valid");
    return failures;
}

int TestTypedAndGenericCanonical()
{
    int failures = 0;
    for (const std::string_view input : {"GC", "GC "}) {
        const cg::packet::PacketResult<GuildJoinNotification> typed =
            GuildJoinNotification::Parse(input);
        failures += Expect(typed.HasValue(), "GC typed parse");
        if (typed.HasValue()) {
            const cg::packet::PacketResult<std::string> built =
                cg::packet::BuildPacket(typed.Value().ToPacket(), GuildJoinNotification::Spec);
            failures += Expect(built.HasValue(), "GC typed build");
            if (built.HasValue()) {
                failures += Expect(built.Value() == "GC ", "GC typed canonical");
            }
        }

        const cg::packet::PacketResult<cg::packet::Packet> generic =
            cg::packet::ParsePacket(input, GuildJoinNotification::Spec);
        failures += Expect(generic.HasValue(), "GC generic parse");
        if (!generic.HasValue()) {
            continue;
        }
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(generic.Value(), GuildJoinNotification::Spec);
        failures += Expect(rebuilt.HasValue(), "GC generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(rebuilt.Value() == "GC ", "GC generic canonical");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        GuildJoinNotification::Parse("bad"),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "GC typed wrong code");
    failures += ExpectError(
        GuildJoinNotification::Parse("GC 1 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "GC",
        true,
        0U,
        "GC typed extra field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    const cg::packet::Packet wrongCode(
        cg::packet::PacketCode("bad"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        GuildJoinNotification::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "GC FromPacket wrong code");

    const cg::packet::Packet wrongDirection(
        cg::packet::PacketCode("GC"),
        cg::packet::PacketDirection::ClientToServer);
    failures += ExpectError(
        GuildJoinNotification::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "GC",
        false,
        0U,
        "GC FromPacket wrong direction");

    cg::packet::Packet extraField(
        cg::packet::PacketCode("GC"),
        cg::packet::PacketDirection::ServerToClient);
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        std::string("1"),
    });
    failures += ExpectError(
        GuildJoinNotification::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "GC",
        true,
        0U,
        "GC FromPacket extra field");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndToPacket()
        + TestTypedAndGenericCanonical()
        + TestParseErrors()
        + TestFromPacketErrors();
    if (failures != 0) {
        std::cerr << failures << " guild join notification packet test(s) failed\n";
        return 1;
    }
    return 0;
}
