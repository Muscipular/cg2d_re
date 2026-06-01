#include "packet/packet.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace {

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
        cg::packet::PacketCode(cg::packet::receive::ProfileMailMessage::Code),
        direction,
        cg::packet::PacketSourceTag::ServerC);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::string BuildPlainMhbt(std::string_view text)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(text);
    if (!encoded.HasValue()) {
        return "Mhbt 1  3 ";
    }

    std::string plain = "Mhbt 1 ";
    plain.append(encoded.Value());
    plain.append(" 3 ");
    return plain;
}

int ExpectProfileMailMessagePacket(
    const cg::packet::Packet& packet,
    std::string_view text,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::ProfileMailMessage::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.source == cg::packet::PacketSourceTag::ServerC, message);
    failures += Expect(packet.fields.size() == 3U, message);
    if (packet.fields.size() != 3U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "aindex", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[0].source == cg::packet::PacketSourceTag::ServerC, message);
    failures += Expect(packet.fields[0].value == "1", message);
    failures += Expect(packet.fields[1].name == "text", message);
    failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[1].source == cg::packet::PacketSourceTag::ServerC, message);
    failures += Expect(packet.fields[1].value == text, message);
    failures += Expect(packet.fields[2].name == "color", message);
    failures += Expect(packet.fields[2].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[2].source == cg::packet::PacketSourceTag::ServerC, message);
    failures += Expect(packet.fields[2].value == "3", message);
    return failures;
}

int ExpectNormalMail(
    const cg::packet::receive::ProfileMailMessage& value,
    std::string_view messageRaw,
    const char* message)
{
    int failures = 0;
    failures += Expect(value.aindex == 1, message);
    failures += Expect(value.color == 3, message);
    failures += Expect(!value.payload.isPetMail, message);
    failures += Expect(value.payload.sentTimeText == "2026/ 6/02 12:34", message);
    failures += Expect(value.payload.messageRaw == messageRaw, message);
    failures += Expect(value.payload.petBaseImageNumberOrMarker == -1, message);
    return failures;
}

int ExpectPetMail(
    const cg::packet::receive::ProfileMailMessage& value,
    std::string_view messageRaw,
    std::string_view petNameRaw,
    const char* message)
{
    int failures = 0;
    failures += Expect(value.aindex == 1, message);
    failures += Expect(value.color == 3, message);
    failures += Expect(value.payload.isPetMail, message);
    failures += Expect(value.payload.sentTimeText == "2026/ 6/02 12:34", message);
    failures += Expect(value.payload.messageRaw == messageRaw, message);
    failures += Expect(value.payload.petBaseImageNumberOrMarker == 62, message);
    failures += Expect(value.payload.petLevel == 10, message);
    failures += Expect(value.payload.petNameRaw == petNameRaw, message);
    failures += Expect(value.payload.itemBaseImageNumber == -1, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string text = "2026/ 6/02 12:34|hello|-1";
    const cg::packet::PacketResult<cg::packet::Packet> parsed = cg::packet::ParsePacket(BuildPlainMhbt(text));

    int failures = Expect(static_cast<bool>(parsed), "parse Mhbt outer");
    if (parsed.HasValue()) {
        failures += ExpectProfileMailMessagePacket(parsed.Value(), text, "parse Mhbt packet");
    }
    return failures;
}

int TestNormalMailTypedParse()
{
    const cg::packet::PacketResult<cg::packet::receive::ProfileMailMessage> parsed =
        cg::packet::receive::ProfileMailMessage::Parse(BuildPlainMhbt("2026/ 6/02 12:34|hello|-1"));

    int failures = Expect(static_cast<bool>(parsed), "typed parse Mhbt normal mail");
    if (parsed.HasValue()) {
        failures += ExpectNormalMail(parsed.Value(), "hello", "typed Mhbt normal mail value");
        failures += Expect(parsed.Value().BuildText() == "2026/ 6/02 12:34|hello|-1", "build normal text");
    }
    return failures;
}

int TestPetMailTypedParse()
{
    const cg::packet::PacketResult<cg::packet::receive::ProfileMailMessage> parsed =
        cg::packet::receive::ProfileMailMessage::Parse(BuildPlainMhbt(
            "2026/ 6/02 12:34|pet mail|62|10|Pochi|-1"));

    int failures = Expect(static_cast<bool>(parsed), "typed parse Mhbt pet mail");
    if (parsed.HasValue()) {
        failures += ExpectPetMail(parsed.Value(), "pet mail", "Pochi", "typed Mhbt pet mail value");
        failures += Expect(
            parsed.Value().BuildText() == "2026/ 6/02 12:34|pet mail|62|10|Pochi|-1",
            "build pet text");
    }
    return failures;
}

int TestEscapedRoundtrip()
{
    cg::packet::receive::ProfileMailMessagePayload payload;
    payload.isPetMail = true;
    payload.sentTimeText = "2026/ 6/02 12:34";
    payload.messageRaw = "body with space and slash \\ tail";
    payload.petBaseImageNumberOrMarker = 62;
    payload.petLevel = 10;
    payload.petNameRaw = "Po chi\\name";
    payload.itemBaseImageNumber = -1;
    const cg::packet::receive::ProfileMailMessage message(1, std::move(payload), 3);

    int failures = ExpectProfileMailMessagePacket(
        message.ToPacket(),
        "2026/ 6/02 12:34|body with space and slash \\ tail|62|10|Po chi\\name|-1",
        "Mhbt ToPacket escaped payload");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(message.ToPacket());
    failures += Expect(static_cast<bool>(built), "build Mhbt escaped payload");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::receive::ProfileMailMessage> parsed =
        cg::packet::receive::ProfileMailMessage::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse Mhbt escaped payload");
    if (parsed.HasValue()) {
        failures += ExpectPetMail(
            parsed.Value(),
            "body with space and slash \\ tail",
            "Po chi\\name",
            "Mhbt escaped roundtrip value");
    }
    return failures;
}

int TestBuildRoundtrip()
{
    cg::packet::receive::ProfileMailMessagePayload payload;
    payload.sentTimeText = "2026/ 6/02 12:34";
    payload.messageRaw = "hello";
    payload.petBaseImageNumberOrMarker = -1;
    const cg::packet::receive::ProfileMailMessage message(1, std::move(payload), 3);

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(message.ToPacket());
    int failures = Expect(static_cast<bool>(built), "build Mhbt normal mail");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == BuildPlainMhbt("2026/ 6/02 12:34|hello|-1"), "Mhbt normal build plain");
    const cg::packet::PacketResult<cg::packet::receive::ProfileMailMessage> parsed =
        cg::packet::receive::ProfileMailMessage::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip Mhbt normal mail");
    if (parsed.HasValue()) {
        failures += ExpectNormalMail(parsed.Value(), "hello", "roundtrip Mhbt normal value");
    }
    return failures;
}

int TestOuterParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("Mhbt 1 text"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Mhbt",
        true,
        2U,
        "Mhbt missing color");
    failures += ExpectError(
        cg::packet::receive::ProfileMailMessage::Parse("Mhbt 1 text"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Mhbt",
        true,
        2U,
        "typed Mhbt missing color");
    failures += ExpectError(
        cg::packet::ParsePacket("Mhbt 1 text 3 extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Mhbt",
        true,
        3U,
        "Mhbt extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("Mhbt _ text 3 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Mhbt",
        true,
        0U,
        "Mhbt bad aindex");
    failures += ExpectError(
        cg::packet::receive::ProfileMailMessage::Parse("Mhbt 1 text _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Mhbt",
        true,
        2U,
        "typed Mhbt bad color");
    failures += ExpectError(
        cg::packet::receive::ProfileMailMessage::Parse("LYIm BU "),
        cg::packet::PacketErrorCode::InvalidCode,
        "LYIm",
        false,
        0U,
        "typed Mhbt wrong code");
    return failures;
}

int TestPayloadFieldCountErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::ProfileMailMessage::Parse(BuildPlainMhbt("one")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Mhbt",
        true,
        1U,
        "Mhbt payload 1 segment");
    failures += ExpectError(
        cg::packet::receive::ProfileMailMessage::Parse(BuildPlainMhbt("one|two")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Mhbt",
        true,
        1U,
        "Mhbt payload 2 segments");
    failures += ExpectError(
        cg::packet::receive::ProfileMailMessage::Parse(BuildPlainMhbt("one|two|3|4")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Mhbt",
        true,
        1U,
        "Mhbt payload 4 segments");
    failures += ExpectError(
        cg::packet::receive::ProfileMailMessage::Parse(BuildPlainMhbt("one|two|3|4|five")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Mhbt",
        true,
        1U,
        "Mhbt payload 5 segments");
    return failures;
}

int TestPayloadNumberErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::ProfileMailMessage::Parse(BuildPlainMhbt("time|message|bad")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Mhbt",
        true,
        2U,
        "Mhbt normal bad marker");
    failures += ExpectError(
        cg::packet::receive::ProfileMailMessage::Parse(BuildPlainMhbt("time|message|2147483648")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Mhbt",
        true,
        2U,
        "Mhbt normal marker overflow");
    failures += ExpectError(
        cg::packet::receive::ProfileMailMessage::Parse(BuildPlainMhbt("time|message|0")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Mhbt",
        true,
        2U,
        "Mhbt normal marker must be -1");
    failures += ExpectError(
        cg::packet::receive::ProfileMailMessage::Parse(BuildPlainMhbt("time|message|62|x|Pochi|-1")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Mhbt",
        true,
        3U,
        "Mhbt pet bad level");
    failures += ExpectError(
        cg::packet::receive::ProfileMailMessage::Parse(BuildPlainMhbt("time|message|62|10|Pochi|999999999999")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Mhbt",
        true,
        5U,
        "Mhbt pet item overflow");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(cg::packet::PacketDirection::ServerToClient, {})),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Mhbt",
        true,
        0U,
        "Mhbt build missing fields");
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("aindex", cg::packet::PacketFieldType::Int62, "1"),
                Field("text", cg::packet::PacketFieldType::EscapedString, "time|msg|-1"),
                Field("color", cg::packet::PacketFieldType::Int62, "3"),
            })),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Mhbt",
        false,
        0U,
        "Mhbt build wrong direction");
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("index", cg::packet::PacketFieldType::Int62, "1"),
                Field("text", cg::packet::PacketFieldType::EscapedString, "time|msg|-1"),
                Field("color", cg::packet::PacketFieldType::Int62, "3"),
            })),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Mhbt",
        true,
        0U,
        "Mhbt build wrong field name");
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("aindex", cg::packet::PacketFieldType::Int62, "1"),
                Field("text", cg::packet::PacketFieldType::RawString, "time|msg|-1"),
                Field("color", cg::packet::PacketFieldType::Int62, "3"),
            })),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Mhbt",
        true,
        1U,
        "Mhbt build wrong field type");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestNormalMailTypedParse();
    failures += TestPetMailTypedParse();
    failures += TestEscapedRoundtrip();
    failures += TestBuildRoundtrip();
    failures += TestOuterParseErrors();
    failures += TestPayloadFieldCountErrors();
    failures += TestPayloadNumberErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
