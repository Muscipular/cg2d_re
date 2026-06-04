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

using GuildMail = cg::packet::receive::GuildMail;
using GuildMailPayload = cg::packet::receive::GuildMailPayload;

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
        cg::packet::PacketCode(GuildMail::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::string BuildRawPlainRll(std::string_view fromRegistSeqNumber, std::string_view text, std::string_view color)
{
    std::string plain = "Rll ";
    plain.append(fromRegistSeqNumber);
    plain.push_back(' ');
    plain.append(text);
    plain.push_back(' ');
    plain.append(color);
    plain.push_back(' ');
    return plain;
}

std::string BuildPlainRll(std::string_view text)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(text);
    if (!encoded.HasValue()) {
        return BuildRawPlainRll("1", "", "3");
    }
    return BuildRawPlainRll("1", encoded.Value(), "3");
}

int ExpectGuildMailPacket(
    const cg::packet::Packet& packet,
    std::string_view text,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(GuildMail::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 3U, message);
    if (packet.fields.size() != 3U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "fromregistseqnumber", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[0].value == "1", message);
    failures += Expect(packet.fields[1].name == "text", message);
    failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[1].value == text, message);
    failures += Expect(packet.fields[2].name == "color", message);
    failures += Expect(packet.fields[2].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[2].value == "3", message);
    return failures;
}

int ExpectNormalMail(
    const GuildMail& value,
    std::string_view messageRaw,
    const char* message)
{
    int failures = 0;
    failures += Expect(value.fromRegistSeqNumber == 1, message);
    failures += Expect(value.color == 3, message);
    failures += Expect(!value.payload.isPetMail, message);
    failures += Expect(value.payload.sentTimeText == "2026/ 6/02 12:34", message);
    failures += Expect(value.payload.messageRaw == messageRaw, message);
    failures += Expect(value.payload.petBaseImageNumberOrMarker == -1, message);
    return failures;
}

int ExpectPetMail(
    const GuildMail& value,
    std::string_view messageRaw,
    std::string_view petNameRaw,
    const char* message)
{
    int failures = 0;
    failures += Expect(value.fromRegistSeqNumber == 1, message);
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
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlainRll(text));

    int failures = Expect(static_cast<bool>(parsed), "parse Rll outer");
    if (parsed.HasValue()) {
        failures += ExpectGuildMailPacket(parsed.Value(), text, "parse Rll packet");
    }
    return failures;
}

int TestNormalMailTypedParse()
{
    const cg::packet::PacketResult<GuildMail> parsed =
        GuildMail::Parse(BuildPlainRll("2026/ 6/02 12:34|hello|-1"));

    int failures = Expect(static_cast<bool>(parsed), "typed parse Rll normal mail");
    if (parsed.HasValue()) {
        failures += ExpectNormalMail(parsed.Value(), "hello", "typed Rll normal mail value");
        failures += Expect(parsed.Value().BuildData() == "2026/ 6/02 12:34|hello|-1", "build normal text");
    }
    return failures;
}

int TestPetMailTypedParse()
{
    const cg::packet::PacketResult<GuildMail> parsed =
        GuildMail::Parse(BuildPlainRll("2026/ 6/02 12:34|pet mail|62|10|Pochi|-1"));

    int failures = Expect(static_cast<bool>(parsed), "typed parse Rll pet mail");
    if (parsed.HasValue()) {
        failures += ExpectPetMail(parsed.Value(), "pet mail", "Pochi", "typed Rll pet mail value");
        failures += Expect(
            parsed.Value().BuildData() == "2026/ 6/02 12:34|pet mail|62|10|Pochi|-1",
            "build pet text");
    }
    return failures;
}

int TestNestedRawEscapesPreserved()
{
    const std::string text = "2026/06/02|A\\zB\\cC\\yD\\nE|62|10|P\\zName|-1";
    const cg::packet::PacketResult<GuildMail> parsed = GuildMail::Parse(BuildRawPlainRll("1", text, "3"));

    int failures = Expect(static_cast<bool>(parsed), "parse Rll raw nested escapes");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().payload.messageRaw == "A\\zB\\cC\\yD\\nE", "Rll raw message escapes");
        failures += Expect(parsed.Value().payload.petNameRaw == "P\\zName", "Rll raw pet name escapes");
        failures += Expect(parsed.Value().BuildData() == text, "Rll raw escape BuildData");
    }
    return failures;
}

int TestBuildRoundtrip()
{
    GuildMailPayload payload;
    payload.sentTimeText = "2026/ 6/02 12:34";
    payload.messageRaw = "hello";
    payload.petBaseImageNumberOrMarker = 0;
    const GuildMail message(1, std::move(payload), 3);

    const cg::packet::PacketResult<std::string> built = BuildPacket(message.ToPacket());
    int failures = Expect(static_cast<bool>(built), "build Rll normal mail");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == BuildPlainRll("2026/ 6/02 12:34|hello|-1"), "Rll normal build plain");
    const cg::packet::PacketResult<GuildMail> parsed = GuildMail::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip Rll normal mail");
    if (parsed.HasValue()) {
        failures += ExpectNormalMail(parsed.Value(), "hello", "roundtrip Rll normal value");
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
        GuildMail::Parse("Rll 1 text"),
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
        "Rll bad fromregistseqnumber");
    failures += ExpectError(
        GuildMail::Parse("Rll 1 time|msg|-1 _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rll",
        true,
        2U,
        "typed Rll bad color");
    failures += ExpectError(
        GuildMail::Parse("yuD time|msg|-1 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "yuD",
        false,
        0U,
        "typed Rll wrong code");
    return failures;
}

int TestPayloadFieldCountErrors()
{
    int failures = 0;
    failures += ExpectError(
        GuildMail::Parse(BuildPlainRll("")),
        cg::packet::PacketErrorCode::EmptyInput,
        "Rll",
        true,
        1U,
        "Rll empty text");
    failures += ExpectError(
        GuildMail::Parse(BuildPlainRll("one")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Rll",
        true,
        1U,
        "Rll payload 1 segment");
    failures += ExpectError(
        GuildMail::Parse(BuildPlainRll("one|two")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Rll",
        true,
        1U,
        "Rll payload 2 segments");
    failures += ExpectError(
        GuildMail::Parse(BuildPlainRll("time|message|-1|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Rll",
        true,
        1U,
        "Rll normal trailing empty segment");
    failures += ExpectError(
        GuildMail::Parse(BuildPlainRll("one|two|3|4")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Rll",
        true,
        1U,
        "Rll payload 4 segments");
    failures += ExpectError(
        GuildMail::Parse(BuildPlainRll("one|two|3|4|five")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Rll",
        true,
        1U,
        "Rll payload 5 segments");
    failures += ExpectError(
        GuildMail::Parse(BuildPlainRll("one|two|3|4|five|6|7")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Rll",
        true,
        1U,
        "Rll payload 7 segments");
    return failures;
}

int TestPayloadNumberErrors()
{
    int failures = 0;
    failures += ExpectError(
        GuildMail::Parse(BuildPlainRll("time|message|0")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rll",
        true,
        2U,
        "Rll normal marker must be -1");
    failures += ExpectError(
        GuildMail::Parse(BuildPlainRll("time|message|-01")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rll",
        true,
        2U,
        "Rll normal marker must be literal -1");
    failures += ExpectError(
        GuildMail::Parse(BuildPlainRll("time|message||10|Pochi|-1")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rll",
        true,
        2U,
        "Rll pet empty petBaseImageNumber");
    failures += ExpectError(
        GuildMail::Parse(BuildPlainRll("time|message|62|x|Pochi|-1")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rll",
        true,
        3U,
        "Rll pet bad level");
    failures += ExpectError(
        GuildMail::Parse(BuildPlainRll("time|message|62|10|Pochi|999999999999")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rll",
        true,
        5U,
        "Rll pet item overflow");
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
                Field("text", cg::packet::PacketFieldType::EscapedString, "time|msg|-1"),
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
                Field("fromRegistSeqNumber", cg::packet::PacketFieldType::Int62, "1"),
                Field("text", cg::packet::PacketFieldType::EscapedString, "time|msg|-1"),
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
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestNormalMailTypedParse();
    failures += TestPetMailTypedParse();
    failures += TestNestedRawEscapesPreserved();
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
