#include "packet/packet.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::ChatMessage::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::ChatMessage::Spec);
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

cg::packet::receive::ChatMessagePayload Payload(std::string textRaw)
{
    cg::packet::receive::ChatMessagePayload payload;
    payload.textRaw = std::move(textRaw);
    return payload;
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
        cg::packet::PacketCode(cg::packet::receive::ChatMessage::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::string BuildPlainZdr(
    std::string_view message,
    std::string_view objectIndex = "-1",
    std::string_view color = "4",
    std::string_view fontSize = "g")
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(message);

    std::string plain = "zdr ";
    plain.append(objectIndex);
    plain.push_back(' ');
    plain.append(encoded.HasValue() ? encoded.Value() : std::string());
    plain.push_back(' ');
    plain.append(color);
    plain.push_back(' ');
    plain.append(fontSize);
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

int ExpectChatPacket(
    const cg::packet::Packet& packet,
    std::string_view objectIndex,
    std::string_view message,
    std::string_view color,
    std::string_view fontSize,
    const char* label)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::ChatMessage::Code), label);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, label);
    failures += Expect(packet.fields.size() == 4U, label);
    if (packet.fields.size() != 4U) {
        return failures;
    }

    failures += ExpectField(packet, 0U, "objectIndex", cg::packet::PacketFieldType::Int62, objectIndex, label);
    failures += ExpectField(packet, 1U, "message", cg::packet::PacketFieldType::EscapedString, message, label);
    failures += ExpectField(packet, 2U, "color", cg::packet::PacketFieldType::Int62, color, label);
    failures += ExpectField(packet, 3U, "fontSize", cg::packet::PacketFieldType::Int62, fontSize, label);
    return failures;
}

int ExpectChatMessage(
    const cg::packet::receive::ChatMessage& value,
    int objectIndex,
    std::string_view textRaw,
    int color,
    int fontSize,
    const char* message)
{
    int failures = 0;
    failures += Expect(value.objectIndex == objectIndex, message);
    failures += Expect(value.payload.textRaw == textRaw, message);
    failures += Expect(value.color == color, message);
    failures += Expect(value.fontSize == fontSize, message);
    failures += Expect(value.BuildMessage() == std::string("P|").append(textRaw), message);
    return failures;
}

int TestSuggestedPacket()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket("zdr -1 P|hello 4 g ");

    int failures = Expect(static_cast<bool>(parsed), "parse zdr suggested packet");
    if (parsed.HasValue()) {
        failures += ExpectChatPacket(parsed.Value(), "-1", "P|hello", "4", "16", "zdr suggested packet fields");
    }

    const cg::packet::PacketResult<cg::packet::receive::ChatMessage> typed =
        cg::packet::receive::ChatMessage::Parse("zdr -1 P|hello 4 g ");
    failures += Expect(static_cast<bool>(typed), "typed parse zdr suggested packet");
    if (typed.HasValue()) {
        failures += ExpectChatMessage(typed.Value(), -1, "hello", 4, 16, "zdr suggested typed values");
    }
    return failures;
}

int TestBuildRoundtrip()
{
    const cg::packet::receive::ChatMessage message(-1, Payload("hello"), 4, 16);
    const cg::packet::PacketResult<std::string> built = BuildPacket(message.ToPacket());

    int failures = Expect(static_cast<bool>(built), "build zdr packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == "zdr -1 P|hello 4 g ", "zdr build suggested plain");

    const cg::packet::PacketResult<cg::packet::receive::ChatMessage> parsed =
        cg::packet::receive::ChatMessage::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip zdr packet");
    if (parsed.HasValue()) {
        failures += ExpectChatMessage(parsed.Value(), -1, "hello", 4, 16, "zdr roundtrip typed values");
    }
    return failures;
}

int TestSpaceTextRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::receive::ChatMessage> parsed =
        cg::packet::receive::ChatMessage::Parse("zdr -1 P|hello\\Sworld 4 g ");

    int failures = Expect(static_cast<bool>(parsed), "parse zdr space text");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectChatMessage(parsed.Value(), -1, "hello world", 4, 16, "zdr space text values");

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build zdr space text");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "zdr -1 P|hello\\Sworld 4 g ", "zdr space text plain");
    }
    return failures;
}

int TestInternalEscapesPreserved()
{
    const std::string textRaw = R"(name\zmsg\cmark\ybell\nliteral)";
    const cg::packet::receive::ChatMessage message(-1, Payload(textRaw), 4, 16);
    const cg::packet::PacketResult<std::string> built = BuildPacket(message.ToPacket());

    int failures = Expect(static_cast<bool>(built), "build zdr internal escapes");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(
        built.Value() == "zdr -1 P|name\\\\zmsg\\\\cmark\\\\ybell\\\\nliteral 4 g ",
        "zdr internal escapes plain");

    const cg::packet::PacketResult<cg::packet::receive::ChatMessage> parsed =
        cg::packet::receive::ChatMessage::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse zdr internal escapes");
    if (parsed.HasValue()) {
        failures += ExpectChatMessage(parsed.Value(), -1, textRaw, 4, 16, "zdr internal escapes preserved");
    }
    return failures;
}

int TestRawEscapesPreserved()
{
    const std::string textRaw = R"(name\zmsg\cmark\ybell\nliteral)";
    const cg::packet::PacketResult<cg::packet::receive::ChatMessage> parsed =
        cg::packet::receive::ChatMessage::Parse("zdr -1 P|name\\zmsg\\cmark\\ybell\\nliteral 4 g ");

    int failures = Expect(static_cast<bool>(parsed), "parse zdr raw escapes");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().payload.textRaw == textRaw, "zdr raw escapes preserved");
    }
    return failures;
}

int TestEmptyText()
{
    const cg::packet::PacketResult<cg::packet::receive::ChatMessage> parsed =
        cg::packet::receive::ChatMessage::Parse("zdr -1 P| 4 g ");

    int failures = Expect(static_cast<bool>(parsed), "parse zdr empty text");
    if (parsed.HasValue()) {
        failures += ExpectChatMessage(parsed.Value(), -1, "", 4, 16, "zdr empty text values");
    }

    const cg::packet::receive::ChatMessage message(-1, Payload(""), 4, 16);
    const cg::packet::PacketResult<std::string> built = BuildPacket(message.ToPacket());
    failures += Expect(static_cast<bool>(built), "build zdr empty text");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "zdr -1 P| 4 g ", "zdr empty text plain");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("zdr -1 P|hello 4"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "zdr",
        true,
        3U,
        "zdr missing fontSize");
    failures += ExpectError(
        ParsePacket("zdr -1 P|hello 4 g x "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "zdr",
        true,
        4U,
        "zdr extra field");
    failures += ExpectError(
        ParsePacket("zdr _ P|hello 4 g "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "zdr",
        true,
        0U,
        "zdr bad objectIndex");
    failures += ExpectError(
        cg::packet::receive::ChatMessage::Parse("zdr -1 P|hello _ g "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "zdr",
        true,
        2U,
        "typed zdr bad color");
    failures += ExpectError(
        cg::packet::receive::ChatMessage::Parse("zdr -1 P|hello 4 _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "zdr",
        true,
        3U,
        "typed zdr bad fontSize");
    failures += ExpectError(
        cg::packet::receive::ChatMessage::Parse(BuildPlainZdr("hello")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "zdr",
        true,
        1U,
        "typed zdr message missing marker delimiter");
    failures += ExpectError(
        cg::packet::receive::ChatMessage::Parse(BuildPlainZdr("Q|hello")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "zdr",
        true,
        1U,
        "typed zdr bad marker");
    failures += ExpectError(
        cg::packet::receive::ChatMessage::Parse(BuildPlainZdr("P|hello|tail")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "zdr",
        true,
        1U,
        "typed zdr payload extra delimiter");
    failures += ExpectError(
        cg::packet::receive::ChatMessage::Parse("Zd hello "),
        cg::packet::PacketErrorCode::InvalidCode,
        "Zd",
        false,
        0U,
        "typed zdr wrong code");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;
    failures += ExpectError(
        BuildPacket(PacketOf(cg::packet::PacketDirection::ServerToClient, {})),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "zdr",
        true,
        0U,
        "zdr build missing fields");
    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("objectIndex", cg::packet::PacketFieldType::Int62, "-1"),
                Field("message", cg::packet::PacketFieldType::EscapedString, "P|hello"),
                Field("color", cg::packet::PacketFieldType::Int62, "4"),
                Field("fontSize", cg::packet::PacketFieldType::Int62, "16"),
            })),
        cg::packet::PacketErrorCode::EncodeFailed,
        "zdr",
        false,
        0U,
        "zdr build wrong direction");
    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("objIndex", cg::packet::PacketFieldType::Int62, "-1"),
                Field("message", cg::packet::PacketFieldType::EscapedString, "P|hello"),
                Field("color", cg::packet::PacketFieldType::Int62, "4"),
                Field("fontSize", cg::packet::PacketFieldType::Int62, "16"),
            })),
        cg::packet::PacketErrorCode::EncodeFailed,
        "zdr",
        true,
        0U,
        "zdr build wrong field name");
    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("objectIndex", cg::packet::PacketFieldType::Int62, "-1"),
                Field("message", cg::packet::PacketFieldType::RawString, "P|hello"),
                Field("color", cg::packet::PacketFieldType::Int62, "4"),
                Field("fontSize", cg::packet::PacketFieldType::Int62, "16"),
            })),
        cg::packet::PacketErrorCode::EncodeFailed,
        "zdr",
        true,
        1U,
        "zdr build wrong field type");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestSuggestedPacket();
    failures += TestBuildRoundtrip();
    failures += TestSpaceTextRoundtrip();
    failures += TestInternalEscapesPreserved();
    failures += TestRawEscapesPreserved();
    failures += TestEmptyText();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
