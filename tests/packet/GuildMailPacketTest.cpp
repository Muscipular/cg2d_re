#include "packet/Packet.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::GuildMail::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::GuildMail::Spec);
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
        cg::packet::PacketCode(cg::packet::receive::GuildMail::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
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

std::string SemanticMessage()
{
    std::string message = Bytes({0xC3, 0xFB, 0x96, 0x7C, 0x96, 0x5C});
    message.append("A|B,C\\D\nE");
    return message;
}

std::string EncodedMessage()
{
    std::string message = Bytes({0xC3, 0xFB, 0x96, 0x7C, 0x96, 0x5C});
    message.append("A\\zB\\cC\\yD\\nE");
    return message;
}

std::string SemanticPetName()
{
    std::string name = Bytes({0xD0, 0xA1, 0xC3, 0xFB, 0x96, 0x7C, 0x96, 0x5C});
    name.append("A|B");
    return name;
}

std::string EncodedPetName()
{
    std::string name = Bytes({0xD0, 0xA1, 0xC3, 0xFB, 0x96, 0x7C, 0x96, 0x5C});
    name.append("A\\zB");
    return name;
}

std::string NormalPayloadText()
{
    std::string text = "2026/ 8/ 1 12:34|";
    text.append(EncodedMessage());
    text.append("|-1");
    return text;
}

std::string PetPayloadText()
{
    std::string text = "2026/ 8/01 12:34|";
    text.append(EncodedMessage());
    text.append("|7300|10|");
    text.append(EncodedPetName());
    text.append("|-1");
    return text;
}

std::string BuildPlainRll(std::string_view text)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(text);
    if (!encoded.HasValue()) {
        return "Rll 1  3 ";
    }

    std::string plain = "Rll 1 ";
    plain.append(encoded.Value());
    plain.append(" 3 ");
    return plain;
}

int ExpectGuildMailPacket(
    const cg::packet::Packet& packet,
    std::string_view text,
    cg::packet::PacketFieldType textType,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::GuildMail::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 3U, message);
    if (packet.fields.size() != 3U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "fromregistseqnumber", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[0].value == "1", message);
    failures += Expect(packet.fields[1].name == "text", message);
    failures += Expect(packet.fields[1].type == textType, message);
    failures += Expect(packet.fields[1].value == text, message);
    failures += Expect(packet.fields[2].name == "color", message);
    failures += Expect(packet.fields[2].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[2].value == "3", message);
    return failures;
}

int ExpectNormalMail(
    const cg::packet::receive::GuildMail& value,
    std::string_view messageRaw,
    const char* message)
{
    int failures = 0;
    failures += Expect(value.fromRegistSeqNumber == 1, message);
    failures += Expect(value.color == 3, message);
    failures += Expect(!value.payload.isPetMail, message);
    failures += Expect(value.payload.sentTimeText == "2026/ 8/ 1 12:34", message);
    failures += Expect(value.payload.messageRaw == messageRaw, message);
    failures += Expect(value.payload.petBaseImageNumberOrMarker == -1, message);
    return failures;
}

int ExpectPetMail(
    const cg::packet::receive::GuildMail& value,
    std::string_view messageRaw,
    std::string_view petNameRaw,
    const char* message)
{
    int failures = 0;
    failures += Expect(value.fromRegistSeqNumber == 1, message);
    failures += Expect(value.color == 3, message);
    failures += Expect(value.payload.isPetMail, message);
    failures += Expect(value.payload.sentTimeText == "2026/ 8/01 12:34", message);
    failures += Expect(value.payload.messageRaw == messageRaw, message);
    failures += Expect(value.payload.petBaseImageNumberOrMarker == 7300, message);
    failures += Expect(value.payload.petLevel == 10, message);
    failures += Expect(value.payload.petNameRaw == petNameRaw, message);
    failures += Expect(value.payload.itemBaseImageNumber == -1, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string text = NormalPayloadText();
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlainRll(text));

    int failures = Expect(static_cast<bool>(parsed), "parse Rll outer");
    if (parsed.HasValue()) {
        failures += ExpectGuildMailPacket(
            parsed.Value(),
            text,
            cg::packet::PacketFieldType::EscapedString,
            "parse Rll packet");
    }
    return failures;
}

int TestNormalMailTypedParse()
{
    const cg::packet::PacketResult<cg::packet::receive::GuildMail> parsed =
        cg::packet::receive::GuildMail::Parse(BuildPlainRll(NormalPayloadText()));

    int failures = Expect(static_cast<bool>(parsed), "typed parse Rll normal mail");
    if (parsed.HasValue()) {
        failures += ExpectNormalMail(parsed.Value(), SemanticMessage(), "typed Rll normal mail value");
        const cg::packet::PacketResult<std::string> text = parsed.Value().BuildData();
        failures += Expect(text.HasValue(), "build normal text");
        if (text.HasValue()) {
            failures += Expect(text.Value() == NormalPayloadText(), "build normal text value");
        }
    }
    return failures;
}

int TestPetMailTypedParse()
{
    const cg::packet::PacketResult<cg::packet::receive::GuildMail> parsed =
        cg::packet::receive::GuildMail::Parse(BuildPlainRll(PetPayloadText()));

    int failures = Expect(static_cast<bool>(parsed), "typed parse Rll pet mail");
    if (parsed.HasValue()) {
        failures += ExpectPetMail(
            parsed.Value(),
            SemanticMessage(),
            SemanticPetName(),
            "typed Rll pet mail value");
        const cg::packet::PacketResult<std::string> text = parsed.Value().BuildData();
        failures += Expect(text.HasValue(), "build pet text");
        if (text.HasValue()) {
            failures += Expect(text.Value() == PetPayloadText(), "build pet text value");
        }
    }
    return failures;
}

int TestLegacyRoundtrip()
{
    cg::packet::receive::GuildMailPayload payload;
    payload.isPetMail = true;
    payload.sentTimeText = "2026/ 8/01 12:34";
    payload.messageRaw = SemanticMessage();
    payload.petBaseImageNumberOrMarker = 7300;
    payload.petLevel = 10;
    payload.petNameRaw = SemanticPetName();
    payload.itemBaseImageNumber = -1;
    const cg::packet::receive::GuildMail message(1, std::move(payload), 3);

    const cg::packet::PacketResult<cg::packet::Packet> packet = message.ToPacket();
    int failures = Expect(packet.HasValue(), "Rll ToPacket legacy payload");
    if (!packet.HasValue()) {
        return failures;
    }
    failures += ExpectGuildMailPacket(
        packet.Value(),
        PetPayloadText(),
        cg::packet::PacketFieldType::SubPayload,
        "Rll ToPacket legacy payload fields");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build Rll legacy payload");
    if (!built.HasValue()) {
        return failures;
    }
    failures += Expect(built.Value() == BuildPlainRll(PetPayloadText()), "Rll legacy canonical plain");

    const cg::packet::PacketResult<cg::packet::receive::GuildMail> parsed =
        cg::packet::receive::GuildMail::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse Rll legacy payload");
    if (parsed.HasValue()) {
        failures += ExpectPetMail(
            parsed.Value(),
            SemanticMessage(),
            SemanticPetName(),
            "Rll legacy roundtrip value");
    }
    return failures;
}

int TestBuildRoundtrip()
{
    cg::packet::receive::GuildMailPayload payload;
    payload.sentTimeText = "2026/ 8/ 1 12:34";
    payload.messageRaw = SemanticMessage();
    payload.petBaseImageNumberOrMarker = -1;
    const cg::packet::receive::GuildMail message(1, std::move(payload), 3);

    const cg::packet::PacketResult<cg::packet::Packet> packet = message.ToPacket();
    int failures = Expect(packet.HasValue(), "Rll normal ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    failures += ExpectGuildMailPacket(
        packet.Value(),
        NormalPayloadText(),
        cg::packet::PacketFieldType::SubPayload,
        "Rll normal ToPacket fields");
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build Rll normal mail");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == BuildPlainRll(NormalPayloadText()), "Rll normal build plain");
    const cg::packet::PacketResult<cg::packet::receive::GuildMail> parsed =
        cg::packet::receive::GuildMail::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip Rll normal mail");
    if (parsed.HasValue()) {
        failures += ExpectNormalMail(parsed.Value(), SemanticMessage(), "roundtrip Rll normal value");
    }
    return failures;
}

int TestOuterParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("Rll 1 text"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Rll",
        true,
        2U,
        "Rll missing color");
    failures += ExpectError(
        cg::packet::receive::GuildMail::Parse("Rll 1 text"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Rll",
        true,
        2U,
        "typed Rll missing color");
    failures += ExpectError(
        ParsePacket("Rll 1 text 3 extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Rll",
        true,
        3U,
        "Rll extra field");
    failures += ExpectError(
        ParsePacket("Rll _ text 3 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rll",
        true,
        0U,
        "Rll bad aindex");
    failures += ExpectError(
        cg::packet::receive::GuildMail::Parse("Rll 1 text _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rll",
        true,
        2U,
        "typed Rll bad color");
    failures += ExpectError(
        cg::packet::receive::GuildMail::Parse("LYIm BU "),
        cg::packet::PacketErrorCode::InvalidCode,
        "LYIm",
        false,
        0U,
        "typed Rll wrong code");
    return failures;
}

int TestPayloadFieldCountErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::GuildMail::Parse(BuildPlainRll("one")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Rll",
        true,
        1U,
        "Rll payload 1 segment");
    failures += ExpectError(
        cg::packet::receive::GuildMail::Parse(BuildPlainRll("one|two")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Rll",
        true,
        1U,
        "Rll payload 2 segments");
    failures += ExpectError(
        cg::packet::receive::GuildMail::Parse(BuildPlainRll("one|two|3|4")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Rll",
        true,
        1U,
        "Rll payload 4 segments");
    failures += ExpectError(
        cg::packet::receive::GuildMail::Parse(BuildPlainRll("one|two|3|4|five")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Rll",
        true,
        1U,
        "Rll payload 5 segments");
    failures += ExpectError(
        cg::packet::receive::GuildMail::Parse(BuildPlainRll("time|message|-1|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Rll",
        true,
        1U,
        "Rll normal trailing segment");
    failures += ExpectError(
        cg::packet::receive::GuildMail::Parse(BuildPlainRll("time|message|7300|10|pet|-1|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Rll",
        true,
        1U,
        "Rll pet trailing segment");
    return failures;
}

int TestPayloadNumberErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::GuildMail::Parse(BuildPlainRll("time|message|bad")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rll",
        true,
        2U,
        "Rll normal bad marker");
    failures += ExpectError(
        cg::packet::receive::GuildMail::Parse(BuildPlainRll("time|message|2147483648")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rll",
        true,
        2U,
        "Rll normal marker overflow");
    failures += ExpectError(
        cg::packet::receive::GuildMail::Parse(BuildPlainRll("time|message|0")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rll",
        true,
        2U,
        "Rll normal marker must be -1");
    failures += ExpectError(
        cg::packet::receive::GuildMail::Parse(BuildPlainRll("time|message|-01")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rll",
        true,
        2U,
        "Rll normal marker must be literal -1");
    failures += ExpectError(
        cg::packet::receive::GuildMail::Parse(BuildPlainRll("time|message|62|x|Pochi|-1")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rll",
        true,
        3U,
        "Rll pet bad level");
    failures += ExpectError(
        cg::packet::receive::GuildMail::Parse(BuildPlainRll("time|message|62|10|Pochi|999999999999")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rll",
        true,
        5U,
        "Rll pet item overflow");
    return failures;
}

int TestLegacyErrors()
{
    int failures = 0;
    std::string truncatedGbk = "time|message|-1";
    truncatedGbk.append(Bytes({0x96}));
    failures += ExpectError(
        cg::packet::receive::GuildMail::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("fromregistseqnumber", cg::packet::PacketFieldType::Int62, "1"),
                Field("text", cg::packet::PacketFieldType::EscapedString, truncatedGbk),
                Field("color", cg::packet::PacketFieldType::Int62, "3"),
            })),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rll",
        true,
        1U,
        "Rll payload truncated GBK");
    failures += ExpectError(
        cg::packet::receive::GuildMail::Parse(BuildPlainRll("time|message\\|-1")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rll",
        true,
        1U,
        "Rll message truncated legacy escape");
    failures += ExpectError(
        cg::packet::receive::GuildMail::Parse(BuildPlainRll("time|message|7300|10|pet\\|-1")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rll",
        true,
        1U,
        "Rll pet name truncated legacy escape");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;
    failures += ExpectError(
        BuildPacket(PacketOf(cg::packet::PacketDirection::ServerToClient, {})),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Rll",
        true,
        0U,
        "Rll build missing fields");
    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("fromregistseqnumber", cg::packet::PacketFieldType::Int62, "1"),
                Field("text", cg::packet::PacketFieldType::SubPayload, "time|msg|-1"),
                Field("color", cg::packet::PacketFieldType::Int62, "3"),
            })),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Rll",
        false,
        0U,
        "Rll build wrong direction");
    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("index", cg::packet::PacketFieldType::Int62, "1"),
                Field("text", cg::packet::PacketFieldType::SubPayload, "time|msg|-1"),
                Field("color", cg::packet::PacketFieldType::Int62, "3"),
            })),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Rll",
        true,
        0U,
        "Rll build wrong field name");
    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("fromregistseqnumber", cg::packet::PacketFieldType::Int62, "1"),
                Field("text", cg::packet::PacketFieldType::Unknown, "time|msg|-1"),
                Field("color", cg::packet::PacketFieldType::Int62, "3"),
            })),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "Rll",
        true,
        1U,
        "Rll build wrong field type");

    cg::packet::receive::GuildMailPayload payload;
    payload.sentTimeText = "time";
    payload.messageRaw = "message";
    payload.petBaseImageNumberOrMarker = 0;
    cg::packet::receive::GuildMail message(1, std::move(payload), 3);
    failures += ExpectError(
        message.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Rll",
        true,
        1U,
        "Rll build normal marker must be -1");
    failures += ExpectError(
        message.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Rll",
        true,
        1U,
        "Rll ToPacket normal marker must be -1");

    payload = {};
    payload.sentTimeText = "time|extra";
    payload.messageRaw = "message";
    message = cg::packet::receive::GuildMail(1, std::move(payload), 3);
    failures += ExpectError(
        message.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Rll",
        true,
        1U,
        "Rll build time delimiter");

    payload = {};
    payload.sentTimeText = "time";
    payload.messageRaw = Bytes({0x96});
    message = cg::packet::receive::GuildMail(1, std::move(payload), 3);
    failures += ExpectError(
        message.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Rll",
        true,
        1U,
        "Rll build truncated message GBK");

    payload = {};
    payload.isPetMail = true;
    payload.sentTimeText = "time";
    payload.messageRaw = "message";
    payload.petBaseImageNumberOrMarker = 7300;
    payload.petNameRaw = Bytes({0x96});
    message = cg::packet::receive::GuildMail(1, std::move(payload), 3);
    failures += ExpectError(
        message.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Rll",
        true,
        1U,
        "Rll build truncated pet name GBK");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestNormalMailTypedParse();
    failures += TestPetMailTypedParse();
    failures += TestLegacyRoundtrip();
    failures += TestBuildRoundtrip();
    failures += TestOuterParseErrors();
    failures += TestPayloadFieldCountErrors();
    failures += TestPayloadNumberErrors();
    failures += TestLegacyErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
