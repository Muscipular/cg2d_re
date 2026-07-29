#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <optional>
#include <string>
#include <string_view>
#include <vector>

namespace {

using CharacterProfile = cg::packet::receive::CharacterProfile;

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, CharacterProfile::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, CharacterProfile::Spec);
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

int FlagsFor(std::initializer_list<std::size_t> bits)
{
    std::uint32_t flags = 0;
    for (const std::size_t bit : bits) {
        flags |= 1U << bit;
    }
    return static_cast<int>(flags);
}

std::string BuildPayload(int flags, const std::vector<std::string>& values)
{
    const cg::packet::PacketResult<std::string> encodedFlags = cg::packet::EncodeInt62(flags);
    if (!encodedFlags.HasValue()) {
        return {};
    }

    std::string payload = encodedFlags.Value();
    payload.push_back('|');
    for (const std::string& value : values) {
        payload.append(value);
        payload.push_back('|');
    }
    return payload;
}

std::string BuildPlainApc(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return "aPc ";
    }

    std::string plain = "aPc ";
    plain.append(encoded.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectCharacterProfilePacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    cg::packet::PacketFieldType fieldType,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(CharacterProfile::Code), message);
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

int TestParsePacketOuter()
{
    const int flags = FlagsFor({1U});
    const std::string payload = BuildPayload(flags, {"100"});
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlainApc(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse aPc outer");
    if (parsed.HasValue()) {
        failures += ExpectCharacterProfilePacket(
            parsed.Value(),
            payload,
            cg::packet::PacketFieldType::EscapedString,
            "parse aPc outer packet");
    }
    return failures;
}

int TestPartialMask()
{
    const int flags = FlagsFor({1U, 27U, 29U});
    const std::string payload = BuildPayload(flags, {"100", "500", "Hero\\zName", "25"});
    const cg::packet::PacketResult<CharacterProfile> parsed =
        CharacterProfile::Parse(BuildPlainApc(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse aPc partial mask");
    if (!parsed.HasValue()) {
        return failures;
    }

    const CharacterProfile& profile = parsed.Value();
    failures += Expect(profile.flags == flags, "aPc partial flags");
    failures += Expect(profile.hp == 100, "aPc partial hp");
    failures += Expect(!profile.maxHp.has_value(), "aPc partial maxHp absent");
    failures += Expect(profile.gold == 500, "aPc partial gold");
    failures += Expect(profile.name == "Hero|Name", "aPc partial name");
    failures += Expect(!profile.ownTitle.has_value(), "aPc partial ownTitle absent");
    failures += Expect(profile.crystal == 25, "aPc partial crystal");
    return failures;
}

int TestAllFields()
{
    std::vector<std::string> values;
    values.reserve(31U);
    for (int value = 1; value <= 28; ++value) {
        values.push_back(std::to_string(value));
    }
    values.push_back("Hero");
    values.push_back("OwnTitle");
    values.push_back("31");

    const std::string payload = BuildPayload(1, values);
    const cg::packet::PacketResult<CharacterProfile> parsed =
        CharacterProfile::Parse(BuildPlainApc(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse aPc ALL fields");
    if (!parsed.HasValue()) {
        return failures;
    }

    using NumericMember = std::optional<int> CharacterProfile::*;
    constexpr std::array<NumericMember, 28U> members = {{
        &CharacterProfile::hp,
        &CharacterProfile::maxHp,
        &CharacterProfile::forcePoint,
        &CharacterProfile::maxForcePoint,
        &CharacterProfile::vital,
        &CharacterProfile::strength,
        &CharacterProfile::toughness,
        &CharacterProfile::quick,
        &CharacterProfile::magic,
        &CharacterProfile::stamina,
        &CharacterProfile::dexterity,
        &CharacterProfile::intelligence,
        &CharacterProfile::experience,
        &CharacterProfile::nextExperience,
        &CharacterProfile::level,
        &CharacterProfile::attack,
        &CharacterProfile::defence,
        &CharacterProfile::agility,
        &CharacterProfile::magicPower,
        &CharacterProfile::recovery,
        &CharacterProfile::charm,
        &CharacterProfile::fame,
        &CharacterProfile::earth,
        &CharacterProfile::water,
        &CharacterProfile::fire,
        &CharacterProfile::wind,
        &CharacterProfile::gold,
        &CharacterProfile::titleId,
    }};

    const CharacterProfile& profile = parsed.Value();
    failures += Expect(profile.flags == 1, "aPc ALL flags");
    for (std::size_t index = 0; index < members.size(); ++index) {
        const std::optional<int>& value = profile.*members[index];
        failures += Expect(
            value.has_value() && *value == static_cast<int>(index + 1U),
            "aPc ALL numeric field");
    }
    failures += Expect(profile.name == "Hero", "aPc ALL name");
    failures += Expect(profile.ownTitle == "OwnTitle", "aPc ALL ownTitle");
    failures += Expect(profile.crystal == 31, "aPc ALL crystal");

    const cg::packet::PacketResult<std::string> rebuilt = profile.BuildData();
    failures += Expect(static_cast<bool>(rebuilt), "build aPc ALL fields");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == payload, "roundtrip aPc ALL fields");
    }
    return failures;
}

int TestLegacyInnerEscape()
{
    const int flags = FlagsFor({29U, 30U});
    const std::string payload = BuildPayload(
        flags,
        {"Line\\nComma\\cPipe\\zSlash\\yUnknown\\q", "Title\\zValue"});
    const cg::packet::PacketResult<CharacterProfile> parsed =
        CharacterProfile::Parse(BuildPlainApc(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse aPc inner escape");
    if (!parsed.HasValue()) {
        return failures;
    }

    const CharacterProfile& profile = parsed.Value();
    failures += Expect(
        profile.name == "Line\nComma,Pipe|Slash\\Unknownq",
        "aPc inner name decoded");
    failures += Expect(profile.ownTitle == "Title|Value", "aPc inner ownTitle decoded");

    const std::string canonical = BuildPayload(
        flags,
        {"Line\\nComma\\cPipe\\zSlash\\yUnknownq", "Title\\zValue"});
    const cg::packet::PacketResult<std::string> rebuilt = profile.BuildData();
    failures += Expect(static_cast<bool>(rebuilt), "build aPc inner escape");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == canonical, "aPc inner escape canonical");
    }
    return failures;
}

int TestGbkDelimiterTrailByte()
{
    const int flags = FlagsFor({29U});
    const std::string gbkCharacter {static_cast<char>(0x81), static_cast<char>(0x7c)};
    const std::string payload = BuildPayload(flags, {gbkCharacter});
    const cg::packet::PacketResult<CharacterProfile> parsed =
        CharacterProfile::Parse(BuildPlainApc(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse aPc GBK 81 7C");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().name == gbkCharacter, "aPc GBK name bytes");
    const cg::packet::PacketResult<std::string> rebuilt = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(rebuilt), "build aPc GBK 81 7C");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == payload, "roundtrip aPc GBK 81 7C");
    }
    return failures;
}

int TestBit31ConsumesNoValue()
{
    const int flags = std::numeric_limits<int>::min();
    const std::string payload = BuildPayload(flags, {});
    const cg::packet::PacketResult<CharacterProfile> parsed =
        CharacterProfile::Parse(BuildPlainApc(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse aPc bit31 no value");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().flags == flags, "aPc bit31 flags");
    failures += Expect(!parsed.Value().hp.has_value(), "aPc bit31 hp absent");
    failures += Expect(!parsed.Value().ownTitle.has_value(), "aPc bit31 ownTitle absent");
    failures += Expect(!parsed.Value().crystal.has_value(), "aPc bit31 crystal absent");
    return failures;
}

int TestToPacketRoundTrip()
{
    CharacterProfile profile;
    profile.flags = FlagsFor({1U, 29U});
    profile.hp = -10;
    profile.name = "Hero|Name";

    const cg::packet::PacketResult<cg::packet::Packet> packet = profile.ToPacket();
    int failures = Expect(static_cast<bool>(packet), "aPc ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const std::string payload = BuildPayload(profile.flags, {"-10", "Hero\\zName"});
    failures += ExpectCharacterProfilePacket(
        packet.Value(),
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "aPc ToPacket data");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build aPc packet");
    if (!built.HasValue()) {
        return failures;
    }
    failures += Expect(built.Value() == BuildPlainApc(payload), "build aPc wire");

    const cg::packet::PacketResult<CharacterProfile> parsed = CharacterProfile::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip aPc packet");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().hp == -10, "roundtrip aPc hp");
        failures += Expect(parsed.Value().name == "Hero|Name", "roundtrip aPc name");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("aPc"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "aPc",
        "aPc missing data");
    failures += ExpectError(
        CharacterProfile::Parse("aPc"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "aPc",
        "typed aPc missing data");
    failures += ExpectError(
        ParsePacket("aPc one two "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "aPc",
        "aPc extra outer token");
    failures += ExpectError(
        CharacterProfile::Parse(BuildPlainApc(BuildPayload(FlagsFor({1U}), {}))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "aPc",
        "aPc selected value missing");
    failures += ExpectError(
        CharacterProfile::Parse(BuildPlainApc(BuildPayload(0, {"100"}))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "aPc",
        "aPc unselected extra value");
    failures += ExpectError(
        CharacterProfile::Parse(BuildPlainApc(BuildPayload(FlagsFor({1U}), {"bad"}))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "aPc",
        "aPc bad decimal");
    failures += ExpectError(
        CharacterProfile::Parse(BuildPlainApc(BuildPayload(FlagsFor({29U}), {"Name\\"}))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "aPc",
        "aPc trailing inner escape");
    failures += ExpectError(
        CharacterProfile::Parse(BuildPlainApc(BuildPayload(
            std::numeric_limits<int>::min(),
            {"unused"}))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "aPc",
        "aPc bit31 consumes no value");

    const std::string truncatedGbk(1U, static_cast<char>(0x81));
    failures += Expect(
        !cg::packet::DecodeLegacyPayloadText(truncatedGbk).HasValue(),
        "aPc decode truncated GBK");
    failures += Expect(
        !cg::packet::EncodeLegacyPayloadText(truncatedGbk).HasValue(),
        "aPc encode truncated GBK");

    CharacterProfile missingSelected;
    missingSelected.flags = FlagsFor({1U});
    failures += ExpectError(
        missingSelected.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "aPc",
        "aPc build missing selected field");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestPartialMask();
    failures += TestAllFields();
    failures += TestLegacyInnerEscape();
    failures += TestGbkDelimiterTrailByte();
    failures += TestBit31ConsumesNoValue();
    failures += TestToPacketRoundTrip();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
