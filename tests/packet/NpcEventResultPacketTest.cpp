#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <string_view>

namespace {

using cg::packet::receive::NpcEventResult;

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
    const NpcEventResult eventResult(62, -1);
    const cg::packet::Packet packet = eventResult.ToPacket();

    int failures = 0;
    failures += Expect(NpcEventResult::Code == "EZ", "EZ code");
    failures += Expect(
        NpcEventResult::Direction == cg::packet::PacketDirection::ServerToClient,
        "EZ direction");
    failures += Expect(NpcEventResult::Fields.size() == 2U, "EZ fields");
    failures += Expect(NpcEventResult::Fields[0].name == "seqno", "EZ seqno name");
    failures += Expect(
        NpcEventResult::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "EZ seqno type");
    failures += Expect(NpcEventResult::Fields[1].name == "result", "EZ result name");
    failures += Expect(
        NpcEventResult::Fields[1].type == cg::packet::PacketFieldType::Int62,
        "EZ result type");
    failures += Expect(NpcEventResult::Spec.code == NpcEventResult::Code, "EZ Spec code");
    failures += Expect(
        NpcEventResult::Spec.direction == NpcEventResult::Direction,
        "EZ Spec direction");
    failures += Expect(NpcEventResult::Spec.fieldCount == 2U, "EZ Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("EZ"), "EZ packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ServerToClient,
        "EZ packet direction");
    failures += Expect(packet.fields.size() == 2U, "EZ packet fields");
    if (packet.fields.size() == 2U) {
        failures += Expect(packet.fields[0].name == "seqno", "EZ packet seqno name");
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            "EZ packet seqno type");
        failures += Expect(packet.fields[0].value == "62", "EZ packet seqno value");
        failures += Expect(packet.fields[1].name == "result", "EZ packet result name");
        failures += Expect(
            packet.fields[1].type == cg::packet::PacketFieldType::Int62,
            "EZ packet result type");
        failures += Expect(packet.fields[1].value == "-1", "EZ packet result value");
    }
    return failures;
}

int TestTypedParseAndCanonical()
{
    const cg::packet::PacketResult<NpcEventResult> parsed =
        NpcEventResult::Parse("EZ 10 -1 ");

    int failures = Expect(parsed.HasValue(), "EZ typed parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().sequenceNumber == 62, "EZ parsed seqno");
    failures += Expect(parsed.Value().result == -1, "EZ parsed result");

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value().ToPacket(), NpcEventResult::Spec);
    failures += Expect(built.HasValue(), "EZ typed build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "EZ 10 -1 ", "EZ canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("EZ 10 -1", NpcEventResult::Spec);

    int failures = Expect(parsed.HasValue(), "EZ generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value(), NpcEventResult::Spec);
    failures += Expect(built.HasValue(), "EZ generic build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "EZ 10 -1 ", "EZ generic canonical");
    }
    return failures;
}

int TestResultPreservesInt()
{
    const cg::packet::PacketResult<NpcEventResult> zero =
        NpcEventResult::Parse("EZ 1 0 ");
    const cg::packet::PacketResult<NpcEventResult> nonZero =
        NpcEventResult::Parse("EZ 1 2 ");

    int failures = Expect(zero.HasValue(), "EZ zero result parse");
    if (zero.HasValue()) {
        failures += Expect(zero.Value().result == 0, "EZ zero result");
    }
    failures += Expect(nonZero.HasValue(), "EZ nonzero result parse");
    if (nonZero.HasValue()) {
        failures += Expect(nonZero.Value().result == 2, "EZ nonzero result preserved");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        NpcEventResult::Parse("bad 10 -1 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "EZ wrong code");
    failures += ExpectError(
        NpcEventResult::Parse("EZ 10"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "EZ",
        true,
        1U,
        "EZ missing field");
    failures += ExpectError(
        NpcEventResult::Parse("EZ 10 -1 0 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "EZ",
        true,
        2U,
        "EZ extra field");
    failures += ExpectError(
        NpcEventResult::Parse("EZ _ -1 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "EZ",
        true,
        0U,
        "EZ invalid seqno");
    failures += ExpectError(
        NpcEventResult::Parse("EZ 10 _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "EZ",
        true,
        1U,
        "EZ invalid result");
    failures += ExpectError(
        NpcEventResult::Parse("EZ 2lkCB2 -1 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "EZ",
        true,
        0U,
        "EZ seqno overflow");
    failures += ExpectError(
        NpcEventResult::Parse("EZ 10 2lkCB2 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "EZ",
        true,
        1U,
        "EZ result overflow");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndToPacket()
        + TestTypedParseAndCanonical()
        + TestGenericRoundtrip()
        + TestResultPreservesInt()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " EZ NPC event result packet test(s) failed\n";
        return 1;
    }
    return 0;
}
