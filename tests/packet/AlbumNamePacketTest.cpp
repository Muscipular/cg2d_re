#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

const cg::packet::PacketSpec& PacketSpec() noexcept
{
    return cg::packet::receive::AlbumName::Spec;
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

std::string BuildPlain(std::string_view albumId, std::string_view name)
{
    const cg::packet::PacketResult<std::string> encodedName = cg::packet::EncodeEscapedString(name);
    std::string plain(cg::packet::receive::AlbumName::Code);
    plain.push_back(' ');
    plain.append(albumId);
    plain.push_back(' ');
    if (encodedName.HasValue()) {
        plain.append(encodedName.Value());
    }
    plain.push_back(' ');
    return plain;
}

int ExpectField(
    const cg::packet::Packet& packet,
    std::size_t index,
    std::string_view name,
    cg::packet::PacketFieldType type,
    std::string_view value,
    const char* message)
{
    int failures = Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }

    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(packet.fields[index].type == type, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int ExpectAlbumNamePacket(
    const cg::packet::Packet& packet,
    std::string_view albumId,
    std::string_view name,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::AlbumName::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "albumId", cg::packet::PacketFieldType::Int62, albumId, message);
    failures += ExpectField(packet, 1U, "name", cg::packet::PacketFieldType::EscapedString, name, message);
    return failures;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlain("1", "Slime"));

    int failures = Expect(static_cast<bool>(parsed), "parse Dn outer");
    if (parsed.HasValue()) {
        failures += ExpectAlbumNamePacket(parsed.Value(), "1", "Slime", "parse Dn packet");
    }
    return failures;
}

int TestTypedRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::receive::AlbumName> parsed =
        cg::packet::receive::AlbumName::Parse(BuildPlain("1", "Slime"));

    int failures = Expect(static_cast<bool>(parsed), "parse Dn typed");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().albumId == 1, "Dn albumId");
    failures += Expect(parsed.Value().name == "Slime", "Dn name");
    failures += ExpectAlbumNamePacket(parsed.Value().ToPacket(), "1", "Slime", "Dn ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build Dn packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlain("1", "Slime"), "roundtrip Dn packet");
    }
    return failures;
}

int TestAlbumIdI62()
{
    const cg::packet::PacketResult<cg::packet::receive::AlbumName> parsed =
        cg::packet::receive::AlbumName::Parse(BuildPlain("a", "Slime"));

    int failures = Expect(static_cast<bool>(parsed), "parse Dn i62 albumId");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().albumId == 10, "Dn albumId a is 10");
        failures += Expect(parsed.Value().name == "Slime", "Dn i62 name");
    }
    return failures;
}

int TestEscapedNameRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::receive::AlbumName> parsed =
        cg::packet::receive::AlbumName::Parse("Dn 1 Blue\\SSlime ");

    int failures = Expect(static_cast<bool>(parsed), "parse Dn escaped name");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().name == "Blue Slime", "Dn escaped name value");

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build Dn escaped name");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "Dn 1 Blue\\SSlime ", "Dn escaped name roundtrip");
    }
    return failures;
}

int TestNameKeepsPipe()
{
    const cg::packet::PacketResult<cg::packet::receive::AlbumName> parsed =
        cg::packet::receive::AlbumName::Parse(BuildPlain("1", "Blue|Slime"));

    int failures = Expect(static_cast<bool>(parsed), "parse Dn pipe name");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().name == "Blue|Slime", "Dn pipe name preserved");
    }
    return failures;
}

int TestEmptyName()
{
    const cg::packet::receive::AlbumName albumName(1, "");
    const cg::packet::PacketResult<std::string> built = BuildPacket(albumName.ToPacket());

    int failures = Expect(static_cast<bool>(built), "build Dn empty name");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == "Dn 1  ", "Dn empty name plain");

    const cg::packet::PacketResult<cg::packet::receive::AlbumName> parsed =
        cg::packet::receive::AlbumName::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse Dn empty name");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().albumId == 1, "Dn empty name albumId");
        failures += Expect(parsed.Value().name.empty(), "Dn empty name value");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::AlbumName::Parse("ycX 1 Slime "),
        cg::packet::PacketErrorCode::InvalidCode,
        "ycX",
        "Dn wrong code");
    failures += ExpectError(
        cg::packet::receive::AlbumName::Parse("Dn 1"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Dn",
        "Dn missing field");
    failures += ExpectError(
        cg::packet::receive::AlbumName::Parse("Dn 1 Slime extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Dn",
        "Dn extra field");
    failures += ExpectError(
        cg::packet::receive::AlbumName::Parse("Dn _ name "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Dn",
        "Dn bad albumId i62");
    failures += ExpectError(
        cg::packet::receive::AlbumName::Parse("Dn zzzzzz name "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Dn",
        "Dn albumId i62 overflow");

    std::string truncatedName = "Dn 1 ";
    truncatedName.push_back(static_cast<char>(0x80U));
    truncatedName.push_back(' ');
    failures += ExpectError(
        cg::packet::receive::AlbumName::Parse(truncatedName),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Dn",
        "Dn truncated name es");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestTypedRoundtrip();
    failures += TestAlbumIdI62();
    failures += TestEscapedNameRoundtrip();
    failures += TestNameKeepsPipe();
    failures += TestEmptyName();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
