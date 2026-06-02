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

std::string BuildPlainIUn(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return "IUn ";
    }

    std::string plain = "IUn ";
    plain.append(encoded.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectAboutListPacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::AboutList::Code), message);
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

int ExpectAboutListEntry(
    const cg::packet::receive::AboutListEntry& entry,
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
        cg::packet::ParsePacket(BuildPlainIUn("1|10|100|Hero|20|200|Mage"));

    int failures = Expect(static_cast<bool>(parsed), "parse IUn outer");
    if (parsed.HasValue()) {
        failures += ExpectAboutListPacket(parsed.Value(), "1|10|100|Hero|20|200|Mage", "parse IUn outer packet");
    }
    return failures;
}

int TestTypedParseEntries()
{
    const cg::packet::PacketResult<cg::packet::receive::AboutList> parsed =
        cg::packet::receive::AboutList::Parse(BuildPlainIUn("1|10|100|Hero|20|200|Mage"));

    int failures = Expect(static_cast<bool>(parsed), "typed parse IUn entries");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().version == 1, "typed IUn version");
    failures += Expect(parsed.Value().entries.size() == 2U, "typed IUn entry count");
    if (parsed.Value().entries.size() == 2U) {
        failures += ExpectAboutListEntry(parsed.Value().entries[0], 10, 100, "Hero", "typed IUn first entry");
        failures += ExpectAboutListEntry(parsed.Value().entries[1], 20, 200, "Mage", "typed IUn second entry");
    }
    return failures;
}

int TestRoundtrip()
{
    std::vector<cg::packet::receive::AboutListEntry> entries;
    entries.emplace_back(10, 100, "Hero");
    entries.emplace_back(20, 200, "Mage");
    const cg::packet::receive::AboutList aboutList(1, std::move(entries));
    const cg::packet::Packet packet = aboutList.ToPacket();

    int failures = ExpectAboutListPacket(packet, "1|10|100|Hero|20|200|Mage", "IUn ToPacket");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build IUn packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == BuildPlainIUn("1|10|100|Hero|20|200|Mage"), "build IUn packet value");

    const cg::packet::PacketResult<cg::packet::receive::AboutList> parsed =
        cg::packet::receive::AboutList::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip IUn packet");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().version == 1, "roundtrip IUn version");
        failures += Expect(parsed.Value().entries.size() == 2U, "roundtrip IUn entry count");
    }
    return failures;
}

int TestEmptyList()
{
    const cg::packet::PacketResult<cg::packet::receive::AboutList> parsed =
        cg::packet::receive::AboutList::Parse(BuildPlainIUn("5"));

    int failures = Expect(static_cast<bool>(parsed), "parse IUn empty list");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().version == 5, "parse IUn empty version");
        failures += Expect(parsed.Value().entries.empty(), "parse IUn empty entries");
        failures += ExpectAboutListPacket(parsed.Value().ToPacket(), "5", "IUn empty ToPacket");
    }
    return failures;
}

int TestEscapedItemNameRoundtrip()
{
    const std::string payload = "2|11|101|Hero Name";
    const cg::packet::PacketResult<cg::packet::receive::AboutList> parsed =
        cg::packet::receive::AboutList::Parse(BuildPlainIUn(payload));

    int failures = Expect(static_cast<bool>(parsed), "typed parse IUn escaped itemName");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().entries.size() == 1U, "typed IUn escaped entry count");
    if (parsed.Value().entries.size() == 1U) {
        failures += ExpectAboutListEntry(parsed.Value().entries[0], 11, 101, "Hero Name", "typed IUn escaped itemName");
    }

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build IUn escaped itemName");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainIUn(payload), "roundtrip IUn escaped itemName");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::AboutList::Parse("IUn"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IUn",
        "typed IUn missing data");
    failures += ExpectError(
        cg::packet::receive::AboutList::Parse(BuildPlainIUn("")),
        cg::packet::PacketErrorCode::EmptyInput,
        "IUn",
        "typed IUn empty payload");
    failures += ExpectError(
        cg::packet::receive::AboutList::Parse(BuildPlainIUn("x")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IUn",
        "typed IUn bad version decimal");
    failures += ExpectError(
        cg::packet::receive::AboutList::Parse(BuildPlainIUn("1|10|100")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IUn",
        "typed IUn entry field count");
    failures += ExpectError(
        cg::packet::receive::AboutList::Parse(BuildPlainIUn("1|x|100|Hero")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IUn",
        "typed IUn bad itemId decimal");
    failures += ExpectError(
        cg::packet::receive::AboutList::Parse(BuildPlainIUn("1|10|x|Hero")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IUn",
        "typed IUn bad graphicNo decimal");
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
