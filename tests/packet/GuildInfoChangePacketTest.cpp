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
    return cg::packet::ParsePacket(plainTokens, cg::packet::send::GuildInfoChange::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::send::GuildInfoChange::Spec);
}

using GuildInfoChange = cg::packet::send::GuildInfoChange;

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
        cg::packet::PacketCode(GuildInfoChange::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::string BuildRawPlain(std::string_view cToken, std::string_view dataToken)
{
    std::string plain = "Ufm ";
    plain.append(cToken);
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

int ExpectGuildInfoChangePacket(
    const cg::packet::Packet& packet,
    std::string_view c,
    std::string_view data,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(GuildInfoChange::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "c", cg::packet::PacketFieldType::Int62, c, message);
    failures += ExpectField(packet, 1U, "data", cg::packet::PacketFieldType::EscapedString, data, message);
    return failures;
}

int TestToPacketAndEncode()
{
    const GuildInfoChange request(2, "guild name");

    int failures = ExpectGuildInfoChangePacket(request.ToPacket(), "2", "guild name", "Ufm ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(request.ToPacket());
    failures += Expect(static_cast<bool>(built), "Ufm BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "Ufm 2 guild\\Sname ", "Ufm BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "Ufm Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "Ufm 2 guild\\Sname ", "Ufm Encode value");
    }
    return failures;
}

int TestCValues()
{
    int failures = 0;

    const cg::packet::PacketResult<std::string> c2 = GuildInfoChange(2, "two").Encode();
    failures += Expect(static_cast<bool>(c2), "Ufm c=2 encode");
    if (c2.HasValue()) {
        failures += Expect(c2.Value() == "Ufm 2 two ", "Ufm c=2 value");
    }

    const cg::packet::PacketResult<std::string> c3 = GuildInfoChange(3, "three").Encode();
    failures += Expect(static_cast<bool>(c3), "Ufm c=3 encode");
    if (c3.HasValue()) {
        failures += Expect(c3.Value() == "Ufm 3 three ", "Ufm c=3 value");
    }

    const cg::packet::PacketResult<std::string> c6 = GuildInfoChange(6, "six").Encode();
    failures += Expect(static_cast<bool>(c6), "Ufm c=6 encode");
    if (c6.HasValue()) {
        failures += Expect(c6.Value() == "Ufm 6 six ", "Ufm c=6 value");
    }
    return failures;
}

int TestEmptyData()
{
    const GuildInfoChange request(2, "");
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "Ufm empty data encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "Ufm 2  ", "Ufm empty data value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "Ufm empty data parse");
    if (parsed.HasValue()) {
        failures += ExpectGuildInfoChangePacket(parsed.Value(), "2", "", "Ufm empty data packet");
    }
    return failures;
}

int TestEscapedDataRoundtrip()
{
    const std::string data = "A B\\C\nD\rE";
    const GuildInfoChange request(2, data);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "Ufm escaped data encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "Ufm 2 A\\SB\\\\C\\nD\\rE ", "Ufm escaped data value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "Ufm escaped data parse");
    if (parsed.HasValue()) {
        failures += ExpectGuildInfoChangePacket(parsed.Value(), "2", data, "Ufm escaped data packet");
    }
    return failures;
}

int TestPipeDataIsNotNested()
{
    const std::string data = "A|B,C";
    const GuildInfoChange request(2, data);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "Ufm pipe data encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "Ufm 2 A|B,C ", "Ufm pipe data value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildRawPlain("2", data));
    failures += Expect(static_cast<bool>(parsed), "Ufm pipe data parse");
    if (parsed.HasValue()) {
        failures += ExpectGuildInfoChangePacket(parsed.Value(), "2", data, "Ufm pipe data packet");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("Ufm 2"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Ufm",
        true,
        1U,
        "Ufm missing data");
    failures += ExpectError(
        ParsePacket("Ufm 2 data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Ufm",
        true,
        2U,
        "Ufm extra field");
    failures += ExpectError(
        ParsePacket("Ufm _ data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Ufm",
        true,
        0U,
        "Ufm bad c");
    failures += ExpectError(
        ParsePacket(BuildRawPlain("2", Bytes({0x81U}))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Ufm",
        true,
        1U,
        "Ufm bad data es");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongDirection = GuildInfoChange(2, "data").ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        BuildPacket(wrongDirection),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Ufm",
        false,
        0U,
        "Ufm wrong direction");

    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {Field("c", cg::packet::PacketFieldType::Int62, "2")})),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Ufm",
        true,
        1U,
        "Ufm manual missing field");

    cg::packet::Packet wrongName = GuildInfoChange(2, "data").ToPacket();
    wrongName.fields[1].name = "message";
    failures += ExpectError(
        BuildPacket(wrongName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Ufm",
        true,
        1U,
        "Ufm wrong field name");

    cg::packet::Packet wrongType = GuildInfoChange(2, "data").ToPacket();
    wrongType.fields[1].type = cg::packet::PacketFieldType::Int62;
    failures += ExpectError(
        BuildPacket(wrongType),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Ufm",
        true,
        1U,
        "Ufm wrong field type");

    cg::packet::Packet badC = GuildInfoChange(2, "data").ToPacket();
    badC.fields[0].value = "bad";
    failures += ExpectError(
        BuildPacket(badC),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Ufm",
        true,
        0U,
        "Ufm bad manual c");

    cg::packet::Packet badData = GuildInfoChange(2, "data").ToPacket();
    badData.fields[1].value = Bytes({0x81U});
    failures += ExpectError(
        BuildPacket(badData),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Ufm",
        true,
        1U,
        "Ufm bad manual data");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestToPacketAndEncode();
    failures += TestCValues();
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
