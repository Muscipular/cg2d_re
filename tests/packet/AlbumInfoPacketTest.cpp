#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

constexpr std::size_t NumericFieldCount = 15U;

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

std::array<int, NumericFieldCount> BuildValues()
{
    return {
        62,
        2,
        1,
        4,
        5,
        6,
        7,
        8,
        9,
        10,
        11,
        12,
        13,
        14,
        15,
    };
}

std::vector<std::string> BuildTokens(
    const std::array<int, NumericFieldCount>& values,
    std::string_view comment,
    std::string_view name)
{
    std::vector<std::string> tokens;
    tokens.reserve(NumericFieldCount + 3U);
    tokens.emplace_back(cg::packet::receive::AlbumInfo::Code);
    for (const int value : values) {
        const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeInt62(value);
        tokens.push_back(encoded.HasValue() ? encoded.Value() : std::string());
    }

    const cg::packet::PacketResult<std::string> encodedComment = cg::packet::EncodeEscapedString(comment);
    const cg::packet::PacketResult<std::string> encodedName = cg::packet::EncodeEscapedString(name);
    tokens.push_back(encodedComment.HasValue() ? encodedComment.Value() : std::string());
    tokens.push_back(encodedName.HasValue() ? encodedName.Value() : std::string());
    return tokens;
}

std::string BuildPlain(
    const std::array<int, NumericFieldCount>& values,
    std::string_view comment,
    std::string_view name)
{
    const cg::packet::PacketResult<std::string> plain =
        cg::packet::BuildPlainTokenString(BuildTokens(values, comment, name));
    return plain.HasValue() ? plain.Value() : std::string();
}

std::string BuildPlainWithToken(std::size_t fieldIndex, std::string token)
{
    std::vector<std::string> tokens = BuildTokens(BuildValues(), "comment", "name");
    tokens[fieldIndex + 1U] = std::move(token);
    const cg::packet::PacketResult<std::string> plain = cg::packet::BuildPlainTokenString(tokens);
    return plain.HasValue() ? plain.Value() : std::string();
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
    failures += Expect(packet.fields[index].source == cg::packet::PacketSourceTag::ServerC, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int ExpectAlbumInfoPacket(
    const cg::packet::Packet& packet,
    const std::array<int, NumericFieldCount>& values,
    std::string_view comment,
    std::string_view name,
    const char* message)
{
    static constexpr std::array<std::string_view, NumericFieldCount> fieldNames = {
        "albumId",
        "imageNumber",
        "catchFlag",
        "rare",
        "type",
        "vital",
        "strength",
        "toughness",
        "quick",
        "magic",
        "earth",
        "water",
        "fire",
        "wind",
        "slot",
    };

    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::AlbumInfo::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.source == cg::packet::PacketSourceTag::ServerC, message);
    failures += Expect(packet.fields.size() == 17U, message);
    if (packet.fields.size() != 17U) {
        return failures;
    }

    for (std::size_t index = 0; index < NumericFieldCount; ++index) {
        failures += ExpectField(
            packet,
            index,
            fieldNames[index],
            cg::packet::PacketFieldType::Int62,
            std::to_string(values[index]),
            message);
    }
    failures += ExpectField(packet, 15U, "comment", cg::packet::PacketFieldType::EscapedString, comment, message);
    failures += ExpectField(packet, 16U, "name", cg::packet::PacketFieldType::EscapedString, name, message);
    return failures;
}

int ExpectAlbumInfoValue(
    const cg::packet::receive::AlbumInfo& value,
    const std::array<int, NumericFieldCount>& values,
    std::string_view comment,
    std::string_view name,
    const char* message)
{
    int failures = 0;
    failures += Expect(value.albumId == values[0], message);
    failures += Expect(value.imageNumber == values[1], message);
    failures += Expect(value.catchFlag == values[2], message);
    failures += Expect(value.rare == values[3], message);
    failures += Expect(value.type == values[4], message);
    failures += Expect(value.vital == values[5], message);
    failures += Expect(value.strength == values[6], message);
    failures += Expect(value.toughness == values[7], message);
    failures += Expect(value.quick == values[8], message);
    failures += Expect(value.magic == values[9], message);
    failures += Expect(value.earth == values[10], message);
    failures += Expect(value.water == values[11], message);
    failures += Expect(value.fire == values[12], message);
    failures += Expect(value.wind == values[13], message);
    failures += Expect(value.slot == values[14], message);
    failures += Expect(value.comment == comment, message);
    failures += Expect(value.name == name, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::array<int, NumericFieldCount> values = BuildValues();
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(BuildPlain(values, "fast pet", "AlbumName"));

    int failures = Expect(static_cast<bool>(parsed), "parse dM outer");
    if (parsed.HasValue()) {
        failures += ExpectAlbumInfoPacket(parsed.Value(), values, "fast pet", "AlbumName", "parse dM packet");
    }
    return failures;
}

int TestTypedRoundtrip()
{
    const std::array<int, NumericFieldCount> values = BuildValues();
    const std::string comment = "A B\\C\nD";
    const std::string name = "Name With Space";
    const cg::packet::PacketResult<cg::packet::receive::AlbumInfo> parsed =
        cg::packet::receive::AlbumInfo::Parse(BuildPlain(values, comment, name));

    int failures = Expect(static_cast<bool>(parsed), "parse dM typed");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectAlbumInfoValue(parsed.Value(), values, comment, name, "dM typed values");
    failures += ExpectAlbumInfoPacket(parsed.Value().ToPacket(), values, comment, name, "dM ToPacket");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build dM");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlain(values, comment, name), "dM roundtrip");
    }
    return failures;
}

int TestRawDisplayEscapesPreserved()
{
    const std::array<int, NumericFieldCount> values = BuildValues();
    const std::string comment = "raw\\z/red\\c/yellow\\y";
    const std::string name = "name\\z";
    const cg::packet::PacketResult<cg::packet::receive::AlbumInfo> parsed =
        cg::packet::receive::AlbumInfo::Parse(BuildPlain(values, comment, name));

    int failures = Expect(static_cast<bool>(parsed), "parse dM raw display escapes");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().comment == comment, "dM raw comment preserved");
        failures += Expect(parsed.Value().name == name, "dM raw name preserved");
    }
    return failures;
}

int TestEmptyCommentAndName()
{
    const std::array<int, NumericFieldCount> values = BuildValues();
    const cg::packet::receive::AlbumInfo albumInfo(
        values[0],
        values[1],
        values[2],
        values[3],
        values[4],
        values[5],
        values[6],
        values[7],
        values[8],
        values[9],
        values[10],
        values[11],
        values[12],
        values[13],
        values[14],
        "",
        "");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(albumInfo.ToPacket());
    int failures = Expect(static_cast<bool>(built), "build dM empty strings");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == BuildPlain(values, "", ""), "dM empty strings plain");

    const cg::packet::PacketResult<cg::packet::receive::AlbumInfo> parsed =
        cg::packet::receive::AlbumInfo::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse dM empty strings");
    if (parsed.HasValue()) {
        failures += ExpectAlbumInfoValue(parsed.Value(), values, "", "", "dM empty strings value");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::AlbumInfo::Parse("Ilz 1 data "),
        cg::packet::PacketErrorCode::InvalidCode,
        "Ilz",
        "dM wrong code");

    std::vector<std::string> tokens = BuildTokens(BuildValues(), "comment", "name");
    tokens.pop_back();
    cg::packet::PacketResult<std::string> plain = cg::packet::BuildPlainTokenString(tokens);
    std::string missingFieldPlain = plain.HasValue() ? plain.Value() : std::string();
    if (!missingFieldPlain.empty()) {
        missingFieldPlain.pop_back();
    }
    failures += ExpectError(
        cg::packet::receive::AlbumInfo::Parse(missingFieldPlain),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "dM",
        "dM missing field");

    tokens = BuildTokens(BuildValues(), "comment", "name");
    tokens.emplace_back("extra");
    plain = cg::packet::BuildPlainTokenString(tokens);
    failures += ExpectError(
        cg::packet::receive::AlbumInfo::Parse(plain.HasValue() ? plain.Value() : std::string()),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "dM",
        "dM extra field");

    for (std::size_t index = 0; index < NumericFieldCount; ++index) {
        failures += ExpectError(
            cg::packet::receive::AlbumInfo::Parse(BuildPlainWithToken(index, "_")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "dM",
            "dM bad i62 field");
    }

    failures += ExpectError(
        cg::packet::receive::AlbumInfo::Parse(BuildPlainWithToken(6U, "zzzzzz")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "dM",
        "dM overflow i62 field");

    std::string truncated;
    truncated.push_back(static_cast<char>(0x80));
    failures += ExpectError(
        cg::packet::receive::AlbumInfo::Parse(BuildPlainWithToken(15U, truncated)),
        cg::packet::PacketErrorCode::DecodeFailed,
        "dM",
        "dM truncated comment es");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestTypedRoundtrip();
    failures += TestRawDisplayEscapesPreserved();
    failures += TestEmptyCommentAndName();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
