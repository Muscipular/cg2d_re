#include "packet/PacketBuilder.hpp"
#include "packet/send.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace {

const cg::packet::PacketSpec& PacketSpec() noexcept
{
    return cg::packet::send::GuildMailSend::Spec;
}

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, PacketSpec());
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, PacketSpec());
}

using GuildMailSend = cg::packet::send::GuildMailSend;

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
        cg::packet::PacketCode(GuildMailSend::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::string BuildRawPlain(
    std::string_view toregistNumberToken,
    std::string_view textToken,
    std::string_view colorToken)
{
    std::string plain = "uId ";
    plain.append(toregistNumberToken);
    plain.push_back(' ');
    plain.append(textToken);
    plain.push_back(' ');
    plain.append(colorToken);
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

int ExpectGuildMailSendPacket(
    const cg::packet::Packet& packet,
    std::string_view toregistNumber,
    std::string_view text,
    std::string_view color,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(GuildMailSend::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ClientToServer, message);
    failures += Expect(packet.fields.size() == 3U, message);
    if (packet.fields.size() != 3U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "toregistNumber", cg::packet::PacketFieldType::Int62, toregistNumber, message);
    failures += ExpectField(packet, 1U, "text", cg::packet::PacketFieldType::EscapedString, text, message);
    failures += ExpectField(packet, 2U, "color", cg::packet::PacketFieldType::Int62, color, message);
    return failures;
}

int TestToPacketAndEncode()
{
    const GuildMailSend request(10, "Guild Mail", 6);

    int failures = ExpectGuildMailSendPacket(request.ToPacket(), "10", "Guild Mail", "6", "uId ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(request.ToPacket());
    failures += Expect(static_cast<bool>(built), "uId BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "uId a Guild\\SMail 6 ", "uId BuildPacket value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(static_cast<bool>(encoded), "uId Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "uId a Guild\\SMail 6 ", "uId Encode value");
    }
    return failures;
}

int TestEmptyText()
{
    const GuildMailSend request(10, "", 6);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "uId empty text encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "uId a  6 ", "uId empty text value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "uId empty text parse");
    if (parsed.HasValue()) {
        failures += ExpectGuildMailSendPacket(parsed.Value(), "10", "", "6", "uId empty text packet");
    }
    return failures;
}

int TestEscapedTextRoundtrip()
{
    const std::string text = "A B\\C\nD\rE";
    const GuildMailSend request(10, text, 6);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "uId escaped text encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "uId a A\\SB\\\\C\\nD\\rE 6 ", "uId escaped text value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "uId escaped text parse");
    if (parsed.HasValue()) {
        failures += ExpectGuildMailSendPacket(parsed.Value(), "10", text, "6", "uId escaped text packet");
    }
    return failures;
}

int TestPipeTextIsNotNested()
{
    const std::string text = "Guild|Mail|Text";
    const GuildMailSend request(10, text, 6);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "uId pipe text encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    failures += Expect(encoded.Value() == "uId a Guild|Mail|Text 6 ", "uId pipe text value");

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildRawPlain("a", text, "6"));
    failures += Expect(static_cast<bool>(parsed), "uId pipe text parse");
    if (parsed.HasValue()) {
        failures += ExpectGuildMailSendPacket(parsed.Value(), "10", text, "6", "uId pipe text packet");
    }
    return failures;
}

int TestParsePacketRoundtrip()
{
    const GuildMailSend request(10, "Guild Mail", 6);
    const cg::packet::PacketResult<std::string> encoded = request.Encode();

    int failures = Expect(static_cast<bool>(encoded), "uId roundtrip encode");
    if (!encoded.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(encoded.Value());
    failures += Expect(static_cast<bool>(parsed), "uId roundtrip parse");
    if (parsed.HasValue()) {
        failures += ExpectGuildMailSendPacket(parsed.Value(), "10", "Guild Mail", "6", "uId roundtrip packet");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("uId a text"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "uId",
        true,
        2U,
        "uId missing color");
    failures += ExpectError(
        ParsePacket("uId a text 6 extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "uId",
        true,
        3U,
        "uId extra field");
    failures += ExpectError(
        ParsePacket("uId _ text 6 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "uId",
        true,
        0U,
        "uId bad toregistNumber");
    failures += ExpectError(
        ParsePacket("uId a text _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "uId",
        true,
        2U,
        "uId bad color");
    failures += ExpectError(
        ParsePacket(BuildRawPlain("a", Bytes({0x81U}), "6")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "uId",
        true,
        1U,
        "uId bad text es");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongDirection = GuildMailSend(10, "text", 6).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        BuildPacket(wrongDirection),
        cg::packet::PacketErrorCode::EncodeFailed,
        "uId",
        false,
        0U,
        "uId wrong direction");

    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("toregistNumber", cg::packet::PacketFieldType::Int62, "10"),
                Field("text", cg::packet::PacketFieldType::EscapedString, "text"),
            })),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "uId",
        true,
        2U,
        "uId manual missing field");

    cg::packet::Packet wrongName = GuildMailSend(10, "text", 6).ToPacket();
    wrongName.fields[1].name = "message";
    failures += ExpectError(
        BuildPacket(wrongName),
        cg::packet::PacketErrorCode::EncodeFailed,
        "uId",
        true,
        1U,
        "uId wrong field name");

    cg::packet::Packet wrongType = GuildMailSend(10, "text", 6).ToPacket();
    wrongType.fields[1].type = cg::packet::PacketFieldType::RawString;
    failures += ExpectError(
        BuildPacket(wrongType),
        cg::packet::PacketErrorCode::EncodeFailed,
        "uId",
        true,
        1U,
        "uId wrong field type");

    cg::packet::Packet badToregistNumber = GuildMailSend(10, "text", 6).ToPacket();
    badToregistNumber.fields[0].value = "bad";
    failures += ExpectError(
        BuildPacket(badToregistNumber),
        cg::packet::PacketErrorCode::EncodeFailed,
        "uId",
        true,
        0U,
        "uId bad manual toregistNumber");

    cg::packet::Packet badColor = GuildMailSend(10, "text", 6).ToPacket();
    badColor.fields[2].value = "bad";
    failures += ExpectError(
        BuildPacket(badColor),
        cg::packet::PacketErrorCode::EncodeFailed,
        "uId",
        true,
        2U,
        "uId bad manual color");

    cg::packet::Packet badText = GuildMailSend(10, "text", 6).ToPacket();
    badText.fields[1].value = Bytes({0x81U});
    failures += ExpectError(
        BuildPacket(badText),
        cg::packet::PacketErrorCode::EncodeFailed,
        "uId",
        true,
        1U,
        "uId bad manual text");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestToPacketAndEncode();
    failures += TestEmptyText();
    failures += TestEscapedTextRoundtrip();
    failures += TestPipeTextIsNotNested();
    failures += TestParsePacketRoundtrip();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
