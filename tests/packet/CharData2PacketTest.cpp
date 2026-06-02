#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::CharacterProfile2::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::CharacterProfile2::Spec);
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

int ExpectCharacterProfile2Packet(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::CharacterProfile2::Code), message);
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

int ExpectCharacterProfile2Values(
    const cg::packet::receive::CharacterProfile2& profile,
    int flagMask,
    const std::vector<std::string>& values,
    const char* message)
{
    int failures = 0;
    failures += Expect(profile.flagMask == flagMask, message);
    failures += Expect(profile.values.size() == values.size(), message);
    if (profile.values.size() != values.size()) {
        return failures;
    }

    for (std::size_t index = 0; index < values.size(); ++index) {
        failures += Expect(profile.values[index] == values[index], message);
    }
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = "2|123|";
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlainUu(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse UU outer");
    if (parsed.HasValue()) {
        failures += ExpectCharacterProfile2Packet(parsed.Value(), payload, "parse UU outer packet");
    }
    return failures;
}

int TestTypedParseSingleValue()
{
    const cg::packet::PacketResult<cg::packet::receive::CharacterProfile2> parsed =
        cg::packet::receive::CharacterProfile2::Parse(BuildPlainUu("2|123|"));

    int failures = Expect(static_cast<bool>(parsed), "parse UU single raw value");
    if (parsed.HasValue()) {
        failures += ExpectCharacterProfile2Values(parsed.Value(), 2, {"123"}, "parse UU single value");
        failures += Expect(parsed.Value().BuildData() == "2|123|", "parse UU preserves trailing delimiter");
    }
    return failures;
}

int TestTypedParseTwoValues()
{
    const cg::packet::PacketResult<cg::packet::receive::CharacterProfile2> parsed =
        cg::packet::receive::CharacterProfile2::Parse(BuildPlainUu("6|11|22|"));

    int failures = Expect(static_cast<bool>(parsed), "parse UU two raw values");
    if (parsed.HasValue()) {
        failures += ExpectCharacterProfile2Values(parsed.Value(), 6, {"11", "22"}, "parse UU two values");
    }
    return failures;
}

int TestFlagMaskOneWithMultipleRawValues()
{
    const cg::packet::PacketResult<cg::packet::receive::CharacterProfile2> parsed =
        cg::packet::receive::CharacterProfile2::Parse(BuildPlainUu("1|100|Name|255|"));

    int failures = Expect(static_cast<bool>(parsed), "parse UU flagMask 1 multi values");
    if (parsed.HasValue()) {
        failures += ExpectCharacterProfile2Values(
            parsed.Value(),
            1,
            {"100", "Name", "255"},
            "parse UU flagMask 1 keeps all raw values");
    }
    return failures;
}

int TestToPacketBuildPreservesTrailingDelimiter()
{
    const cg::packet::receive::CharacterProfile2 profile(6, {"11", "22"});
    const cg::packet::Packet packet = profile.ToPacket();

    int failures = ExpectCharacterProfile2Packet(packet, "6|11|22|", "UU ToPacket data");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build UU packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == BuildPlainUu("6|11|22|"), "build UU packet preserves trailing delimiter");

    const cg::packet::PacketResult<cg::packet::receive::CharacterProfile2> parsed =
        cg::packet::receive::CharacterProfile2::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip UU packet");
    if (parsed.HasValue()) {
        failures += ExpectCharacterProfile2Values(parsed.Value(), 6, {"11", "22"}, "roundtrip UU values");
    }
    return failures;
}

int TestEmptyValuesBuild()
{
    const cg::packet::receive::CharacterProfile2 profile(2, {});

    int failures = Expect(profile.BuildData() == "2|", "build UU empty values");
    const cg::packet::PacketResult<std::string> built = BuildPacket(profile.ToPacket());
    failures += Expect(static_cast<bool>(built), "build UU empty values packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainUu("2|"), "build UU empty values packet value");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("UU"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "UU",
        "UU missing data");
    failures += ExpectError(
        cg::packet::receive::CharacterProfile2::Parse("UU"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "UU",
        "typed UU missing data");
    failures += ExpectError(
        ParsePacket("UU one two "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "UU",
        "UU extra outer token");
    failures += ExpectError(
        cg::packet::receive::CharacterProfile2::Parse(BuildPlainUu("")),
        cg::packet::PacketErrorCode::EmptyInput,
        "UU",
        "typed UU empty payload");
    failures += ExpectError(
        cg::packet::receive::CharacterProfile2::Parse(BuildPlainUu("2|123")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "UU",
        "typed UU missing trailing delimiter");
    failures += ExpectError(
        cg::packet::receive::CharacterProfile2::Parse(BuildPlainUu("_|123|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "UU",
        "typed UU bad flagMask");
    failures += ExpectError(
        cg::packet::receive::CharacterProfile2::Parse(BuildPlainUu("6|11||")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "UU",
        "typed UU middle empty value");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestTypedParseSingleValue();
    failures += TestTypedParseTwoValues();
    failures += TestFlagMaskOneWithMultipleRawValues();
    failures += TestToPacketBuildPreservesTrailingDelimiter();
    failures += TestEmptyValuesBuild();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
