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
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::BattleCommand::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::BattleCommand::Spec);
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
        cg::packet::PacketCode(cg::packet::receive::BattleCommand::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

int ExpectBattleCommandPacket(
    const cg::packet::Packet& packet,
    std::string_view command,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::BattleCommand::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "command", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[0].value == command, message);
    return failures;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket("LYIm BU ");

    int failures = Expect(static_cast<bool>(parsed), "parse LYIm BU");
    if (parsed.HasValue()) {
        failures += ExpectBattleCommandPacket(parsed.Value(), "BU", "parse LYIm packet");
    }
    return failures;
}

int TestPipeRoundtrip()
{
    const std::string command = "P|14|1|0";
    const cg::packet::receive::BattleCommand battleCommand(command);

    int failures = ExpectBattleCommandPacket(battleCommand.ToPacket(), command, "LYIm ToPacket pipe");

    const cg::packet::PacketResult<std::string> built = BuildPacket(battleCommand.ToPacket());
    failures += Expect(static_cast<bool>(built), "build LYIm pipe command");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == "LYIm P|14|1|0 ", "build LYIm preserves pipe");

    const cg::packet::PacketResult<cg::packet::receive::BattleCommand> parsed =
        cg::packet::receive::BattleCommand::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse LYIm pipe roundtrip");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().command == command, "roundtrip LYIm pipe command");
    }
    return failures;
}

int TestEscapedSpaceRoundtrip()
{
    const std::string command = "BU target now";
    const cg::packet::receive::BattleCommand battleCommand(command);
    const cg::packet::PacketResult<std::string> built = BuildPacket(battleCommand.ToPacket());

    int failures = Expect(static_cast<bool>(built), "build LYIm escaped space");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == "LYIm BU\\Starget\\Snow ", "build LYIm escaped space token");

    const cg::packet::PacketResult<cg::packet::receive::BattleCommand> parsed =
        cg::packet::receive::BattleCommand::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse LYIm escaped space");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().command == command, "roundtrip LYIm escaped space command");
    }
    return failures;
}

int TestEmptyCommand()
{
    const cg::packet::receive::BattleCommand battleCommand("");
    const cg::packet::PacketResult<std::string> built = BuildPacket(battleCommand.ToPacket());

    int failures = Expect(static_cast<bool>(built), "build LYIm empty command");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == "LYIm  ", "build LYIm empty command plain");

    const cg::packet::PacketResult<cg::packet::receive::BattleCommand> parsed =
        cg::packet::receive::BattleCommand::Parse("LYIm  ");
    failures += Expect(static_cast<bool>(parsed), "parse LYIm empty command");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().command.empty(), "LYIm empty command value");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ParsePacket("LYIm"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "LYIm",
        true,
        0U,
        "LYIm missing command");
    failures += ExpectError(
        cg::packet::receive::BattleCommand::Parse("LYIm"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "LYIm",
        true,
        0U,
        "typed LYIm missing command");
    failures += ExpectError(
        ParsePacket("LYIm BU extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "LYIm",
        true,
        1U,
        "LYIm extra field");
    failures += ExpectError(
        cg::packet::receive::BattleCommand::Parse("Zd BU "),
        cg::packet::PacketErrorCode::InvalidCode,
        "Zd",
        false,
        0U,
        "typed LYIm wrong code");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;
    failures += ExpectError(
        BuildPacket(PacketOf(cg::packet::PacketDirection::ClientToServer, {
            Field("command", cg::packet::PacketFieldType::EscapedString, "BU"),
        })),
        cg::packet::PacketErrorCode::EncodeFailed,
        "LYIm",
        false,
        0U,
        "LYIm build wrong direction");
    failures += ExpectError(
        BuildPacket(PacketOf(cg::packet::PacketDirection::ServerToClient, {
            Field("data", cg::packet::PacketFieldType::EscapedString, "BU"),
        })),
        cg::packet::PacketErrorCode::EncodeFailed,
        "LYIm",
        true,
        0U,
        "LYIm build wrong field name");
    failures += ExpectError(
        BuildPacket(PacketOf(cg::packet::PacketDirection::ServerToClient, {
            Field("command", cg::packet::PacketFieldType::Unknown, "BU"),
        })),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "LYIm",
        true,
        0U,
        "LYIm build wrong field type");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestPipeRoundtrip();
    failures += TestEscapedSpaceRoundtrip();
    failures += TestEmptyCommand();
    failures += TestParseErrors();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
