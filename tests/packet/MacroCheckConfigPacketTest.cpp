#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::MacroCheckConfig;

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
    const MacroCheckConfig config(60, 50);
    const cg::packet::Packet packet = config.ToPacket();

    int failures = 0;
    failures += Expect(MacroCheckConfig::Code == "tq", "tq code");
    failures += Expect(
        MacroCheckConfig::Direction == cg::packet::PacketDirection::ServerToClient,
        "tq direction");
    failures += Expect(MacroCheckConfig::Fields.size() == 2U, "tq fields");
    failures += Expect(MacroCheckConfig::Fields[0].name == "listMax", "tq listMax name");
    failures += Expect(
        MacroCheckConfig::Fields[1].name == "repeatThreshold",
        "tq repeatThreshold name");
    failures += Expect(
        MacroCheckConfig::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "tq listMax type");
    failures += Expect(
        MacroCheckConfig::Fields[1].type == cg::packet::PacketFieldType::Int62,
        "tq repeatThreshold type");
    failures += Expect(MacroCheckConfig::Spec.code == MacroCheckConfig::Code, "tq Spec code");
    failures += Expect(
        MacroCheckConfig::Spec.direction == MacroCheckConfig::Direction,
        "tq Spec direction");
    failures += Expect(MacroCheckConfig::Spec.fieldCount == 2U, "tq Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("tq"), "tq packet code");
    failures += Expect(packet.direction == MacroCheckConfig::Direction, "tq packet direction");
    failures += Expect(packet.fields.size() == 2U, "tq packet fields");
    failures += ExpectField(packet, 0U, "listMax", "60", "tq listMax packet field");
    failures += ExpectField(packet, 1U, "repeatThreshold", "50", "tq repeat packet field");
    return failures;
}

int ExpectValues(
    std::string_view wire,
    int listMax,
    int repeatThreshold,
    const char* message)
{
    const cg::packet::PacketResult<MacroCheckConfig> parsed = MacroCheckConfig::Parse(wire);
    int failures = Expect(parsed.HasValue(), message);
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().listMax == listMax, message);
    failures += Expect(parsed.Value().repeatThreshold == repeatThreshold, message);

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value().ToPacket(), MacroCheckConfig::Spec);
    failures += Expect(built.HasValue(), message);
    if (built.HasValue()) {
        failures += Expect(built.Value() == wire, message);
    }
    return failures;
}

int TestSamplesAndUnrestrictedValues()
{
    int failures = 0;
    failures += ExpectValues("tq Y O ", 60, 50, "tq active default");
    failures += ExpectValues("tq a 5 ", 10, 5, "tq client lower bound");
    failures += ExpectValues("tq 1iE 4 ", 5000, 4, "tq client normalization input");
    failures += ExpectValues("tq -1 -2 ", -1, -2, "tq negative values");
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("tq Y O", MacroCheckConfig::Spec);

    int failures = Expect(parsed.HasValue(), "tq generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value(), MacroCheckConfig::Spec);
    failures += Expect(built.HasValue(), "tq generic build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "tq Y O ", "tq generic canonical");
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 2U>& tokens)
{
    return "tq " + tokens[0] + ' ' + tokens[1] + ' ';
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        MacroCheckConfig::Parse("bad Y O "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "tq wrong code");
    failures += ExpectError(
        MacroCheckConfig::Parse("tq Y"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "tq",
        true,
        1U,
        "tq missing field");
    failures += ExpectError(
        MacroCheckConfig::Parse("tq Y O 0 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "tq",
        true,
        2U,
        "tq extra field");

    const std::array<std::string, 2U> valid = {"Y", "O"};
    for (std::size_t index = 0U; index < valid.size(); ++index) {
        std::array<std::string, 2U> invalid = valid;
        invalid[index] = "_";
        failures += ExpectError(
            MacroCheckConfig::Parse(BuildPlain(invalid)),
            cg::packet::PacketErrorCode::DecodeFailed,
            "tq",
            true,
            index,
            "tq invalid i62");

        std::array<std::string, 2U> overflow = valid;
        overflow[index] = "2lkCB2";
        failures += ExpectError(
            MacroCheckConfig::Parse(BuildPlain(overflow)),
            cg::packet::PacketErrorCode::DecodeFailed,
            "tq",
            true,
            index,
            "tq i62 overflow");
    }
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndToPacket()
        + TestSamplesAndUnrestrictedValues()
        + TestGenericRoundtrip()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " macro check config packet test(s) failed\n";
        return 1;
    }
    return 0;
}
