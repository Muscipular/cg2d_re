#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

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
    const char* message)
{
    int failures = Expect(!static_cast<bool>(result), message);
    failures += Expect(result.HasError(), message);
    if (!result.HasError()) {
        return failures;
    }

    const cg::packet::PacketError& error = result.Error();
    failures += Expect(error.code == code, message);
    failures += Expect(error.packetCode.has_value(), message);
    if (error.packetCode.has_value()) {
        failures += Expect(error.packetCode->View() == packetCode, message);
    }
    return failures;
}

std::string BuildPlainTnt(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return "TNT ";
    }

    std::string plain = "TNT ";
    plain.append(encoded.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectPartyParamPacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::PartyParam::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[0].value == value, message);
    return failures;
}

int ExpectPartyParamEntry(
    const cg::packet::receive::PartyParamEntry& entry,
    int objectIndex,
    int hp,
    int maxHp,
    int forcePoint,
    int maxForcePoint,
    const char* message)
{
    int failures = 0;
    failures += Expect(entry.objectIndex == objectIndex, message);
    failures += Expect(entry.hp == hp, message);
    failures += Expect(entry.maxHp == maxHp, message);
    failures += Expect(entry.forcePoint == forcePoint, message);
    failures += Expect(entry.maxForcePoint == maxForcePoint, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = "a|100|120|30|40|";
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(BuildPlainTnt(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse TNT outer");
    if (parsed.HasValue()) {
        failures += ExpectPartyParamPacket(parsed.Value(), payload, "parse TNT outer packet");
    }
    return failures;
}

int TestSingleEntry()
{
    const cg::packet::PacketResult<cg::packet::receive::PartyParam> parsed =
        cg::packet::receive::PartyParam::Parse(BuildPlainTnt("a|100|120|30|40|"));

    int failures = Expect(static_cast<bool>(parsed), "parse TNT single entry");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().entries.size() == 1U, "TNT single entry count");
    if (parsed.Value().entries.size() == 1U) {
        failures += ExpectPartyParamEntry(parsed.Value().entries[0], 10, 100, 120, 30, 40, "TNT single entry value");
    }
    return failures;
}

int TestMultiEntryOrder()
{
    const cg::packet::PacketResult<cg::packet::receive::PartyParam> parsed =
        cg::packet::receive::PartyParam::Parse(BuildPlainTnt("1|10|20|30|40|2|50|60|70|80|"));

    int failures = Expect(static_cast<bool>(parsed), "parse TNT multi entry");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().entries.size() == 2U, "TNT multi entry count");
    if (parsed.Value().entries.size() == 2U) {
        failures += ExpectPartyParamEntry(parsed.Value().entries[0], 1, 10, 20, 30, 40, "TNT first entry");
        failures += ExpectPartyParamEntry(parsed.Value().entries[1], 2, 50, 60, 70, 80, "TNT second entry");
    }
    return failures;
}

int TestRoundtripPreservesTrailingDelimiter()
{
    std::vector<cg::packet::receive::PartyParamEntry> entries;
    entries.push_back(cg::packet::receive::PartyParamEntry {10, 100, 120, 30, 40});
    entries.push_back(cg::packet::receive::PartyParamEntry {2, 50, 60, 70, 80});
    const cg::packet::receive::PartyParam partyParam(std::move(entries));
    const cg::packet::Packet packet = partyParam.ToPacket();

    int failures = ExpectPartyParamPacket(packet, "a|100|120|30|40|2|50|60|70|80|", "TNT ToPacket");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build TNT packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(
        built.Value() == BuildPlainTnt("a|100|120|30|40|2|50|60|70|80|"),
        "build TNT packet preserves trailing delimiter");

    const cg::packet::PacketResult<cg::packet::receive::PartyParam> parsed =
        cg::packet::receive::PartyParam::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip TNT packet");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().entries.size() == 2U, "roundtrip TNT entry count");
    }
    return failures;
}

std::string BuildSixEntriesPayload()
{
    std::string payload;
    for (int index = 1; index <= 6; ++index) {
        payload.append(std::to_string(index));
        payload.append("|10|20|30|40|");
    }
    return payload;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::PartyParam::Parse(BuildPlainTnt("a|100|120|30|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TNT",
        "TNT entry field count");
    failures += ExpectError(
        cg::packet::receive::PartyParam::Parse(BuildPlainTnt("_|100|120|30|40|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TNT",
        "TNT bad objectIndex i62");
    failures += ExpectError(
        cg::packet::receive::PartyParam::Parse(BuildPlainTnt("a|x|120|30|40|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TNT",
        "TNT bad hp decimal");
    failures += ExpectError(
        cg::packet::receive::PartyParam::Parse(BuildPlainTnt("a|100|x|30|40|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TNT",
        "TNT bad maxHp decimal");
    failures += ExpectError(
        cg::packet::receive::PartyParam::Parse(BuildPlainTnt("a|100|120|x|40|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TNT",
        "TNT bad forcePoint decimal");
    failures += ExpectError(
        cg::packet::receive::PartyParam::Parse(BuildPlainTnt("a|100|120|30|x|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TNT",
        "TNT bad maxForcePoint decimal");
    failures += ExpectError(
        cg::packet::receive::PartyParam::Parse(BuildPlainTnt(BuildSixEntriesPayload())),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TNT",
        "TNT entry count over 5");
    failures += ExpectError(
        cg::packet::receive::PartyParam::Parse(BuildPlainTnt("a|100|120|30|40")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TNT",
        "TNT missing trailing delimiter");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestSingleEntry();
    failures += TestMultiEntryOrder();
    failures += TestRoundtripPreservesTrailingDelimiter();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
