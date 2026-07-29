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

using ProfileList = cg::packet::receive::ProfileList;
using ProfileListEntry = cg::packet::receive::ProfileListEntry;

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, ProfileList::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, ProfileList::Spec);
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
        failures += Expect(error.packetCode->View() == ProfileList::Code, message);
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
    cg::packet::PacketFieldType fieldType,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(ProfileList::Code), message);
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

int ExpectEntry(
    const ProfileListEntry& entry,
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
    const std::string plain = BuildPlainEzt(payload);
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(plain);

    int failures = Expect(static_cast<bool>(parsed), "parse eZT generic outer");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectProfileListPacket(
        parsed.Value(),
        payload,
        cg::packet::PacketFieldType::EscapedString,
        "parse eZT generic outer field");
    const cg::packet::PacketResult<std::string> rebuilt = BuildPacket(parsed.Value());
    failures += Expect(static_cast<bool>(rebuilt), "build eZT generic outer");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == plain, "roundtrip eZT generic outer");
    }
    return failures;
}

int TestZeroAndMultipleEntries()
{
    int failures = 0;
    const cg::packet::PacketResult<ProfileList> empty =
        ProfileList::Parse(BuildPlainEzt("1"));
    failures += Expect(static_cast<bool>(empty), "parse eZT version-only");
    if (empty.HasValue()) {
        failures += Expect(empty.Value().version == 1, "parse eZT version-only version");
        failures += Expect(empty.Value().entries.empty(), "parse eZT version-only entries");
        const cg::packet::PacketResult<std::string> data = empty.Value().BuildData();
        failures += Expect(static_cast<bool>(data), "build eZT version-only");
        if (data.HasValue()) {
            failures += Expect(data.Value() == "1", "build eZT version-only payload");
        }
    }

    const std::string payload = "-1|-2|3|Hero|4|-5|Mage";
    const cg::packet::PacketResult<ProfileList> multiple =
        ProfileList::Parse(BuildPlainEzt(payload));
    failures += Expect(static_cast<bool>(multiple), "parse eZT multiple entries");
    if (!multiple.HasValue()) {
        return failures;
    }

    failures += Expect(multiple.Value().version == -1, "parse eZT multiple version");
    failures += Expect(multiple.Value().entries.size() == 2U, "parse eZT multiple count");
    if (multiple.Value().entries.size() == 2U) {
        failures += ExpectEntry(multiple.Value().entries[0], -2, 3, "Hero", "parse eZT entry 0");
        failures += ExpectEntry(multiple.Value().entries[1], 4, -5, "Mage", "parse eZT entry 1");
    }
    const cg::packet::PacketResult<std::string> rebuilt = multiple.Value().BuildData();
    failures += Expect(static_cast<bool>(rebuilt), "build eZT multiple entries");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == payload, "roundtrip eZT multiple entries");
    }
    return failures;
}

int TestEmptyItemName()
{
    const std::string payload = "1|2|3|";
    const cg::packet::PacketResult<ProfileList> parsed =
        ProfileList::Parse(BuildPlainEzt(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse eZT empty itemName");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().entries.size() == 1U, "parse eZT empty itemName count");
    if (parsed.Value().entries.size() == 1U) {
        failures += ExpectEntry(parsed.Value().entries[0], 2, 3, "", "parse eZT empty itemName entry");
    }
    const cg::packet::PacketResult<std::string> rebuilt = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(rebuilt), "build eZT empty itemName");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == payload, "roundtrip eZT empty itemName");
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
    const cg::packet::PacketResult<ProfileList> parsed =
        ProfileList::Parse(BuildPlainEzt(legacy));

    int failures = Expect(static_cast<bool>(parsed), "parse eZT legacy escapes");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().entries.size() == 1U, "parse eZT legacy escape count");
    if (parsed.Value().entries.size() == 1U) {
        failures += ExpectEntry(
            parsed.Value().entries[0],
            10,
            20,
            semantic,
            "decode eZT legacy n/c/z/y and unknown");
    }

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "build eZT legacy canonical");
    if (data.HasValue()) {
        failures += Expect(data.Value() == canonical, "canonicalize eZT legacy escapes");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "eZT legacy ToPacket");
    if (packet.HasValue()) {
        failures += ExpectProfileListPacket(
            packet.Value(),
            canonical,
            cg::packet::PacketFieldType::SubPayload,
            "eZT legacy ToPacket field");
        const cg::packet::PacketResult<std::string> wire = BuildPacket(packet.Value());
        failures += Expect(static_cast<bool>(wire), "build eZT legacy wire");
        if (wire.HasValue()) {
            failures += Expect(wire.Value() == BuildPlainEzt(canonical), "canonical eZT legacy wire");
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
    const cg::packet::PacketResult<ProfileList> parsed =
        ProfileList::Parse(BuildPlainEzt(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse eZT GBK 81 7C");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().entries.size() == 1U, "parse eZT GBK entry count");
    if (parsed.Value().entries.size() == 1U) {
        failures += ExpectEntry(parsed.Value().entries[0], 2, 3, itemName, "parse eZT GBK itemName");
    }
    const cg::packet::PacketResult<std::string> rebuilt = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(rebuilt), "build eZT GBK 81 7C");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == payload, "roundtrip eZT GBK 81 7C");
    }
    return failures;
}

int TestIntegerLimits()
{
    const std::string payload =
        "-2147483648|2147483647|-2147483648|Limit";
    const cg::packet::PacketResult<ProfileList> parsed =
        ProfileList::Parse(BuildPlainEzt(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse eZT int limits");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(
        parsed.Value().version == std::numeric_limits<int>::min(),
        "parse eZT min version");
    failures += Expect(parsed.Value().entries.size() == 1U, "parse eZT int limit entry count");
    if (parsed.Value().entries.size() == 1U) {
        failures += ExpectEntry(
            parsed.Value().entries[0],
            std::numeric_limits<int>::max(),
            std::numeric_limits<int>::min(),
            "Limit",
            "parse eZT item integer limits");
    }

    const cg::packet::PacketResult<ProfileList> maxVersion =
        ProfileList::Parse(BuildPlainEzt("2147483647"));
    failures += Expect(static_cast<bool>(maxVersion), "parse eZT max version");
    if (maxVersion.HasValue()) {
        failures += Expect(
            maxVersion.Value().version == std::numeric_limits<int>::max(),
            "parse eZT max version value");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("eZT"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "generic eZT missing outer field");
    failures += ExpectError(
        ProfileList::Parse("eZT"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "typed eZT missing outer field");
    failures += ExpectError(
        ParsePacket("eZT one two "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "generic eZT extra outer field");
    failures += ExpectError(
        ProfileList::Parse("eZT one two "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "typed eZT extra outer field");
    failures += ExpectError(
        ProfileList::Parse(BuildPlainEzt("")),
        cg::packet::PacketErrorCode::EmptyInput,
        "typed eZT empty payload");
    failures += ExpectError(
        ProfileList::Parse(BuildPlainEzt("+1")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "typed eZT invalid decimal sign");
    failures += ExpectError(
        ProfileList::Parse(BuildPlainEzt("x")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "typed eZT invalid decimal character");
    failures += ExpectError(
        ProfileList::Parse(BuildPlainEzt("2147483648")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "typed eZT positive decimal overflow");
    failures += ExpectError(
        ProfileList::Parse(BuildPlainEzt("-2147483649")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "typed eZT negative decimal overflow");
    failures += ExpectError(
        ProfileList::Parse(BuildPlainEzt("1|2|3")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "typed eZT missing entry field");
    failures += ExpectError(
        ProfileList::Parse(BuildPlainEzt("1|2|3|Name|extra")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "typed eZT extra entry field");
    failures += ExpectError(
        ProfileList::Parse(BuildPlainEzt("1|2|3|Name|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "typed eZT extra trailing delimiter");
    failures += ExpectErrorAt(
        ProfileList::Parse(BuildPlainEzt("1|bad|3|Name")),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "typed eZT invalid itemId");
    failures += ExpectErrorAt(
        ProfileList::Parse(BuildPlainEzt("1|2|bad|Name")),
        cg::packet::PacketErrorCode::DecodeFailed,
        2U,
        "typed eZT invalid graphicNo");
    failures += ExpectErrorAt(
        ProfileList::Parse(BuildPlainEzt("1|2|3|Name\\")),
        cg::packet::PacketErrorCode::DecodeFailed,
        3U,
        "typed eZT truncated legacy escape");

    std::string truncatedGbk = "eZT 1|2|3|";
    truncatedGbk.push_back(static_cast<char>(0x81));
    failures += ExpectErrorAt(
        ProfileList::Parse(truncatedGbk),
        cg::packet::PacketErrorCode::DecodeFailed,
        0U,
        "typed eZT truncated GBK");
    return failures;
}

int TestBuildAndToPacketErrors()
{
    std::string truncatedGbk(1U, static_cast<char>(0x81));
    const ProfileList profileList(
        1,
        {
            ProfileListEntry(2, 3, "Valid"),
            ProfileListEntry(4, 5, std::move(truncatedGbk)),
        });

    int failures = 0;
    failures += ExpectErrorAt(
        profileList.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        6U,
        "eZT BuildData truncated GBK context");
    failures += ExpectErrorAt(
        profileList.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        6U,
        "eZT ToPacket truncated GBK context");
    return failures;
}

int TestCanonicalToPacket()
{
    const ProfileList profileList(
        1,
        {
            ProfileListEntry(10, 100, "Hero"),
            ProfileListEntry(20, 200, ""),
        });
    const std::string payload = "1|10|100|Hero|20|200|";
    const cg::packet::PacketResult<cg::packet::Packet> packet = profileList.ToPacket();

    int failures = Expect(static_cast<bool>(packet), "eZT canonical ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    failures += ExpectProfileListPacket(
        packet.Value(),
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "eZT canonical ToPacket field");
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build eZT canonical packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainEzt(payload), "build eZT canonical wire");
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
