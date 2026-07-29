#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::NpcEventRequest;

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

int ExpectField(
    const cg::packet::Packet& packet,
    std::size_t index,
    std::string_view name,
    std::string_view value,
    const char* message)
{
    int failures = Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }
    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(packet.fields[index].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

std::string BuildPlain(const std::array<std::string, 5U>& tokens)
{
    return "pg " + tokens[0] + ' ' + tokens[1] + ' ' + tokens[2] + ' ' + tokens[3] + ' '
        + tokens[4] + ' ';
}

int TestSpecAndEncode()
{
    const NpcEventRequest request(62, -1, 36, 1, 0);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(NpcEventRequest::Code == "pg", "pg code");
    failures += Expect(
        NpcEventRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "pg direction");
    failures += Expect(NpcEventRequest::Fields.size() == 5U, "pg fields");
    failures += Expect(NpcEventRequest::Fields[0].name == "event", "pg event name");
    failures += Expect(NpcEventRequest::Fields[1].name == "seqno", "pg seqno name");
    failures += Expect(NpcEventRequest::Fields[2].name == "x", "pg x name");
    failures += Expect(NpcEventRequest::Fields[3].name == "y", "pg y name");
    failures += Expect(NpcEventRequest::Fields[4].name == "dir", "pg dir name");
    for (const cg::packet::PacketFieldSpec& field : NpcEventRequest::Fields) {
        failures += Expect(field.type == cg::packet::PacketFieldType::Int62, "pg field type");
    }
    failures += Expect(NpcEventRequest::Spec.code == NpcEventRequest::Code, "pg Spec code");
    failures += Expect(
        NpcEventRequest::Spec.direction == NpcEventRequest::Direction,
        "pg Spec direction");
    failures += Expect(NpcEventRequest::Spec.fieldCount == 5U, "pg Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("pg"), "pg packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "pg packet direction");
    failures += Expect(packet.fields.size() == 5U, "pg packet fields");
    failures += ExpectField(packet, 0U, "event", "62", "pg event field");
    failures += ExpectField(packet, 1U, "seqno", "-1", "pg seqno field");
    failures += ExpectField(packet, 2U, "x", "36", "pg x field");
    failures += ExpectField(packet, 3U, "y", "1", "pg y field");
    failures += ExpectField(packet, 4U, "dir", "0", "pg dir field");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "pg Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "pg 10 -1 A 1 0 ", "pg Encode value");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("pg 10 -1 A 1 0 ", NpcEventRequest::Spec);

    int failures = Expect(parsed.HasValue(), "pg ParsePacket");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectField(parsed.Value(), 0U, "event", "62", "pg parsed event");
    failures += ExpectField(parsed.Value(), 1U, "seqno", "-1", "pg parsed seqno");
    failures += ExpectField(parsed.Value(), 2U, "x", "36", "pg parsed x");
    failures += ExpectField(parsed.Value(), 3U, "y", "1", "pg parsed y");
    failures += ExpectField(parsed.Value(), 4U, "dir", "0", "pg parsed dir");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), NpcEventRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "pg BuildPacket");
    if (!rebuilt.HasValue()) {
        return failures;
    }
    failures += Expect(rebuilt.Value() == "pg 10 -1 A 1 0 ", "pg canonical");

    const cg::packet::PacketResult<cg::packet::Packet> reparsed =
        cg::packet::ParsePacket(rebuilt.Value(), NpcEventRequest::Spec);
    failures += Expect(reparsed.HasValue(), "pg reparse canonical");
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad 10 -1 A 1 0 ", NpcEventRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "pg wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("pg 10 -1 A 1", NpcEventRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "pg",
        true,
        4U,
        "pg missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("pg 10 -1 A 1 0 2 ", NpcEventRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "pg",
        true,
        5U,
        "pg extra field");

    const std::array<std::string, 5U> validTokens = {"10", "-1", "A", "1", "0"};
    for (std::size_t index = 0U; index < validTokens.size(); ++index) {
        std::array<std::string, 5U> invalidTokens = validTokens;
        invalidTokens[index] = "_";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(invalidTokens), NpcEventRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "pg",
            true,
            index,
            "pg invalid i62");

        std::array<std::string, 5U> overflowTokens = validTokens;
        overflowTokens[index] = "2lkCB2";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(overflowTokens), NpcEventRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "pg",
            true,
            index,
            "pg i62 overflow");
    }
    return failures;
}

} // namespace

int main()
{
    const int failures = TestSpecAndEncode() + TestGenericRoundtrip() + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " NPC event request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
