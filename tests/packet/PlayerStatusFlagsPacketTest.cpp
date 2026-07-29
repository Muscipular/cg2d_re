#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <string_view>

namespace {

using cg::packet::receive::PlayerStatusFlags;

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
    const PlayerStatusFlags status(125);
    const cg::packet::Packet packet = status.ToPacket();

    int failures = 0;
    failures += Expect(PlayerStatusFlags::Code == "SkBD", "SkBD code");
    failures += Expect(
        PlayerStatusFlags::Direction == cg::packet::PacketDirection::ServerToClient,
        "SkBD direction");
    failures += Expect(PlayerStatusFlags::Fields.size() == 1U, "SkBD fields");
    failures += Expect(PlayerStatusFlags::Fields[0].name == "flags", "SkBD field name");
    failures += Expect(
        PlayerStatusFlags::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "SkBD field type");
    failures += Expect(PlayerStatusFlags::Spec.code == PlayerStatusFlags::Code, "SkBD Spec code");
    failures += Expect(
        PlayerStatusFlags::Spec.direction == PlayerStatusFlags::Direction,
        "SkBD Spec direction");
    failures += Expect(PlayerStatusFlags::Spec.fieldCount == 1U, "SkBD Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("SkBD"), "SkBD packet code");
    failures += Expect(packet.direction == PlayerStatusFlags::Direction, "SkBD packet direction");
    failures += Expect(packet.fields.size() == 1U, "SkBD packet fields");
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "flags", "SkBD packet field name");
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            "SkBD packet field type");
        failures += Expect(packet.fields[0].value == "125", "SkBD packet field value");
    }
    return failures;
}

int TestTypedParseAndCanonical()
{
    const cg::packet::PacketResult<PlayerStatusFlags> parsed =
        PlayerStatusFlags::Parse("SkBD 21 ");

    int failures = Expect(parsed.HasValue(), "SkBD typed parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().flags == 125, "SkBD flags value");

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value().ToPacket(), PlayerStatusFlags::Spec);
    failures += Expect(built.HasValue(), "SkBD typed build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "SkBD 21 ", "SkBD typed canonical");
    }
    return failures;
}

int TestGenericRoundtripAndUnrestrictedValues()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("SkBD 21", PlayerStatusFlags::Spec);

    int failures = Expect(parsed.HasValue(), "SkBD generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value(), PlayerStatusFlags::Spec);
    failures += Expect(built.HasValue(), "SkBD generic build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "SkBD 21 ", "SkBD generic canonical");
    }

    const cg::packet::PacketResult<PlayerStatusFlags> bitOne =
        PlayerStatusFlags::Parse("SkBD 2 ");
    failures += Expect(bitOne.HasValue(), "SkBD bit1 parse");
    if (bitOne.HasValue()) {
        failures += Expect(bitOne.Value().flags == 2, "SkBD bit1 retained");
    }

    const cg::packet::PacketResult<PlayerStatusFlags> high =
        PlayerStatusFlags::Parse("SkBD 1000 ");
    failures += Expect(high.HasValue(), "SkBD high bits parse");
    if (high.HasValue()) {
        failures += Expect(high.Value().flags == 238328, "SkBD high bits retained");
    }

    const cg::packet::PacketResult<PlayerStatusFlags> negative =
        PlayerStatusFlags::Parse("SkBD -1 ");
    failures += Expect(negative.HasValue(), "SkBD negative parse");
    if (negative.HasValue()) {
        failures += Expect(negative.Value().flags == -1, "SkBD negative retained");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        PlayerStatusFlags::Parse("bad 21 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "SkBD wrong code");
    failures += ExpectError(
        PlayerStatusFlags::Parse("SkBD"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "SkBD",
        true,
        0U,
        "SkBD missing field");
    failures += ExpectError(
        PlayerStatusFlags::Parse("SkBD 21 0 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "SkBD",
        true,
        1U,
        "SkBD extra field");
    failures += ExpectError(
        PlayerStatusFlags::Parse("SkBD _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "SkBD",
        true,
        0U,
        "SkBD invalid i62");
    failures += ExpectError(
        PlayerStatusFlags::Parse("SkBD 2lkCB2 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "SkBD",
        true,
        0U,
        "SkBD i62 overflow");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndToPacket()
        + TestTypedParseAndCanonical()
        + TestGenericRoundtripAndUnrestrictedValues()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " player status flags packet test(s) failed\n";
        return 1;
    }
    return 0;
}
