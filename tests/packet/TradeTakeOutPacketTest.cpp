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
    return cg::packet::ParsePacket(plainTokens, cg::packet::send::TradeTakeOut::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::send::TradeTakeOut::Spec);
}

using TradeTakeOut = cg::packet::send::TradeTakeOut;

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
        cg::packet::PacketCode(TradeTakeOut::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::string BuildRawPlain(std::string_view itemToken, std::string_view petToken, std::string_view goldToken)
{
    std::string plain = "aiAb ";
    plain.append(itemToken);
    plain.push_back(' ');
    plain.append(petToken);
    plain.push_back(' ');
    plain.append(goldToken);
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

int ExpectTradeTakeOutPacket(
    const cg::packet::Packet& packet,
    std::string_view item,
    std::string_view pet,
    std::string_view gold,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(TradeTakeOut::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.fields.size() == 3U, message);
    if (packet.fields.size() != 3U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "item", cg::packet::PacketFieldType::EscapedString, item, message);
    failures += ExpectField(packet, 1U, "pet", cg::packet::PacketFieldType::EscapedString, pet, message);
    failures += ExpectField(packet, 2U, "gold", cg::packet::PacketFieldType::Int62, gold, message);
    return failures;
}

int TestToPacketAndEncode()
{
    const TradeTakeOut request("8|12", "0|2", 10);

    int failures = ExpectTradeTakeOutPacket(request.ToPacket(), "8|12", "0|2", "10", "aiAb ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(request.ToPacket());
    failures += Expect(static_cast<bool>(built), "aiAb BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "aiAb 8|12 0|2 a ", "aiAb BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "aiAb Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "aiAb 8|12 0|2 a ", "aiAb Encode value");
    }
    return failures;
}

int TestEmptyItemPet()
{
    const TradeTakeOut request("", "", 0);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "aiAb empty item pet encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "aiAb   0 ", "aiAb empty item pet value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "aiAb empty item pet parse");
    if (parsed.HasValue()) {
        failures += ExpectTradeTakeOutPacket(parsed.Value(), "", "", "0", "aiAb empty item pet packet");
    }
    return failures;
}

int TestEscapedItemPetRoundtrip()
{
    const std::string item = "A B\\C\nD";
    const std::string pet = "P Q\\R";
    const TradeTakeOut request(item, pet, 10);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "aiAb escaped encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "aiAb A\\SB\\\\C\\nD P\\SQ\\\\R a ", "aiAb escaped value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "aiAb escaped parse");
    if (parsed.HasValue()) {
        failures += ExpectTradeTakeOutPacket(parsed.Value(), item, pet, "10", "aiAb escaped packet");
    }
    return failures;
}

int TestPipePayloadIsOpaque()
{
    const std::string item = "8|9";
    const std::string pet = "100|8|101|9";
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildRawPlain(item, pet, "a"));

    int failures = Expect(static_cast<bool>(parsed), "aiAb raw sample parse");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectTradeTakeOutPacket(parsed.Value(), item, pet, "10", "aiAb raw sample packet");

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value());
    failures += Expect(static_cast<bool>(built), "aiAb raw sample build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "aiAb 8|9 100|8|101|9 a ", "aiAb raw sample roundtrip");
    }
    return failures;
}

int TestEmptyGoldParsesAsZero()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildRawPlain("8|12", "0|2", ""));
    int failures = Expect(static_cast<bool>(parsed), "aiAb empty gold parse");
    if (parsed.HasValue()) {
        failures += ExpectTradeTakeOutPacket(parsed.Value(), "8|12", "0|2", "0", "aiAb empty gold packet");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("aiAb 8|12 0|2"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "aiAb",
        true,
        2U,
        "aiAb missing gold");
    failures += ExpectError(
        ParsePacket("aiAb 8|12 0|2 a extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "aiAb",
        true,
        3U,
        "aiAb extra field");
    failures += ExpectError(
        ParsePacket(BuildRawPlain("8|12", "0|2", "_")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "aiAb",
        true,
        2U,
        "aiAb bad gold");
    failures += ExpectError(
        ParsePacket(BuildRawPlain(Bytes({0x81U}), "0|2", "a")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "aiAb",
        true,
        0U,
        "aiAb bad item es");
    failures += ExpectError(
        ParsePacket(BuildRawPlain("8|12", Bytes({0x81U}), "a")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "aiAb",
        true,
        1U,
        "aiAb bad pet es");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongDirection = TradeTakeOut("8|12", "0|2", 10).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        BuildPacket(wrongDirection),
        cg::packet::PacketErrorCode::EncodeFailed,
        "aiAb",
        false,
        0U,
        "aiAb wrong direction");

    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("item", cg::packet::PacketFieldType::EscapedString, "8|12"),
                Field("pet", cg::packet::PacketFieldType::EscapedString, "0|2"),
            })),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "aiAb",
        true,
        2U,
        "aiAb manual missing field");

    cg::packet::Packet wrongName = TradeTakeOut("8|12", "0|2", 10).ToPacket();
    wrongName.fields[1].name = "pets";
    failures += ExpectError(
        BuildPacket(wrongName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "aiAb",
        true,
        1U,
        "aiAb wrong field name");

    cg::packet::Packet wrongType = TradeTakeOut("8|12", "0|2", 10).ToPacket();
    wrongType.fields[0].type = cg::packet::PacketFieldType::RawString;
    failures += ExpectError(
        BuildPacket(wrongType),
        cg::packet::PacketErrorCode::EncodeFailed,
        "aiAb",
        true,
        0U,
        "aiAb wrong field type");

    cg::packet::Packet emptyGold = TradeTakeOut("8|12", "0|2", 10).ToPacket();
    emptyGold.fields[2].value.clear();
    failures += ExpectError(
        BuildPacket(emptyGold),
        cg::packet::PacketErrorCode::EncodeFailed,
        "aiAb",
        true,
        2U,
        "aiAb empty manual gold");

    cg::packet::Packet badGold = TradeTakeOut("8|12", "0|2", 10).ToPacket();
    badGold.fields[2].value = "bad";
    failures += ExpectError(
        BuildPacket(badGold),
        cg::packet::PacketErrorCode::EncodeFailed,
        "aiAb",
        true,
        2U,
        "aiAb bad manual gold");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestToPacketAndEncode();
    failures += TestEmptyItemPet();
    failures += TestEscapedItemPetRoundtrip();
    failures += TestPipePayloadIsOpaque();
    failures += TestEmptyGoldParsesAsZero();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
