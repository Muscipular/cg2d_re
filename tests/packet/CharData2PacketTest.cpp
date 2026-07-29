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

using CharacterProfile2 = cg::packet::receive::CharacterProfile2;
using NumericMember = std::optional<int> CharacterProfile2::*;

constexpr std::array<NumericMember, 23U> NumericMembers = {{
    &CharacterProfile2::critical,
    &CharacterProfile2::counter,
    &CharacterProfile2::hitRate,
    &CharacterProfile2::avoid,
    &CharacterProfile2::poison,
    &CharacterProfile2::sleep,
    &CharacterProfile2::stone,
    &CharacterProfile2::drunk,
    &CharacterProfile2::confusion,
    &CharacterProfile2::amnesia,
    &CharacterProfile2::duelPoint,
    &CharacterProfile2::injury,
    &CharacterProfile2::tribe,
    &CharacterProfile2::penalty,
    &CharacterProfile2::rebirth,
    &CharacterProfile2::faceImage,
    &CharacterProfile2::skillSlot,
    &CharacterProfile2::walkSpeed,
    &CharacterProfile2::modAttack,
    &CharacterProfile2::modDefence,
    &CharacterProfile2::goldLimit,
    &CharacterProfile2::poolGoldLimit,
    &CharacterProfile2::poolGoldLimit2,
}};

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, CharacterProfile2::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, CharacterProfile2::Spec);
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

std::string BuildPayload(int flagMask, const std::vector<std::string>& values)
{
    const cg::packet::PacketResult<std::string> encodedFlagMask =
        cg::packet::EncodeInt62(flagMask);
    if (!encodedFlagMask.HasValue()) {
        return {};
    }

    std::string payload = encodedFlagMask.Value();
    payload.push_back('|');
    for (const std::string& value : values) {
        payload.append(value);
        payload.push_back('|');
    }
    return payload;
}

std::string BuildPlainUu(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return "UU ";
    }

    std::string plain = "UU ";
    plain.append(encoded.Value());
    plain.push_back(' ');
    return plain;
}

void PopulateAllNumericFields(CharacterProfile2& profile)
{
    for (std::size_t index = 0; index < NumericMembers.size(); ++index) {
        profile.*NumericMembers[index] = static_cast<int>(index + 1U);
    }
}

int ExpectCharacterProfile2Packet(
    const cg::packet::Packet& packet,
    std::string_view value,
    cg::packet::PacketFieldType fieldType,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(CharacterProfile2::Code), message);
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
    const std::string payload = BuildPayload(FlagsFor({1U}), {"123"});
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlainUu(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse UU outer");
    if (parsed.HasValue()) {
        failures += ExpectCharacterProfile2Packet(
            parsed.Value(),
            payload,
            cg::packet::PacketFieldType::EscapedString,
            "parse UU outer packet");
    }
    return failures;
}

int TestPartialMask()
{
    const int flagMask = FlagsFor({1U, 3U, 18U, 23U});
    const std::string payload = BuildPayload(
        flagMask,
        {"-10", "2147483647", "-2147483648", "42"});
    const cg::packet::PacketResult<CharacterProfile2> parsed =
        CharacterProfile2::Parse(BuildPlainUu(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse UU partial mask");
    if (!parsed.HasValue()) {
        return failures;
    }

    const CharacterProfile2& profile = parsed.Value();
    failures += Expect(profile.flagMask == flagMask, "UU partial flagMask");
    failures += Expect(profile.critical == -10, "UU partial critical");
    failures += Expect(!profile.counter.has_value(), "UU partial counter absent");
    failures += Expect(profile.hitRate == std::numeric_limits<int>::max(), "UU partial hitRate");
    failures += Expect(profile.walkSpeed == std::numeric_limits<int>::min(), "UU partial walkSpeed");
    failures += Expect(profile.poolGoldLimit2 == 42, "UU partial poolGoldLimit2");
    failures += Expect(!profile.bankPage.has_value(), "UU partial bankPage absent");

    const cg::packet::PacketResult<std::string> rebuilt = profile.BuildData();
    failures += Expect(static_cast<bool>(rebuilt), "build UU partial mask");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == payload, "roundtrip UU partial mask");
    }
    return failures;
}

int TestAllFields()
{
    std::vector<std::string> values;
    values.reserve(24U);
    for (int value = 1; value <= 24; ++value) {
        values.push_back(std::to_string(value));
    }

    const std::string payload = BuildPayload(1, values);
    const cg::packet::PacketResult<CharacterProfile2> parsed =
        CharacterProfile2::Parse(BuildPlainUu(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse UU ALL fields");
    if (!parsed.HasValue()) {
        return failures;
    }

    const CharacterProfile2& profile = parsed.Value();
    failures += Expect(profile.flagMask == 1, "UU ALL flagMask");
    for (std::size_t index = 0; index < NumericMembers.size(); ++index) {
        const std::optional<int>& value = profile.*NumericMembers[index];
        failures += Expect(
            value.has_value() && *value == static_cast<int>(index + 1U),
            "UU ALL numeric field");
    }
    failures += Expect(profile.bankPage == 24, "UU ALL bankPage");

    const cg::packet::PacketResult<std::string> rebuilt = profile.BuildData();
    failures += Expect(static_cast<bool>(rebuilt), "build UU ALL fields");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == payload, "roundtrip UU ALL fields");
    }
    return failures;
}

int TestZeroMask()
{
    const cg::packet::PacketResult<CharacterProfile2> parsed =
        CharacterProfile2::Parse(BuildPlainUu("0|"));

    int failures = Expect(static_cast<bool>(parsed), "parse UU zero mask");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().flagMask == 0, "UU zero flagMask");
    failures += Expect(!parsed.Value().critical.has_value(), "UU zero critical absent");
    failures += Expect(!parsed.Value().bankPage.has_value(), "UU zero bankPage absent");
    const cg::packet::PacketResult<std::string> rebuilt = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(rebuilt), "build UU zero mask");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "0|", "roundtrip UU zero mask");
    }
    return failures;
}

int TestToPacketRoundTrip()
{
    CharacterProfile2 profile;
    profile.flagMask = FlagsFor({2U, 20U});
    profile.counter = -11;
    profile.modDefence = 22;

    const cg::packet::PacketResult<cg::packet::Packet> packet = profile.ToPacket();
    int failures = Expect(static_cast<bool>(packet), "UU ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const std::string payload = BuildPayload(profile.flagMask, {"-11", "22"});
    failures += ExpectCharacterProfile2Packet(
        packet.Value(),
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "UU ToPacket data");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build UU packet");
    if (!built.HasValue()) {
        return failures;
    }
    failures += Expect(built.Value() == BuildPlainUu(payload), "build UU wire");

    const cg::packet::PacketResult<CharacterProfile2> parsed =
        CharacterProfile2::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip UU packet");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().counter == -11, "roundtrip UU counter");
        failures += Expect(parsed.Value().modDefence == 22, "roundtrip UU modDefence");
        failures += Expect(!parsed.Value().bankPage.has_value(), "roundtrip UU bankPage absent");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("UU"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "UU",
        "UU missing data");
    failures += ExpectError(
        CharacterProfile2::Parse("UU"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "UU",
        "typed UU missing data");
    failures += ExpectError(
        ParsePacket("UU one two "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "UU",
        "UU extra outer token");
    failures += ExpectError(
        CharacterProfile2::Parse(BuildPlainUu("")),
        cg::packet::PacketErrorCode::EmptyInput,
        "UU",
        "typed UU empty payload");
    failures += ExpectError(
        CharacterProfile2::Parse(BuildPlainUu("2|123")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "UU",
        "typed UU missing trailing delimiter");
    failures += ExpectError(
        CharacterProfile2::Parse(BuildPlainUu("_|123|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "UU",
        "typed UU bad flagMask");
    failures += ExpectError(
        CharacterProfile2::Parse(BuildPlainUu("2|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "UU",
        "typed UU selected value missing");
    failures += ExpectError(
        CharacterProfile2::Parse(BuildPlainUu("0|1|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "UU",
        "typed UU unselected extra value");
    failures += ExpectError(
        CharacterProfile2::Parse(BuildPlainUu("2||")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "UU",
        "typed UU empty value");
    failures += ExpectError(
        CharacterProfile2::Parse(BuildPlainUu("2|+1|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "UU",
        "typed UU invalid decimal");
    failures += ExpectError(
        CharacterProfile2::Parse(BuildPlainUu("2|2147483648|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "UU",
        "typed UU positive int overflow");
    failures += ExpectError(
        CharacterProfile2::Parse(BuildPlainUu("2|-2147483649|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "UU",
        "typed UU negative int overflow");
    failures += ExpectError(
        CharacterProfile2::Parse(BuildPlainUu(BuildPayload(FlagsFor({0U, 1U}), {"1"}))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "UU",
        "typed UU bit0 combination");
    failures += ExpectError(
        CharacterProfile2::Parse(BuildPlainUu(BuildPayload(FlagsFor({24U}), {"1"}))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "UU",
        "typed UU unsupported bit24");
    failures += ExpectError(
        CharacterProfile2::Parse(BuildPlainUu(BuildPayload(
            std::numeric_limits<int>::min(),
            {}))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "UU",
        "typed UU explicit bit31");

    std::vector<std::string> missingBankPage(23U, "1");
    failures += ExpectError(
        CharacterProfile2::Parse(BuildPlainUu(BuildPayload(1, missingBankPage))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "UU",
        "typed UU ALL missing bankPage");
    missingBankPage.push_back("1");
    missingBankPage.push_back("1");
    failures += ExpectError(
        CharacterProfile2::Parse(BuildPlainUu(BuildPayload(1, missingBankPage))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "UU",
        "typed UU ALL extra value");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    CharacterProfile2 missingSelected;
    missingSelected.flagMask = FlagsFor({1U});
    failures += ExpectError(
        missingSelected.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "UU",
        "UU build missing selected field");

    CharacterProfile2 unexpectedField;
    unexpectedField.critical = 1;
    failures += ExpectError(
        unexpectedField.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "UU",
        "UU build unexpected field");

    CharacterProfile2 unexpectedBankPage;
    unexpectedBankPage.bankPage = 1;
    failures += ExpectError(
        unexpectedBankPage.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "UU",
        "UU build unexpected bankPage");

    CharacterProfile2 missingBankPage;
    missingBankPage.flagMask = 1;
    PopulateAllNumericFields(missingBankPage);
    failures += ExpectError(
        missingBankPage.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "UU",
        "UU build ALL missing bankPage");

    CharacterProfile2 unsupportedFlag;
    unsupportedFlag.flagMask = FlagsFor({24U});
    failures += ExpectError(
        unsupportedFlag.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "UU",
        "UU build unsupported flag");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestPartialMask();
    failures += TestAllFields();
    failures += TestZeroMask();
    failures += TestToPacketRoundTrip();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
