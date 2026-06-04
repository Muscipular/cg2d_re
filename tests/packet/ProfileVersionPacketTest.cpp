#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::ProfileVersion::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::ProfileVersion::Spec);
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

std::string BuildPlainHnp(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return "HNP ";
    }

    std::string plain = "HNP ";
    plain.append(encoded.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectProfileVersionPacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::ProfileVersion::Code), message);
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

int ExpectProfileVersionValues(
    const cg::packet::receive::ProfileVersion& profileVersion,
    int profile,
    int about,
    int mail,
    int seek,
    const char* message)
{
    int failures = 0;
    failures += Expect(profileVersion.profileVersion == profile, message);
    failures += Expect(profileVersion.aboutVersion == about, message);
    failures += Expect(profileVersion.mailCharge == mail, message);
    failures += Expect(profileVersion.seekPos == seek, message);
    return failures;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlainHnp("1|2|100|-1"));

    int failures = Expect(static_cast<bool>(parsed), "parse HNP outer");
    if (parsed.HasValue()) {
        failures += ExpectProfileVersionPacket(parsed.Value(), "1|2|100|-1", "parse HNP outer packet");
    }
    return failures;
}

int TestTypedParseRawDecimal()
{
    const cg::packet::PacketResult<cg::packet::receive::ProfileVersion> parsed =
        cg::packet::receive::ProfileVersion::Parse(BuildPlainHnp("1|2|100|-1"));

    int failures = Expect(static_cast<bool>(parsed), "parse HNP raw decimal payload");
    if (parsed.HasValue()) {
        failures += ExpectProfileVersionValues(parsed.Value(), 1, 2, 100, -1, "parse HNP values");
    }
    return failures;
}

int TestBuildRoundtrip()
{
    const cg::packet::receive::ProfileVersion profileVersion(1, 2, 100, -1);
    const cg::packet::Packet packet = profileVersion.ToPacket();

    int failures = ExpectProfileVersionPacket(packet, "1|2|100|-1", "HNP ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build HNP packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == BuildPlainHnp("1|2|100|-1"), "build HNP packet value");

    const cg::packet::PacketResult<cg::packet::receive::ProfileVersion> parsed =
        cg::packet::receive::ProfileVersion::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip HNP packet");
    if (parsed.HasValue()) {
        failures += ExpectProfileVersionValues(parsed.Value(), 1, 2, 100, -1, "roundtrip HNP values");
    }
    return failures;
}

int TestPuk3SeekPos()
{
    const cg::packet::PacketResult<cg::packet::receive::ProfileVersion> parsed =
        cg::packet::receive::ProfileVersion::Parse(BuildPlainHnp("3|4|100|42"));

    int failures = Expect(static_cast<bool>(parsed), "parse HNP PUK3 seekPos sample");
    if (parsed.HasValue()) {
        failures += ExpectProfileVersionValues(parsed.Value(), 3, 4, 100, 42, "parse HNP PUK3 seekPos 42");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("HNP"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "HNP",
        "HNP missing data");
    failures += ExpectError(
        cg::packet::receive::ProfileVersion::Parse("HNP"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "HNP",
        "typed HNP missing data");
    failures += ExpectError(
        cg::packet::receive::ProfileVersion::Parse(BuildPlainHnp("1|2|100")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "HNP",
        "typed HNP three segments");
    failures += ExpectError(
        cg::packet::receive::ProfileVersion::Parse(BuildPlainHnp("1|2|100|-1|0")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "HNP",
        "typed HNP five segments");
    failures += ExpectError(
        cg::packet::receive::ProfileVersion::Parse(BuildPlainHnp("1|x|100|-1")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "HNP",
        "typed HNP bad raw decimal");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestTypedParseRawDecimal();
    failures += TestBuildRoundtrip();
    failures += TestPuk3SeekPos();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
