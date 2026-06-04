#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::ProfileList::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::ProfileList::Spec);
}

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

std::string BuildPlainEzt(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return "eZT ";
    }

    std::string plain = "eZT ";
    plain.append(encoded.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectProfileListPacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::ProfileList::Code), message);
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

int ExpectProfileListEntry(
    const cg::packet::receive::ProfileListEntry& entry,
    int itemId,
    int graphicNo,
    std::string_view itemName,
    const char* message)
{
    int failures = 0;
    failures += Expect(entry.itemId == itemId, message);
    failures += Expect(entry.graphicNo == graphicNo, message);
    failures += Expect(entry.itemName == itemName, message);
    return failures;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlainEzt("1|10|100|Hero|20|200|Mage"));

    int failures = Expect(static_cast<bool>(parsed), "parse eZT outer");
    if (parsed.HasValue()) {
        failures += ExpectProfileListPacket(parsed.Value(), "1|10|100|Hero|20|200|Mage", "parse eZT outer packet");
    }
    return failures;
}

int TestTypedParseEntries()
{
    const cg::packet::PacketResult<cg::packet::receive::ProfileList> parsed =
        cg::packet::receive::ProfileList::Parse(BuildPlainEzt("1|10|100|Hero|20|200|Mage"));

    int failures = Expect(static_cast<bool>(parsed), "typed parse eZT entries");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().version == 1, "typed eZT version");
    failures += Expect(parsed.Value().entries.size() == 2U, "typed eZT entry count");
    if (parsed.Value().entries.size() == 2U) {
        failures += ExpectProfileListEntry(parsed.Value().entries[0], 10, 100, "Hero", "typed eZT first entry");
        failures += ExpectProfileListEntry(parsed.Value().entries[1], 20, 200, "Mage", "typed eZT second entry");
    }
    return failures;
}

int TestRoundtrip()
{
    std::vector<cg::packet::receive::ProfileListEntry> entries;
    entries.emplace_back(10, 100, "Hero");
    entries.emplace_back(20, 200, "Mage");
    const cg::packet::receive::ProfileList profileList(1, std::move(entries));
    const cg::packet::Packet packet = profileList.ToPacket();

    int failures = ExpectProfileListPacket(packet, "1|10|100|Hero|20|200|Mage", "eZT ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build eZT packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == BuildPlainEzt("1|10|100|Hero|20|200|Mage"), "build eZT packet value");

    const cg::packet::PacketResult<cg::packet::receive::ProfileList> parsed =
        cg::packet::receive::ProfileList::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip eZT packet");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().version == 1, "roundtrip eZT version");
        failures += Expect(parsed.Value().entries.size() == 2U, "roundtrip eZT entry count");
    }
    return failures;
}

int TestEmptyList()
{
    const cg::packet::PacketResult<cg::packet::receive::ProfileList> parsed =
        cg::packet::receive::ProfileList::Parse(BuildPlainEzt("1"));

    int failures = Expect(static_cast<bool>(parsed), "parse eZT empty list");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().version == 1, "parse eZT empty version");
        failures += Expect(parsed.Value().entries.empty(), "parse eZT empty entries");
        failures += ExpectProfileListPacket(parsed.Value().ToPacket(), "1", "eZT empty ToPacket");
    }
    return failures;
}

int TestEscapedItemNameRoundtrip()
{
    const std::string payload = "2|11|101|Hero Name";
    const cg::packet::PacketResult<cg::packet::receive::ProfileList> parsed =
        cg::packet::receive::ProfileList::Parse(BuildPlainEzt(payload));

    int failures = Expect(static_cast<bool>(parsed), "typed parse eZT escaped itemName");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().entries.size() == 1U, "typed eZT escaped entry count");
    if (parsed.Value().entries.size() == 1U) {
        failures += ExpectProfileListEntry(parsed.Value().entries[0], 11, 101, "Hero Name", "typed eZT escaped itemName");
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build eZT escaped itemName");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainEzt(payload), "roundtrip eZT escaped itemName");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::ProfileList::Parse("eZT"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "eZT",
        "typed eZT missing data");
    failures += ExpectError(
        cg::packet::receive::ProfileList::Parse(BuildPlainEzt("x")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "eZT",
        "typed eZT bad version decimal");
    failures += ExpectError(
        cg::packet::receive::ProfileList::Parse(BuildPlainEzt("1|10|100")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "eZT",
        "typed eZT entry field count");
    failures += ExpectError(
        cg::packet::receive::ProfileList::Parse(BuildPlainEzt("1|x|100|Hero")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "eZT",
        "typed eZT bad itemId decimal");
    failures += ExpectError(
        cg::packet::receive::ProfileList::Parse(BuildPlainEzt("1|10|x|Hero")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "eZT",
        "typed eZT bad graphicNo decimal");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestTypedParseEntries();
    failures += TestRoundtrip();
    failures += TestEmptyList();
    failures += TestEscapedItemNameRoundtrip();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
