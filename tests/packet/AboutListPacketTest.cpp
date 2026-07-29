#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

using AboutList = cg::packet::receive::AboutList;
using AboutListEntry = cg::packet::receive::AboutListEntry;

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, AboutList::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, AboutList::Spec);
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
        failures += Expect(error.packetCode->View() == AboutList::Code, message);
    }
    return failures;
}

template <typename T>
int ExpectErrorAt(
    const cg::packet::PacketResult<T>& result,
    cg::packet::PacketErrorCode code,
    std::size_t fieldIndex,
    const char* message)
{
    int failures = ExpectError(result, code, message);
    if (result.HasError()) {
        failures += Expect(result.Error().fieldIndex.has_value(), message);
        if (result.Error().fieldIndex.has_value()) {
            failures += Expect(result.Error().fieldIndex.value() == fieldIndex, message);
        }
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
    cg::packet::PacketFieldType fieldType,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(AboutList::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == fieldType, message);
    failures += Expect(packet.fields[0].value == value, message);
    return failures;
}

int ExpectAboutListEntry(
    const AboutListEntry& entry,
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

int TestGenericOuterRoundtrip()
{
    const std::string payload = "1|10|100|Hero";
    const std::string plain = BuildPlainIUn(payload);
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(plain);

    int failures = Expect(static_cast<bool>(parsed), "parse IUn generic outer");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectAboutListPacket(
        parsed.Value(),
        payload,
        cg::packet::PacketFieldType::EscapedString,
        "parse IUn generic outer field");
    const cg::packet::PacketResult<std::string> rebuilt = BuildPacket(parsed.Value());
    failures += Expect(static_cast<bool>(rebuilt), "build IUn generic outer");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == plain, "roundtrip IUn generic outer");
    }
    return failures;
}

int TestZeroAndMultipleEntries()
{
    int failures = 0;
    const cg::packet::PacketResult<AboutList> empty =
        AboutList::Parse(BuildPlainIUn("1"));
    failures += Expect(static_cast<bool>(empty), "parse IUn version-only");
    if (empty.HasValue()) {
        failures += Expect(empty.Value().version == 1, "parse IUn version-only version");
        failures += Expect(empty.Value().entries.empty(), "parse IUn version-only entries");
        const cg::packet::PacketResult<std::string> data = empty.Value().BuildData();
        failures += Expect(static_cast<bool>(data), "build IUn version-only");
        if (data.HasValue()) {
            failures += Expect(data.Value() == "1", "build IUn version-only payload");
        }
    }

    const std::string payload = "-1|-2|3|Hero|4|-5|Mage";
    const cg::packet::PacketResult<AboutList> multiple =
        AboutList::Parse(BuildPlainIUn(payload));
    failures += Expect(static_cast<bool>(multiple), "parse IUn multiple entries");
    if (!multiple.HasValue()) {
        return failures;
    }

    failures += Expect(multiple.Value().version == -1, "parse IUn multiple version");
    failures += Expect(multiple.Value().entries.size() == 2U, "parse IUn multiple count");
    if (multiple.Value().entries.size() == 2U) {
        failures += ExpectAboutListEntry(multiple.Value().entries[0], -2, 3, "Hero", "parse IUn entry 0");
        failures += ExpectAboutListEntry(multiple.Value().entries[1], 4, -5, "Mage", "parse IUn entry 1");
    }
    const cg::packet::PacketResult<std::string> rebuilt = multiple.Value().BuildData();
    failures += Expect(static_cast<bool>(rebuilt), "build IUn multiple entries");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == payload, "roundtrip IUn multiple entries");
    }
    return failures;
}

int TestEmptyItemName()
{
    const std::string payload = "1|2|3|";
    const cg::packet::PacketResult<AboutList> parsed =
        AboutList::Parse(BuildPlainIUn(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse IUn empty itemName");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().entries.size() == 1U, "parse IUn empty itemName count");
    if (parsed.Value().entries.size() == 1U) {
        failures += ExpectAboutListEntry(parsed.Value().entries[0], 2, 3, "", "parse IUn empty itemName entry");
    }
    const cg::packet::PacketResult<std::string> rebuilt = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(rebuilt), "build IUn empty itemName");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == payload, "roundtrip IUn empty itemName");
    }
    return failures;
}

int TestLegacyEscapeCanonical()
{
    const std::string legacy =
        "7|10|20|line\\ncomma\\cpipe\\zslash\\yunknown\\q";
    const std::string semantic =
        "line\ncomma,pipe|slash\\unknownq";
    const std::string canonical =
        "7|10|20|line\\ncomma\\cpipe\\zslash\\yunknownq";
    const cg::packet::PacketResult<AboutList> parsed =
        AboutList::Parse(BuildPlainIUn(legacy));

    int failures = Expect(static_cast<bool>(parsed), "parse IUn legacy escapes");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().entries.size() == 1U, "parse IUn legacy escape count");
    if (parsed.Value().entries.size() == 1U) {
        failures += ExpectAboutListEntry(
            parsed.Value().entries[0],
            10,
            20,
            semantic,
            "decode IUn legacy n/c/z/y and unknown");
    }

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "build IUn legacy canonical");
    if (data.HasValue()) {
        failures += Expect(data.Value() == canonical, "canonicalize IUn legacy escapes");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "IUn legacy ToPacket");
    if (packet.HasValue()) {
        failures += ExpectAboutListPacket(
            packet.Value(),
            canonical,
            cg::packet::PacketFieldType::SubPayload,
            "IUn legacy ToPacket field");
        const cg::packet::PacketResult<std::string> wire = BuildPacket(packet.Value());
        failures += Expect(static_cast<bool>(wire), "build IUn legacy wire");
        if (wire.HasValue()) {
            failures += Expect(wire.Value() == BuildPlainIUn(canonical), "canonical IUn legacy wire");
        }
    }
    return failures;
}

int TestGbkTrailDelimiter()
{
    std::string itemName;
    itemName.push_back(static_cast<char>(0x81));
    itemName.push_back(static_cast<char>(0x7c));

    std::string payload = "1|2|3|";
    payload.append(itemName);
    const cg::packet::PacketResult<AboutList> parsed =
        AboutList::Parse(BuildPlainIUn(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse IUn GBK 81 7C");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().entries.size() == 1U, "parse IUn GBK entry count");
    if (parsed.Value().entries.size() == 1U) {
        failures += ExpectAboutListEntry(parsed.Value().entries[0], 2, 3, itemName, "parse IUn GBK itemName");
    }
    const cg::packet::PacketResult<std::string> rebuilt = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(rebuilt), "build IUn GBK 81 7C");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == payload, "roundtrip IUn GBK 81 7C");
    }
    return failures;
}

int TestIntegerLimits()
{
    const std::string payload =
        "-2147483648|2147483647|-2147483648|Limit";
    const cg::packet::PacketResult<AboutList> parsed =
        AboutList::Parse(BuildPlainIUn(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse IUn int limits");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(
        parsed.Value().version == std::numeric_limits<int>::min(),
        "parse IUn min version");
    failures += Expect(parsed.Value().entries.size() == 1U, "parse IUn int limit entry count");
    if (parsed.Value().entries.size() == 1U) {
        failures += ExpectAboutListEntry(
            parsed.Value().entries[0],
            std::numeric_limits<int>::max(),
            std::numeric_limits<int>::min(),
            "Limit",
            "parse IUn item integer limits");
    }

    const cg::packet::PacketResult<AboutList> maxVersion =
        AboutList::Parse(BuildPlainIUn("2147483647"));
    failures += Expect(static_cast<bool>(maxVersion), "parse IUn max version");
    if (maxVersion.HasValue()) {
        failures += Expect(
            maxVersion.Value().version == std::numeric_limits<int>::max(),
            "parse IUn max version value");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("IUn"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "generic IUn missing outer field");
    failures += ExpectError(
        AboutList::Parse("IUn"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "typed IUn missing outer field");
    failures += ExpectError(
        ParsePacket("IUn one two "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "generic IUn extra outer field");
    failures += ExpectError(
        AboutList::Parse("IUn one two "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "typed IUn extra outer field");
    failures += ExpectError(
        AboutList::Parse(BuildPlainIUn("")),
        cg::packet::PacketErrorCode::EmptyInput,
        "typed IUn empty payload");
    failures += ExpectError(
        AboutList::Parse(BuildPlainIUn("+1")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "typed IUn invalid decimal sign");
    failures += ExpectError(
        AboutList::Parse(BuildPlainIUn("x")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "typed IUn invalid decimal character");
    failures += ExpectError(
        AboutList::Parse(BuildPlainIUn("2147483648")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "typed IUn positive decimal overflow");
    failures += ExpectError(
        AboutList::Parse(BuildPlainIUn("-2147483649")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "typed IUn negative decimal overflow");
    failures += ExpectError(
        AboutList::Parse(BuildPlainIUn("1|2|3")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "typed IUn missing entry field");
    failures += ExpectError(
        AboutList::Parse(BuildPlainIUn("1|2|3|Name|extra")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "typed IUn extra entry field");
    failures += ExpectError(
        AboutList::Parse(BuildPlainIUn("1|2|3|Name|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "typed IUn extra trailing delimiter");
    failures += ExpectErrorAt(
        AboutList::Parse(BuildPlainIUn("1|bad|3|Name")),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "typed IUn invalid itemId");
    failures += ExpectErrorAt(
        AboutList::Parse(BuildPlainIUn("1|2|bad|Name")),
        cg::packet::PacketErrorCode::DecodeFailed,
        2U,
        "typed IUn invalid graphicNo");
    failures += ExpectErrorAt(
        AboutList::Parse(BuildPlainIUn("1|2|3|Name\\")),
        cg::packet::PacketErrorCode::DecodeFailed,
        3U,
        "typed IUn truncated legacy escape");

    std::string truncatedGbk = "IUn 1|2|3|";
    truncatedGbk.push_back(static_cast<char>(0x81));
    failures += ExpectErrorAt(
        AboutList::Parse(truncatedGbk),
        cg::packet::PacketErrorCode::DecodeFailed,
        0U,
        "typed IUn truncated GBK");
    return failures;
}

int TestBuildAndToPacketErrors()
{
    std::string truncatedGbk(1U, static_cast<char>(0x81));
    const AboutList aboutList(
        1,
        {
            AboutListEntry(2, 3, "Valid"),
            AboutListEntry(4, 5, std::move(truncatedGbk)),
        });

    int failures = 0;
    failures += ExpectErrorAt(
        aboutList.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        6U,
        "IUn BuildData truncated GBK context");
    failures += ExpectErrorAt(
        aboutList.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        6U,
        "IUn ToPacket truncated GBK context");
    return failures;
}

int TestCanonicalToPacket()
{
    const AboutList aboutList(
        1,
        {
            AboutListEntry(10, 100, "Hero"),
            AboutListEntry(20, 200, ""),
        });
    const std::string payload = "1|10|100|Hero|20|200|";
    const cg::packet::PacketResult<cg::packet::Packet> packet = aboutList.ToPacket();

    int failures = Expect(static_cast<bool>(packet), "IUn canonical ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    failures += ExpectAboutListPacket(
        packet.Value(),
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "IUn canonical ToPacket field");
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build IUn canonical packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainIUn(payload), "build IUn canonical wire");
    }
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestGenericOuterRoundtrip();
    failures += TestZeroAndMultipleEntries();
    failures += TestEmptyItemName();
    failures += TestLegacyEscapeCanonical();
    failures += TestGbkTrailDelimiter();
    failures += TestIntegerLimits();
    failures += TestParseErrors();
    failures += TestBuildAndToPacketErrors();
    failures += TestCanonicalToPacket();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
