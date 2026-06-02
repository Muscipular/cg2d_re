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
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::AlbumFlags::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::AlbumFlags::Spec);
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

std::string BuildPlain(std::string_view code, std::string_view albumVersion, std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedPayload = cg::packet::EncodeEscapedString(payload);
    std::string plain(code);
    plain.push_back(' ');
    plain.append(albumVersion);
    plain.push_back(' ');
    if (encodedPayload.HasValue()) {
        plain.append(encodedPayload.Value());
    }
    plain.push_back(' ');
    return plain;
}

std::string BuildPlainIlz(std::string_view albumVersion, std::string_view payload)
{
    return BuildPlain(cg::packet::receive::AlbumFlags::Code, albumVersion, payload);
}

int ExpectAlbumFlagsPacket(
    const cg::packet::Packet& packet,
    std::string_view albumVersion,
    std::string_view data,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::AlbumFlags::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "albumversion", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[0].value == albumVersion, message);
    failures += Expect(packet.fields[1].name == "data", message);
    failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[1].value == data, message);
    return failures;
}

int ExpectPages(
    const std::vector<int>& pages,
    const std::vector<int>& expected,
    const char* message)
{
    int failures = Expect(pages.size() == expected.size(), message);
    if (pages.size() != expected.size()) {
        return failures;
    }

    for (std::size_t index = 0; index < pages.size(); ++index) {
        failures += Expect(pages[index] == expected[index], message);
    }
    return failures;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlainIlz("2", "1|0|-1|"));

    int failures = Expect(static_cast<bool>(parsed), "parse Ilz outer");
    if (parsed.HasValue()) {
        failures += ExpectAlbumFlagsPacket(parsed.Value(), "2", "1|0|-1|", "parse Ilz outer packet");
    }
    return failures;
}

int TestRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::receive::AlbumFlags> parsed =
        cg::packet::receive::AlbumFlags::Parse(BuildPlainIlz("2", "1|0|-1|"));

    int failures = Expect(static_cast<bool>(parsed), "parse Ilz pages");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().albumVersion == 2, "Ilz albumVersion");
    failures += ExpectPages(parsed.Value().pages, {1, 0, -1}, "Ilz pages");
    failures += Expect(parsed.Value().BuildData() == "1|0|-1|", "Ilz BuildData");
    failures += ExpectAlbumFlagsPacket(parsed.Value().ToPacket(), "2", "1|0|-1|", "Ilz ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build Ilz packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainIlz("2", "1|0|-1|"), "roundtrip Ilz packet");
    }
    return failures;
}

int TestVersionI62()
{
    const cg::packet::PacketResult<cg::packet::receive::AlbumFlags> parsed =
        cg::packet::receive::AlbumFlags::Parse(BuildPlainIlz("10", "1|"));

    int failures = Expect(static_cast<bool>(parsed), "parse Ilz i62 version");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().albumVersion == 62, "Ilz version 10 is 62");
        failures += ExpectPages(parsed.Value().pages, {1}, "Ilz i62 version pages");
    }
    return failures;
}

int TestEmptyData()
{
    const cg::packet::PacketResult<cg::packet::receive::AlbumFlags> parsed =
        cg::packet::receive::AlbumFlags::Parse(BuildPlainIlz("2", ""));

    int failures = Expect(static_cast<bool>(parsed), "parse Ilz empty data");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().albumVersion == 2, "Ilz empty albumVersion");
        failures += Expect(parsed.Value().pages.empty(), "Ilz empty pages");
        failures += Expect(parsed.Value().BuildData().empty(), "Ilz empty BuildData");
    }
    return failures;
}

int TestNoTrailingDelimiter()
{
    const cg::packet::PacketResult<cg::packet::receive::AlbumFlags> parsed =
        cg::packet::receive::AlbumFlags::Parse(BuildPlainIlz("2", "1|0|-1"));

    int failures = Expect(static_cast<bool>(parsed), "parse Ilz no trailing delimiter");
    if (parsed.HasValue()) {
        failures += ExpectPages(parsed.Value().pages, {1, 0, -1}, "Ilz no trailing pages");
        failures += Expect(parsed.Value().BuildData() == "1|0|-1|", "Ilz build adds trailing delimiter");
    }
    return failures;
}

int TestConstructAndBuild()
{
    const cg::packet::receive::AlbumFlags albumFlags(2, {1, 0, -1});
    int failures = ExpectAlbumFlagsPacket(albumFlags.ToPacket(), "2", "1|0|-1|", "Ilz construct ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(albumFlags.ToPacket());
    failures += Expect(static_cast<bool>(built), "build constructed Ilz");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainIlz("2", "1|0|-1|"), "constructed Ilz plain");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::AlbumFlags::Parse(BuildPlainIlz("2", "1|x|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Ilz",
        "Ilz bad page decimal");
    failures += ExpectError(
        cg::packet::receive::AlbumFlags::Parse(BuildPlainIlz("2", "1||2|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Ilz",
        "Ilz middle empty page");
    failures += ExpectError(
        cg::packet::receive::AlbumFlags::Parse(BuildPlainIlz("2", "2147483648|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Ilz",
        "Ilz page overflow");
    failures += ExpectError(
        cg::packet::receive::AlbumFlags::Parse("Ilz _ 1| "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Ilz",
        "Ilz bad albumversion i62");
    failures += ExpectError(
        cg::packet::receive::AlbumFlags::Parse("rU data "),
        cg::packet::PacketErrorCode::InvalidCode,
        "rU",
        "Ilz wrong code");
    failures += ExpectError(
        cg::packet::receive::AlbumFlags::Parse("Ilz 2"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Ilz",
        "Ilz missing data");
    failures += ExpectError(
        cg::packet::receive::AlbumFlags::Parse("Ilz 2 1| extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Ilz",
        "Ilz extra field");

    std::string truncatedLegacy = "Ilz 2 ";
    truncatedLegacy.push_back(static_cast<char>(0x80));
    truncatedLegacy.push_back(' ');
    failures += ExpectError(
        cg::packet::receive::AlbumFlags::Parse(truncatedLegacy),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Ilz",
        "Ilz bad outer es");

    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestRoundtrip();
    failures += TestVersionI62();
    failures += TestEmptyData();
    failures += TestNoTrailingDelimiter();
    failures += TestConstructAndBuild();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
