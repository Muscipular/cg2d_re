#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
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
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::CharacterProfile::Code), message);
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

int ExpectCharacterProfileValues(
    const cg::packet::receive::CharacterProfile& profile,
    int flags,
    const std::vector<std::string>& values,
    const char* message)
{
    int failures = 0;
    failures += Expect(profile.flags == flags, message);
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
    const std::string payload = "1|100|200|";
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(BuildPlainApc(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse aPc outer");
    if (parsed.HasValue()) {
        failures += ExpectCharacterProfilePacket(parsed.Value(), payload, "parse aPc outer packet");
    }
    return failures;
}

int TestTypedParseRawValues()
{
    const cg::packet::PacketResult<cg::packet::receive::CharacterProfile> parsed =
        cg::packet::receive::CharacterProfile::Parse(BuildPlainApc("1|100|200|"));

    int failures = Expect(static_cast<bool>(parsed), "parse aPc raw values");
    if (parsed.HasValue()) {
        failures += ExpectCharacterProfileValues(parsed.Value(), 1, {"100", "200"}, "parse aPc values");
        failures += Expect(parsed.Value().BuildData() == "1|100|200|", "parse aPc drops only trailing empty segment");
    }
    return failures;
}

int TestSingleFieldFlagsSample()
{
    const cg::packet::PacketResult<cg::packet::receive::CharacterProfile> parsed =
        cg::packet::receive::CharacterProfile::Parse(BuildPlainApc("a|123|"));

    int failures = Expect(static_cast<bool>(parsed), "parse aPc flags 10 sample");
    if (parsed.HasValue()) {
        failures += ExpectCharacterProfileValues(parsed.Value(), 10, {"123"}, "parse aPc flags 10 value");
    }
    return failures;
}

int TestStringFieldsAreRawValues()
{
    const cg::packet::PacketResult<cg::packet::receive::CharacterProfile> parsed =
        cg::packet::receive::CharacterProfile::Parse(BuildPlainApc("1|Name|OwnTitle|"));

    int failures = Expect(static_cast<bool>(parsed), "parse aPc string values");
    if (parsed.HasValue()) {
        failures += ExpectCharacterProfileValues(parsed.Value(), 1, {"Name", "OwnTitle"}, "parse aPc string raw values");
    }
    return failures;
}

int TestMiddleEmptyValueIsPreserved()
{
    const cg::packet::PacketResult<cg::packet::receive::CharacterProfile> parsed =
        cg::packet::receive::CharacterProfile::Parse(BuildPlainApc("1||200|"));

    int failures = Expect(static_cast<bool>(parsed), "parse aPc middle empty value");
    if (parsed.HasValue()) {
        failures += ExpectCharacterProfileValues(parsed.Value(), 1, {"", "200"}, "parse aPc keeps middle empty value");
        failures += Expect(parsed.Value().BuildData() == "1||200|", "build aPc keeps middle empty value");
    }
    return failures;
}

int TestToPacketBuildPreservesTrailingDelimiter()
{
    const cg::packet::receive::CharacterProfile profile(10, {"123"});
    const cg::packet::Packet packet = profile.ToPacket();

    int failures = ExpectCharacterProfilePacket(packet, "a|123|", "aPc ToPacket data");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build aPc packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == BuildPlainApc("a|123|"), "build aPc packet preserves trailing delimiter");

    const cg::packet::PacketResult<cg::packet::receive::CharacterProfile> parsed =
        cg::packet::receive::CharacterProfile::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip aPc packet");
    if (parsed.HasValue()) {
        failures += ExpectCharacterProfileValues(parsed.Value(), 10, {"123"}, "roundtrip aPc values");
    }
    return failures;
}

int TestEmptyValuesBuild()
{
    const cg::packet::receive::CharacterProfile profile(1, {});

    int failures = Expect(profile.BuildData() == "1|", "build aPc empty values");
    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(profile.ToPacket());
    failures += Expect(static_cast<bool>(built), "build aPc empty values packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainApc("1|"), "build aPc empty values packet value");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("aPc"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "aPc",
        "aPc missing data");
    failures += ExpectError(
        cg::packet::receive::CharacterProfile::Parse("aPc"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "aPc",
        "typed aPc missing data");
    failures += ExpectError(
        cg::packet::ParsePacket("aPc one two "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "aPc",
        "aPc extra outer token");
    failures += ExpectError(
        cg::packet::receive::CharacterProfile::Parse(BuildPlainApc("1|100")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "aPc",
        "typed aPc missing trailing delimiter");
    failures += ExpectError(
        cg::packet::receive::CharacterProfile::Parse(BuildPlainApc("_|100|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "aPc",
        "typed aPc bad flags");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestTypedParseRawValues();
    failures += TestSingleFieldFlagsSample();
    failures += TestStringFieldsAreRawValues();
    failures += TestMiddleEmptyValueIsPreserved();
    failures += TestToPacketBuildPreservesTrailingDelimiter();
    failures += TestEmptyValuesBuild();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
