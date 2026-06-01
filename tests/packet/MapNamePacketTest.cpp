#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <cstddef>
#include <iostream>
#include <initializer_list>
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
        cg::packet::PacketCode(cg::packet::receive::MapName::Code),
        direction,
        cg::packet::PacketSourceTag::Xlsx);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

int ExpectMapNamePacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::MapName::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.source == cg::packet::PacketSourceTag::Xlsx, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[0].source == cg::packet::PacketSourceTag::Xlsx, message);
    failures += Expect(packet.fields[0].value == value, message);
    return failures;
}

int TestParsePacket()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("TeD Central\\SCity ");
    int failures = Expect(static_cast<bool>(parsed), "parse TeD");
    if (parsed.HasValue()) {
        failures += ExpectMapNamePacket(parsed.Value(), "Central City", "parse TeD packet");
    }
    return failures;
}

int TestTypedParseAndBuild()
{
    const cg::packet::PacketResult<cg::packet::receive::MapName> parsed =
        cg::packet::receive::MapName::Parse("TeD Central\\SCity ");
    int failures = Expect(static_cast<bool>(parsed), "typed parse TeD");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().data == "Central City", "typed TeD data");
    const cg::packet::Packet packet = parsed.Value().ToPacket();
    failures += ExpectMapNamePacket(packet, "Central City", "typed TeD ToPacket");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build typed TeD");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "TeD Central\\SCity ", "build typed TeD value");
    }
    return failures;
}

int TestBusinessEscapedString()
{
    const std::string data = "Floor\\z3\\c4";
    const cg::packet::receive::MapName mapName(data);
    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(mapName.ToPacket());

    int failures = Expect(static_cast<bool>(built), "build TeD business escaped string");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == "TeD Floor\\\\z3\\\\c4 ", "build TeD business escaped value");

    const cg::packet::PacketResult<cg::packet::receive::MapName> parsed =
        cg::packet::receive::MapName::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse TeD business escaped string");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().data == data, "parse TeD business escaped value");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("TeD"),
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
        cg::packet::ParsePacket("TeD one two "),
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
        cg::packet::BuildPacket(PacketOf(cg::packet::PacketDirection::ServerToClient, {})),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TeD",
        true,
        0U,
        "TeD build missing field");
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("data", cg::packet::PacketFieldType::EscapedString, "Central City"),
                Field("extra", cg::packet::PacketFieldType::EscapedString, "x"),
            })),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TeD",
        true,
        1U,
        "TeD build extra field");
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {Field("data", cg::packet::PacketFieldType::EscapedString, "Central City")})),
        cg::packet::PacketErrorCode::EncodeFailed,
        "TeD",
        false,
        0U,
        "TeD direction mismatch");
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("wrong", cg::packet::PacketFieldType::EscapedString, "Central City")})),
        cg::packet::PacketErrorCode::EncodeFailed,
        "TeD",
        true,
        0U,
        "TeD field name mismatch");
    failures += ExpectError(
        cg::packet::BuildPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("data", cg::packet::PacketFieldType::RawString, "Central City")})),
        cg::packet::PacketErrorCode::EncodeFailed,
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
    failures += TestParsePacket();
    failures += TestTypedParseAndBuild();
    failures += TestBusinessEscapedString();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
