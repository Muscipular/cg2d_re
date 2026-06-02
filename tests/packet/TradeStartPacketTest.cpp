#include "packet/packet.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace {

using TradeStart = cg::packet::receive::TradeStart;

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
        cg::packet::PacketCode(TradeStart::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::string BuildRawPlain(std::string_view nameToken, std::string_view levelToken)
{
    std::string plain = "ms ";
    plain.append(nameToken);
    plain.push_back(' ');
    plain.append(levelToken);
    plain.push_back(' ');
    return plain;
}

int ExpectTradeStartPacket(
    const cg::packet::Packet& packet,
    std::string_view nameRaw,
    std::string_view level,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(TradeStart::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "name", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[0].value == nameRaw, message);
    failures += Expect(packet.fields[1].name == "level", message);
    failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[1].value == level, message);
    return failures;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("ms Alice a ");

    int failures = Expect(static_cast<bool>(parsed), "parse ms outer");
    if (parsed.HasValue()) {
        failures += ExpectTradeStartPacket(parsed.Value(), "Alice", "10", "parse ms packet");
    }
    return failures;
}

int TestTypedRoundtrip()
{
    const TradeStart start("Alice", 10);
    const cg::packet::Packet packet = start.ToPacket();

    int failures = ExpectTradeStartPacket(packet, "Alice", "10", "ms ToPacket");
    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build ms packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == "ms Alice a ", "ms build value");

    const cg::packet::PacketResult<TradeStart> parsed = TradeStart::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse built ms");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().nameRaw == "Alice", "ms parsed nameRaw");
        failures += Expect(parsed.Value().level == 10, "ms parsed level");
    }
    return failures;
}

int TestRawEscapesPreserved()
{
    const std::string nameRaw = "A\\zB\\cC\\yD\\nE";
    const cg::packet::PacketResult<TradeStart> parsed =
        TradeStart::Parse(BuildRawPlain(nameRaw, "a"));

    int failures = Expect(static_cast<bool>(parsed), "parse ms raw escaped name");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().nameRaw == nameRaw, "ms keep raw escaped name");
    failures += Expect(parsed.Value().level == 10, "ms raw escaped level");

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build ms raw escaped");
    if (built.HasValue()) {
        failures += Expect(
            built.Value() == "ms A\\\\zB\\\\cC\\\\yD\\\\nE a ",
            "ms raw escaped build value");
    }
    return failures;
}

int TestEmptyNameAllowed()
{
    const cg::packet::PacketResult<TradeStart> parsed = TradeStart::Parse("ms  a ");

    int failures = Expect(static_cast<bool>(parsed), "parse ms empty name");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().nameRaw.empty(), "ms empty nameRaw");
    failures += Expect(parsed.Value().level == 10, "ms empty name level");

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build ms empty name");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "ms  a ", "ms empty name build value");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        TradeStart::Parse("Cxo Alice "),
        cg::packet::PacketErrorCode::InvalidCode,
        "Cxo",
        false,
        0U,
        "ms wrong code");
    failures += ExpectError(
        TradeStart::Parse("ms Alice"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ms",
        true,
        1U,
        "ms missing level");
    failures += ExpectError(
        TradeStart::Parse("ms Alice a extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ms",
        true,
        2U,
        "ms extra field");
    failures += ExpectError(
        TradeStart::Parse("ms Alice _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ms",
        true,
        1U,
        "ms bad i62");
    return failures;
}

int TestManualPacketErrors()
{
    int failures = 0;
    failures += ExpectError(
        TradeStart::FromPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {
                Field("name", cg::packet::PacketFieldType::EscapedString, "Alice"),
                Field("level", cg::packet::PacketFieldType::Int62, "10"),
            })),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ms",
        false,
        0U,
        "ms wrong direction");
    failures += ExpectError(
        TradeStart::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("nameRaw", cg::packet::PacketFieldType::EscapedString, "Alice"),
                Field("level", cg::packet::PacketFieldType::Int62, "10"),
            })),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ms",
        true,
        0U,
        "ms wrong field name");
    failures += ExpectError(
        TradeStart::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("name", cg::packet::PacketFieldType::RawString, "Alice"),
                Field("level", cg::packet::PacketFieldType::Int62, "10"),
            })),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "ms",
        true,
        0U,
        "ms wrong field type");
    failures += ExpectError(
        TradeStart::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {
                Field("name", cg::packet::PacketFieldType::EscapedString, "Alice"),
                Field("level", cg::packet::PacketFieldType::Int62, "a"),
            })),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ms",
        true,
        1U,
        "ms manual bad level");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestTypedRoundtrip();
    failures += TestRawEscapesPreserved();
    failures += TestEmptyNameAllowed();
    failures += TestParseErrors();
    failures += TestManualPacketErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
