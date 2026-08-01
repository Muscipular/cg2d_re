#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <optional>
#include <string>
#include <string_view>
#include <utility>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::MapName::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::MapName::Spec);
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
    bool hasFieldIndex,
    std::size_t fieldIndex,
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

    if (hasFieldIndex) {
        failures += Expect(error.fieldIndex.has_value(), message);
        if (error.fieldIndex.has_value()) {
            failures += Expect(*error.fieldIndex == fieldIndex, message);
        }
    } else {
        failures += Expect(!error.fieldIndex.has_value(), message);
    }
    return failures;
}

std::string Bytes(std::initializer_list<unsigned char> bytes)
{
    std::string value;
    value.reserve(bytes.size());
    for (const unsigned char byte : bytes) {
        value.push_back(static_cast<char>(byte));
    }
    return value;
}

std::string EncodeLegacy(std::string_view value)
{
    const cg::packet::PacketResult<std::string> encoded =
        cg::packet::EncodeLegacyPayloadText(value);
    return encoded.HasValue() ? encoded.Value() : std::string();
}

std::string BuildPlainTeD(std::string_view outerDecodedData)
{
    const cg::packet::PacketResult<std::string> encoded =
        cg::packet::EncodeEscapedString(outerDecodedData);
    if (!encoded.HasValue()) {
        return {};
    }

    std::string plain = "TeD ";
    plain.append(encoded.Value());
    plain.push_back(' ');
    return plain;
}

std::string ActiveMapName()
{
    return Bytes({0xA5, 0xD0, 0xA5, 0xED, 0xA5, 0xB9});
}

std::string SemanticName()
{
    std::string name = Bytes({0xC3, 0xFB, 0x81, 0x7C, 0x81, 0x5C});
    name.append("A|B,C\\D\nE");
    return name;
}

std::string InnerLegacyName()
{
    return EncodeLegacy(SemanticName());
}

std::string WholeLegacyData(std::string_view innerPayload)
{
    return EncodeLegacy(innerPayload);
}

std::string SemanticWholeLegacyData()
{
    std::string innerPayload = InnerLegacyName();
    innerPayload.append("|12");
    return WholeLegacyData(innerPayload);
}

cg::packet::PacketField Field(
    std::string_view name,
    cg::packet::PacketFieldType type,
    std::string value)
{
    cg::packet::PacketField field;
    field.name = std::string(name);
    field.type = type;
    field.value = std::move(value);
    return field;
}

cg::packet::Packet PacketOf(
    cg::packet::PacketDirection direction,
    std::initializer_list<cg::packet::PacketField> fields)
{
    cg::packet::Packet packet(
        cg::packet::PacketCode(cg::packet::receive::MapName::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

int ExpectMapNamePacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    cg::packet::PacketFieldType type,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::MapName::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == type, message);
    failures += Expect(packet.fields[0].value == value, message);
    return failures;
}

int ExpectMapName(
    const cg::packet::receive::MapName& value,
    std::string_view name,
    std::optional<int> palette,
    const char* message)
{
    int failures = 0;
    failures += Expect(value.name == name, message);
    failures += Expect(value.palette == palette, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string data = SemanticWholeLegacyData();
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlainTeD(data));

    int failures = Expect(parsed.HasValue(), "parse TeD outer");
    if (parsed.HasValue()) {
        failures += ExpectMapNamePacket(
            parsed.Value(),
            data,
            cg::packet::PacketFieldType::EscapedString,
            "parse TeD outer fields");
    }
    return failures;
}

int TestActiveMap()
{
    std::string data = ActiveMapName();
    data.append("\\z12");
    const cg::packet::PacketResult<cg::packet::receive::MapName> parsed =
        cg::packet::receive::MapName::Parse(BuildPlainTeD(data));

    int failures = Expect(parsed.HasValue(), "typed parse TeD active map");
    if (parsed.HasValue()) {
        failures += ExpectMapName(parsed.Value(), ActiveMapName(), 12, "typed TeD active map value");
    }
    return failures;
}

int TestTwoLegacyRoundtrip()
{
    const cg::packet::receive::MapName mapName(SemanticName(), 12);
    const cg::packet::PacketResult<cg::packet::Packet> packet = mapName.ToPacket();

    int failures = Expect(packet.HasValue(), "TeD two legacy ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }
    failures += ExpectMapNamePacket(
        packet.Value(),
        SemanticWholeLegacyData(),
        cg::packet::PacketFieldType::SubPayload,
        "TeD two legacy ToPacket fields");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(built.HasValue(), "build TeD two legacy");
    if (!built.HasValue()) {
        return failures;
    }
    failures += Expect(
        built.Value() == BuildPlainTeD(SemanticWholeLegacyData()),
        "build TeD two legacy plain");

    const cg::packet::PacketResult<cg::packet::receive::MapName> parsed =
        cg::packet::receive::MapName::Parse(built.Value());
    failures += Expect(parsed.HasValue(), "parse TeD two legacy");
    if (parsed.HasValue()) {
        failures += ExpectMapName(parsed.Value(), SemanticName(), 12, "TeD two legacy semantic value");
    }
    return failures;
}

int TestNameOnlyAndEmpty()
{
    const cg::packet::PacketResult<cg::packet::receive::MapName> nameOnly =
        cg::packet::receive::MapName::Parse(BuildPlainTeD("Central City"));
    int failures = Expect(nameOnly.HasValue(), "typed parse TeD name only");
    if (nameOnly.HasValue()) {
        failures += ExpectMapName(nameOnly.Value(), "Central City", std::nullopt, "TeD name only value");
    }

    const cg::packet::PacketResult<cg::packet::receive::MapName> empty =
        cg::packet::receive::MapName::Parse("TeD ");
    failures += Expect(empty.HasValue(), "typed parse TeD empty");
    if (!empty.HasValue()) {
        return failures;
    }
    failures += ExpectMapName(empty.Value(), "", std::nullopt, "TeD empty value");

    const cg::packet::PacketResult<cg::packet::Packet> packet = empty.Value().ToPacket();
    failures += Expect(packet.HasValue(), "TeD empty ToPacket");
    if (packet.HasValue()) {
        const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
        failures += Expect(built.HasValue(), "build TeD empty");
        if (built.HasValue()) {
            failures += Expect(built.Value() == "TeD  ", "TeD empty canonical plain");
        }
    }
    return failures;
}

int TestPaletteRightPadding()
{
    const std::string data = WholeLegacyData("Floor|12   ");
    const cg::packet::PacketResult<cg::packet::receive::MapName> parsed =
        cg::packet::receive::MapName::Parse(BuildPlainTeD(data));

    int failures = Expect(parsed.HasValue(), "typed parse TeD padded palette");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectMapName(parsed.Value(), "Floor", 12, "TeD padded palette value");

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(packet.HasValue(), "TeD padded palette ToPacket");
    if (packet.HasValue()) {
        failures += Expect(
            packet.Value().fields[0].value == WholeLegacyData("Floor|12"),
            "TeD padded palette canonical value");
    }
    return failures;
}

int TestPayloadErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::MapName::Parse(BuildPlainTeD(WholeLegacyData("name|12|extra"))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TeD",
        true,
        0U,
        "TeD payload extra field");
    failures += ExpectError(
        cg::packet::receive::MapName::Parse(BuildPlainTeD(WholeLegacyData("name|"))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TeD",
        true,
        0U,
        "TeD empty palette");
    failures += ExpectError(
        cg::packet::receive::MapName::Parse(BuildPlainTeD(WholeLegacyData("name|x"))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TeD",
        true,
        0U,
        "TeD invalid palette");
    failures += ExpectError(
        cg::packet::receive::MapName::Parse(BuildPlainTeD(WholeLegacyData("name| 12"))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TeD",
        true,
        0U,
        "TeD palette leading space");
    failures += ExpectError(
        cg::packet::receive::MapName::Parse(BuildPlainTeD(WholeLegacyData("name|2147483648"))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TeD",
        true,
        0U,
        "TeD palette positive overflow");
    failures += ExpectError(
        cg::packet::receive::MapName::Parse(BuildPlainTeD(WholeLegacyData("name|-2147483649"))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TeD",
        true,
        0U,
        "TeD palette negative overflow");
    failures += ExpectError(
        cg::packet::receive::MapName::Parse(BuildPlainTeD("name\\")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TeD",
        true,
        0U,
        "TeD truncated whole legacy");
    failures += ExpectError(
        cg::packet::receive::MapName::Parse(BuildPlainTeD(WholeLegacyData("name\\"))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TeD",
        true,
        0U,
        "TeD truncated name legacy");

    const cg::packet::receive::MapName invalidName(Bytes({0x96}), std::nullopt);
    failures += ExpectError(
        invalidName.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "TeD",
        true,
        0U,
        "TeD truncated GBK name");
    return failures;
}

int TestOuterErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("TeD"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TeD",
        true,
        0U,
        "TeD missing field");
    failures += ExpectError(
        cg::packet::receive::MapName::Parse("TeD"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TeD",
        true,
        0U,
        "typed TeD missing field");
    failures += ExpectError(
        ParsePacket("TeD one two "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TeD",
        true,
        1U,
        "TeD extra field");
    failures += ExpectError(
        cg::packet::receive::MapName::Parse("TeD one two "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TeD",
        true,
        1U,
        "typed TeD extra field");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;
    failures += ExpectError(
        BuildPacket(PacketOf(cg::packet::PacketDirection::ServerToClient, {})),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TeD",
        true,
        0U,
        "TeD build missing field");
    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("data", cg::packet::PacketFieldType::SubPayload, "Central City"),
                Field("extra", cg::packet::PacketFieldType::SubPayload, "x"),
            })),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TeD",
        true,
        1U,
        "TeD build extra field");
    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {Field("data", cg::packet::PacketFieldType::SubPayload, "Central City")})),
        cg::packet::PacketErrorCode::EncodeFailed,
        "TeD",
        false,
        0U,
        "TeD direction mismatch");
    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("wrong", cg::packet::PacketFieldType::SubPayload, "Central City")})),
        cg::packet::PacketErrorCode::EncodeFailed,
        "TeD",
        true,
        0U,
        "TeD field name mismatch");
    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("data", cg::packet::PacketFieldType::Unknown, "Central City")})),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "TeD",
        true,
        0U,
        "TeD field type mismatch");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestActiveMap();
    failures += TestTwoLegacyRoundtrip();
    failures += TestNameOnlyAndEmpty();
    failures += TestPaletteRightPadding();
    failures += TestPayloadErrors();
    failures += TestOuterErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
