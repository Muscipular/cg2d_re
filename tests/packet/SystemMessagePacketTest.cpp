#include "packet/packet.hpp"

#include <cstddef>
#include <iostream>
#include <initializer_list>
#include <string>
#include <string_view>
#include <utility>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::SystemMessage::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::SystemMessage::Spec);
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
        cg::packet::PacketCode(cg::packet::receive::SystemMessage::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

int ExpectSystemMessagePacket(
    const cg::packet::Packet& packet,
    std::string_view message,
    const char* label)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::SystemMessage::Code), label);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, label);
    failures += Expect(packet.fields.size() == 1U, label);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "message", label);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, label);
    failures += Expect(packet.fields[0].value == message, label);
    return failures;
}

int TestParsePacket()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket("Zd hello ");

    int failures = Expect(static_cast<bool>(parsed), "parse Zd");
    if (parsed.HasValue()) {
        failures += ExpectSystemMessagePacket(parsed.Value(), "hello", "parse Zd packet");
    }
    return failures;
}

int TestTypedRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::receive::SystemMessage> parsed =
        cg::packet::receive::SystemMessage::Parse("Zd hello ");

    int failures = Expect(static_cast<bool>(parsed), "parse Zd typed");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().message == "hello", "Zd message");
    failures += ExpectSystemMessagePacket(parsed.Value().ToPacket(), "hello", "Zd ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build Zd packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "Zd hello ", "roundtrip Zd packet");
    }
    return failures;
}

int TestEmptyMessage()
{
    const cg::packet::receive::SystemMessage systemMessage("");
    const cg::packet::PacketResult<std::string> built = BuildPacket(systemMessage.ToPacket());

    int failures = Expect(static_cast<bool>(built), "build Zd empty message");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == "Zd  ", "Zd empty message plain");

    const cg::packet::PacketResult<cg::packet::receive::SystemMessage> parsed =
        cg::packet::receive::SystemMessage::Parse("Zd ");
    failures += Expect(static_cast<bool>(parsed), "parse Zd empty message");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().message.empty(), "Zd empty message value");
    }
    return failures;
}

int TestEscapedRoundtrip()
{
    const std::string message = "hello world\npath\\to\\z|tail";
    const cg::packet::receive::SystemMessage systemMessage(message);
    const cg::packet::PacketResult<std::string> built = BuildPacket(systemMessage.ToPacket());

    int failures = Expect(static_cast<bool>(built), "build Zd escaped message");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == "Zd hello\\Sworld\\npath\\\\to\\\\z|tail ", "Zd escaped plain");

    const cg::packet::PacketResult<cg::packet::receive::SystemMessage> parsed =
        cg::packet::receive::SystemMessage::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse Zd escaped message");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().message == message, "Zd escaped message value");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("Zd"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Zd",
        true,
        0U,
        "Zd missing field");
    failures += ExpectError(
        cg::packet::receive::SystemMessage::Parse("Zd"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Zd",
        true,
        0U,
        "typed Zd missing field");
    failures += ExpectError(
        ParsePacket("Zd one two "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Zd",
        true,
        1U,
        "Zd extra field");
    failures += ExpectError(
        cg::packet::receive::SystemMessage::Parse("Zd one two "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Zd",
        true,
        1U,
        "typed Zd extra field");
    failures += ExpectError(
        cg::packet::receive::SystemMessage::Parse("TeD hello "),
        cg::packet::PacketErrorCode::InvalidCode,
        "TeD",
        false,
        0U,
        "typed Zd wrong code");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;
    failures += ExpectError(
        BuildPacket(PacketOf(cg::packet::PacketDirection::ServerToClient, {})),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Zd",
        true,
        0U,
        "Zd build missing field");
    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("message", cg::packet::PacketFieldType::EscapedString, "hello"),
                Field("extra", cg::packet::PacketFieldType::EscapedString, "x"),
            })),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Zd",
        true,
        1U,
        "Zd build extra field");
    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {Field("message", cg::packet::PacketFieldType::EscapedString, "hello")})),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Zd",
        false,
        0U,
        "Zd direction mismatch");
    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("wrong", cg::packet::PacketFieldType::EscapedString, "hello")})),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Zd",
        true,
        0U,
        "Zd field name mismatch");
    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("message", cg::packet::PacketFieldType::Payload, "hello")})),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "Zd",
        true,
        0U,
        "Zd field type mismatch");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacket();
    failures += TestTypedRoundtrip();
    failures += TestEmptyMessage();
    failures += TestEscapedRoundtrip();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
