#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <string_view>

namespace {

using cg::packet::receive::BattleEntryNotification;

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
    const BattleEntryNotification notification(2, 62);
    const cg::packet::Packet packet = notification.ToPacket();

    int failures = 0;
    failures += Expect(BattleEntryNotification::Code == "dAgg", "dAgg code");
    failures += Expect(
        BattleEntryNotification::Direction == cg::packet::PacketDirection::ServerToClient,
        "dAgg direction");
    failures += Expect(BattleEntryNotification::Fields.size() == 2U, "dAgg fields");
    failures += Expect(
        BattleEntryNotification::Fields[0].name == "battletype",
        "dAgg battletype name");
    failures += Expect(
        BattleEntryNotification::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "dAgg battletype type");
    failures += Expect(
        BattleEntryNotification::Fields[1].name == "fieldno",
        "dAgg fieldno name");
    failures += Expect(
        BattleEntryNotification::Fields[1].type == cg::packet::PacketFieldType::Int62,
        "dAgg fieldno type");
    failures += Expect(
        BattleEntryNotification::Spec.code == BattleEntryNotification::Code,
        "dAgg Spec code");
    failures += Expect(
        BattleEntryNotification::Spec.direction == BattleEntryNotification::Direction,
        "dAgg Spec direction");
    failures += Expect(BattleEntryNotification::Spec.fieldCount == 2U, "dAgg Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("dAgg"), "dAgg packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ServerToClient,
        "dAgg packet direction");
    failures += Expect(packet.fields.size() == 2U, "dAgg packet fields");
    if (packet.fields.size() == 2U) {
        failures += Expect(packet.fields[0].name == "battletype", "dAgg packet battletype name");
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            "dAgg packet battletype type");
        failures += Expect(packet.fields[0].value == "2", "dAgg packet battletype value");
        failures += Expect(packet.fields[1].name == "fieldno", "dAgg packet fieldno name");
        failures += Expect(
            packet.fields[1].type == cg::packet::PacketFieldType::Int62,
            "dAgg packet fieldno type");
        failures += Expect(packet.fields[1].value == "62", "dAgg packet fieldno value");
    }
    return failures;
}

int TestTypedParseAndCanonical()
{
    const cg::packet::PacketResult<BattleEntryNotification> parsed =
        BattleEntryNotification::Parse("dAgg 2 10 ");

    int failures = Expect(parsed.HasValue(), "dAgg typed parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().battleType == 2, "dAgg parsed battletype");
    failures += Expect(parsed.Value().fieldNumber == 62, "dAgg parsed fieldno");

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value().ToPacket(), BattleEntryNotification::Spec);
    failures += Expect(built.HasValue(), "dAgg typed build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "dAgg 2 10 ", "dAgg canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("dAgg 2 10", BattleEntryNotification::Spec);

    int failures = Expect(parsed.HasValue(), "dAgg generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value(), BattleEntryNotification::Spec);
    failures += Expect(built.HasValue(), "dAgg generic build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "dAgg 2 10 ", "dAgg generic canonical");
    }
    return failures;
}

int TestIntValuesArePreserved()
{
    const cg::packet::PacketResult<BattleEntryNotification> zero =
        BattleEntryNotification::Parse("dAgg 0 0 ");
    const cg::packet::PacketResult<BattleEntryNotification> negative =
        BattleEntryNotification::Parse("dAgg -1 -2 ");
    const cg::packet::PacketResult<BattleEntryNotification> nonBoolean =
        BattleEntryNotification::Parse("dAgg 3 4 ");

    int failures = Expect(zero.HasValue(), "dAgg zero values parse");
    if (zero.HasValue()) {
        failures += Expect(zero.Value().battleType == 0, "dAgg zero battletype");
        failures += Expect(zero.Value().fieldNumber == 0, "dAgg zero fieldno");
    }
    failures += Expect(negative.HasValue(), "dAgg negative values parse");
    if (negative.HasValue()) {
        failures += Expect(negative.Value().battleType == -1, "dAgg negative battletype");
        failures += Expect(negative.Value().fieldNumber == -2, "dAgg negative fieldno");
    }
    failures += Expect(nonBoolean.HasValue(), "dAgg non-boolean values parse");
    if (nonBoolean.HasValue()) {
        failures += Expect(nonBoolean.Value().battleType == 3, "dAgg non-boolean battletype");
        failures += Expect(nonBoolean.Value().fieldNumber == 4, "dAgg non-boolean fieldno");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        BattleEntryNotification::Parse("bad 2 10 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "dAgg wrong code");
    failures += ExpectError(
        BattleEntryNotification::Parse("dAgg 2"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "dAgg",
        true,
        1U,
        "dAgg missing field");
    failures += ExpectError(
        BattleEntryNotification::Parse("dAgg 2 10 0 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "dAgg",
        true,
        2U,
        "dAgg extra field");
    failures += ExpectError(
        BattleEntryNotification::Parse("dAgg _ 10 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "dAgg",
        true,
        0U,
        "dAgg invalid battletype");
    failures += ExpectError(
        BattleEntryNotification::Parse("dAgg 2 _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "dAgg",
        true,
        1U,
        "dAgg invalid fieldno");
    failures += ExpectError(
        BattleEntryNotification::Parse("dAgg 2lkCB2 10 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "dAgg",
        true,
        0U,
        "dAgg battletype overflow");
    failures += ExpectError(
        BattleEntryNotification::Parse("dAgg 2 2lkCB2 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "dAgg",
        true,
        1U,
        "dAgg fieldno overflow");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndToPacket()
        + TestTypedParseAndCanonical()
        + TestGenericRoundtrip()
        + TestIntValuesArePreserved()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " dAgg battle entry notification packet test(s) failed\n";
        return 1;
    }
    return 0;
}
