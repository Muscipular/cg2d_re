#include "packet/PacketBuilder.hpp"
#include "packet/send.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::send::GuildTitleSet::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::send::GuildTitleSet::Spec);
}

using GuildTitleSet = cg::packet::send::GuildTitleSet;

int Expect(bool condition, const char* message)
{
    if (condition) {
        return 0;
    }

    std::cerr << "EXPECT failed: " << message << '\n';
    return 1;
}

std::string Bytes(std::initializer_list<unsigned int> values)
{
    std::string text;
    text.reserve(values.size());
    for (const unsigned int value : values) {
        text.push_back(static_cast<char>(value));
    }
    return text;
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
        cg::packet::PacketCode(GuildTitleSet::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::string BuildRawPlain(std::string_view indexToken, std::string_view bitflagToken, std::string_view dataToken)
{
    std::string plain = "GY ";
    plain.append(indexToken);
    plain.push_back(' ');
    plain.append(bitflagToken);
    plain.push_back(' ');
    plain.append(dataToken);
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
    int failures = 0;
    failures += Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }

    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(packet.fields[index].type == type, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int ExpectGuildTitleSetPacket(
    const cg::packet::Packet& packet,
    std::string_view index,
    std::string_view bitflag,
    std::string_view data,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(GuildTitleSet::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.fields.size() == 3U, message);
    if (packet.fields.size() != 3U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "index", cg::packet::PacketFieldType::Int62, index, message);
    failures += ExpectField(packet, 1U, "bitflag", cg::packet::PacketFieldType::Int62, bitflag, message);
    failures += ExpectField(packet, 2U, "data", cg::packet::PacketFieldType::EscapedString, data, message);
    return failures;
}

int TestToPacketAndEncode()
{
    const GuildTitleSet request(10, 128, "Guild Room");

    int failures = ExpectGuildTitleSetPacket(request.ToPacket(), "10", "128", "Guild Room", "GY ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(request.ToPacket());
    failures += Expect(static_cast<bool>(built), "GY BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "GY a 24 Guild\\SRoom ", "GY BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "GY Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "GY a 24 Guild\\SRoom ", "GY Encode value");
    }
    return failures;
}

int TestRoundtrip()
{
    const GuildTitleSet request(10, 128, "Guild Room");
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "GY roundtrip encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "GY roundtrip parse");
    if (parsed.HasValue()) {
        failures += ExpectGuildTitleSetPacket(parsed.Value(), "10", "128", "Guild Room", "GY roundtrip packet");
    }
    return failures;
}

int TestEmptyData()
{
    const GuildTitleSet request(10, 128, "");
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "GY empty data encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "GY a 24  ", "GY empty data value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "GY empty data parse");
    if (parsed.HasValue()) {
        failures += ExpectGuildTitleSetPacket(parsed.Value(), "10", "128", "", "GY empty data packet");
    }
    return failures;
}

int TestEscapedDataRoundtrip()
{
    const std::string data = "A B\\C\nD\rE";
    const GuildTitleSet request(10, 128, data);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "GY escaped data encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "GY a 24 A\\SB\\\\C\\nD\\rE ", "GY escaped data value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "GY escaped data parse");
    if (parsed.HasValue()) {
        failures += ExpectGuildTitleSetPacket(parsed.Value(), "10", "128", data, "GY escaped data packet");
    }
    return failures;
}

int TestPipeDataIsNotNested()
{
    const std::string data = "Guild|Room,Title";
    const GuildTitleSet request(10, 128, data);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "GY pipe data encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "GY a 24 Guild|Room,Title ", "GY pipe data value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildRawPlain("a", "24", data));
    failures += Expect(static_cast<bool>(parsed), "GY pipe data parse");
    if (parsed.HasValue()) {
        failures += ExpectGuildTitleSetPacket(parsed.Value(), "10", "128", data, "GY pipe data packet");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("GY a 24"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "GY",
        true,
        2U,
        "GY missing data");
    failures += ExpectError(
        ParsePacket("GY a 24 data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "GY",
        true,
        3U,
        "GY extra field");
    failures += ExpectError(
        ParsePacket("GY _ 24 data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "GY",
        true,
        0U,
        "GY bad index");
    failures += ExpectError(
        ParsePacket("GY a _ data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "GY",
        true,
        1U,
        "GY bad bitflag");
    failures += ExpectError(
        ParsePacket(BuildRawPlain("a", "24", Bytes({0x81U}))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "GY",
        true,
        2U,
        "GY bad data es");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongDirection = GuildTitleSet(10, 128, "data").ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        BuildPacket(wrongDirection),
        cg::packet::PacketErrorCode::EncodeFailed,
        "GY",
        false,
        0U,
        "GY wrong direction");

    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("index", cg::packet::PacketFieldType::Int62, "10"),
                Field("bitflag", cg::packet::PacketFieldType::Int62, "128"),
            })),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "GY",
        true,
        2U,
        "GY manual missing field");

    cg::packet::Packet wrongName = GuildTitleSet(10, 128, "data").ToPacket();
    wrongName.fields[2].name = "guildRoom";
    failures += ExpectError(
        BuildPacket(wrongName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "GY",
        true,
        2U,
        "GY wrong field name");

    cg::packet::Packet wrongType = GuildTitleSet(10, 128, "data").ToPacket();
    wrongType.fields[2].type = cg::packet::PacketFieldType::Int62;
    failures += ExpectError(
        BuildPacket(wrongType),
        cg::packet::PacketErrorCode::EncodeFailed,
        "GY",
        true,
        2U,
        "GY wrong field type");

    cg::packet::Packet badIndex = GuildTitleSet(10, 128, "data").ToPacket();
    badIndex.fields[0].value = "bad";
    failures += ExpectError(
        BuildPacket(badIndex),
        cg::packet::PacketErrorCode::EncodeFailed,
        "GY",
        true,
        0U,
        "GY bad manual index");

    cg::packet::Packet badBitflag = GuildTitleSet(10, 128, "data").ToPacket();
    badBitflag.fields[1].value = "bad";
    failures += ExpectError(
        BuildPacket(badBitflag),
        cg::packet::PacketErrorCode::EncodeFailed,
        "GY",
        true,
        1U,
        "GY bad manual bitflag");

    cg::packet::Packet badData = GuildTitleSet(10, 128, "data").ToPacket();
    badData.fields[2].value = Bytes({0x81U});
    failures += ExpectError(
        BuildPacket(badData),
        cg::packet::PacketErrorCode::EncodeFailed,
        "GY",
        true,
        2U,
        "GY bad manual data");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestToPacketAndEncode();
    failures += TestRoundtrip();
    failures += TestEmptyData();
    failures += TestEscapedDataRoundtrip();
    failures += TestPipeDataIsNotNested();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
