#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::PlayerPositionSync;

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
    int failures = Expect(result.HasError(), message);
    if (!result.HasError()) {
        return failures;
    }

    const cg::packet::PacketError& error = result.Error();
    failures += Expect(error.code == code, message);
    failures += Expect(error.packetCode.has_value(), message);
    if (error.packetCode.has_value()) {
        failures += Expect(error.packetCode->View() == packetCode, message);
    }
    failures += Expect(error.fieldIndex.has_value() == hasFieldIndex, message);
    if (hasFieldIndex && error.fieldIndex.has_value()) {
        failures += Expect(*error.fieldIndex == fieldIndex, message);
    }
    return failures;
}

int ExpectField(
    const cg::packet::Packet& packet,
    std::size_t index,
    std::string_view name,
    std::string_view value,
    const char* message)
{
    int failures = Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }
    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(packet.fields[index].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int TestSpecAndToPacket()
{
    const PlayerPositionSync sync(10, -1, 36);
    const cg::packet::Packet packet = sync.ToPacket();

    int failures = 0;
    failures += Expect(PlayerPositionSync::Code == "QBt", "QBt code");
    failures += Expect(
        PlayerPositionSync::Direction == cg::packet::PacketDirection::ServerToClient,
        "QBt direction");
    failures += Expect(PlayerPositionSync::Fields.size() == 3U, "QBt fields");
    failures += Expect(PlayerPositionSync::Fields[0].name == "x", "QBt x field name");
    failures += Expect(PlayerPositionSync::Fields[1].name == "y", "QBt y field name");
    failures += Expect(PlayerPositionSync::Fields[2].name == "dir", "QBt dir field name");
    for (const cg::packet::PacketFieldSpec& field : PlayerPositionSync::Fields) {
        failures += Expect(field.type == cg::packet::PacketFieldType::Int62, "QBt field type");
    }
    failures += Expect(PlayerPositionSync::Spec.code == PlayerPositionSync::Code, "QBt Spec code");
    failures += Expect(
        PlayerPositionSync::Spec.direction == PlayerPositionSync::Direction,
        "QBt Spec direction");
    failures += Expect(PlayerPositionSync::Spec.fieldCount == 3U, "QBt Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("QBt"), "QBt packet code");
    failures += Expect(packet.direction == PlayerPositionSync::Direction, "QBt packet direction");
    failures += Expect(packet.fields.size() == 3U, "QBt packet fields");
    failures += ExpectField(packet, 0U, "x", "10", "QBt x packet field");
    failures += ExpectField(packet, 1U, "y", "-1", "QBt y packet field");
    failures += ExpectField(packet, 2U, "dir", "36", "QBt dir packet field");
    return failures;
}

int TestTypedParseAndCanonical()
{
    const cg::packet::PacketResult<PlayerPositionSync> parsed =
        PlayerPositionSync::Parse("QBt a -1 A ");

    int failures = Expect(parsed.HasValue(), "QBt typed parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().x == 10, "QBt x value");
    failures += Expect(parsed.Value().y == -1, "QBt y value");
    failures += Expect(parsed.Value().direction == 36, "QBt direction value");

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value().ToPacket(), PlayerPositionSync::Spec);
    failures += Expect(built.HasValue(), "QBt typed build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "QBt a -1 A ", "QBt typed canonical");
    }
    return failures;
}

int TestGenericRoundtripAndUnrestrictedValues()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("QBt a -1 A", PlayerPositionSync::Spec);

    int failures = Expect(parsed.HasValue(), "QBt generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value(), PlayerPositionSync::Spec);
    failures += Expect(built.HasValue(), "QBt generic build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "QBt a -1 A ", "QBt generic canonical");
    }

    const cg::packet::PacketResult<PlayerPositionSync> unrestricted =
        PlayerPositionSync::Parse("QBt -1 -2 10 ");
    failures += Expect(unrestricted.HasValue(), "QBt unrestricted parse");
    if (unrestricted.HasValue()) {
        failures += Expect(unrestricted.Value().x == -1, "QBt negative x retained");
        failures += Expect(unrestricted.Value().y == -2, "QBt negative y retained");
        failures += Expect(unrestricted.Value().direction == 62, "QBt unusual direction retained");
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 3U>& tokens)
{
    return "QBt " + tokens[0] + ' ' + tokens[1] + ' ' + tokens[2] + ' ';
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        PlayerPositionSync::Parse("bad a -1 A "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "QBt wrong code");
    failures += ExpectError(
        PlayerPositionSync::Parse("QBt a -1"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "QBt",
        true,
        2U,
        "QBt missing field");
    failures += ExpectError(
        PlayerPositionSync::Parse("QBt a -1 A 0 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "QBt",
        true,
        3U,
        "QBt extra field");

    const std::array<std::string, 3U> valid = {"a", "-1", "A"};
    for (std::size_t index = 0U; index < valid.size(); ++index) {
        std::array<std::string, 3U> invalid = valid;
        invalid[index] = "_";
        failures += ExpectError(
            PlayerPositionSync::Parse(BuildPlain(invalid)),
            cg::packet::PacketErrorCode::DecodeFailed,
            "QBt",
            true,
            index,
            "QBt invalid i62");

        std::array<std::string, 3U> overflow = valid;
        overflow[index] = "2lkCB2";
        failures += ExpectError(
            PlayerPositionSync::Parse(BuildPlain(overflow)),
            cg::packet::PacketErrorCode::DecodeFailed,
            "QBt",
            true,
            index,
            "QBt i62 overflow");
    }
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndToPacket()
        + TestTypedParseAndCanonical()
        + TestGenericRoundtripAndUnrestrictedValues()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " player position sync packet test(s) failed\n";
        return 1;
    }
    return 0;
}
