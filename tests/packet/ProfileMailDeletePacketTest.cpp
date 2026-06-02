#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <iostream>
#include <string>
#include <string_view>

namespace {

const cg::packet::PacketSpec& PacketSpec() noexcept
{
    return cg::packet::receive::ProfileMailDelete::Spec;
}

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, PacketSpec());
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, PacketSpec());
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

std::string BuildPlainCnb(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return "cNB ";
    }

    std::string plain = "cNB ";
    plain.append(encoded.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectProfileMailDeletePacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::ProfileMailDelete::Code), message);
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

int ExpectProfileMailDeleteValue(
    const cg::packet::receive::ProfileMailDelete& packet,
    int profileIndex,
    std::string_view name,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.profileIndex == profileIndex, message);
    failures += Expect(packet.name == name, message);
    return failures;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlainCnb("12|Alice"));

    int failures = Expect(static_cast<bool>(parsed), "parse cNB outer");
    if (parsed.HasValue()) {
        failures += ExpectProfileMailDeletePacket(parsed.Value(), "12|Alice", "parse cNB outer packet");
    }
    return failures;
}

int TestTypedParse()
{
    const cg::packet::PacketResult<cg::packet::receive::ProfileMailDelete> parsed =
        cg::packet::receive::ProfileMailDelete::Parse(BuildPlainCnb("12|Alice"));

    int failures = Expect(static_cast<bool>(parsed), "typed parse cNB");
    if (parsed.HasValue()) {
        failures += ExpectProfileMailDeleteValue(parsed.Value(), 12, "Alice", "typed cNB value");
    }
    return failures;
}

int TestEscapedNameRoundtrip()
{
    const std::string payload = "12|Alice Smith";
    const cg::packet::PacketResult<cg::packet::receive::ProfileMailDelete> parsed =
        cg::packet::receive::ProfileMailDelete::Parse(BuildPlainCnb(payload));

    int failures = Expect(static_cast<bool>(parsed), "typed parse cNB escaped name");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectProfileMailDeleteValue(parsed.Value(), 12, "Alice Smith", "typed cNB escaped name value");

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build cNB escaped name");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainCnb(payload), "roundtrip cNB escaped name");
    }
    return failures;
}

int TestBuildRoundtrip()
{
    const cg::packet::receive::ProfileMailDelete packetValue(12, "Alice");
    const cg::packet::Packet packet = packetValue.ToPacket();

    int failures = ExpectProfileMailDeletePacket(packet, "12|Alice", "cNB ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build cNB packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == BuildPlainCnb("12|Alice"), "build cNB packet value");

    const cg::packet::PacketResult<cg::packet::receive::ProfileMailDelete> parsed =
        cg::packet::receive::ProfileMailDelete::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip cNB packet");
    if (parsed.HasValue()) {
        failures += ExpectProfileMailDeleteValue(parsed.Value(), 12, "Alice", "roundtrip cNB value");
    }
    return failures;
}

int TestEmptyName()
{
    const cg::packet::PacketResult<cg::packet::receive::ProfileMailDelete> parsed =
        cg::packet::receive::ProfileMailDelete::Parse(BuildPlainCnb("12|"));

    int failures = Expect(static_cast<bool>(parsed), "typed parse cNB empty name");
    if (parsed.HasValue()) {
        failures += ExpectProfileMailDeleteValue(parsed.Value(), 12, "", "typed cNB empty name value");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::ProfileMailDelete::Parse("cNB"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "cNB",
        "typed cNB missing data");
    failures += ExpectError(
        cg::packet::receive::ProfileMailDelete::Parse(BuildPlainCnb("x|Alice")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "cNB",
        "typed cNB bad profileIndex decimal");
    failures += ExpectError(
        cg::packet::receive::ProfileMailDelete::Parse(BuildPlainCnb("12")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "cNB",
        "typed cNB one segment");
    failures += ExpectError(
        cg::packet::receive::ProfileMailDelete::Parse(BuildPlainCnb("12|Alice|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "cNB",
        "typed cNB trailing empty segment");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestTypedParse();
    failures += TestEscapedNameRoundtrip();
    failures += TestBuildRoundtrip();
    failures += TestEmptyName();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
