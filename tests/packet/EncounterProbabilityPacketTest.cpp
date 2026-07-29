#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::EncounterProbability;

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
    const EncounterProbability probability(10, 36);
    const cg::packet::Packet packet = probability.ToPacket();

    int failures = 0;
    failures += Expect(EncounterProbability::Code == "OaHi", "OaHi code");
    failures += Expect(
        EncounterProbability::Direction == cg::packet::PacketDirection::ServerToClient,
        "OaHi direction");
    failures += Expect(EncounterProbability::Fields.size() == 2U, "OaHi fields");
    failures += Expect(EncounterProbability::Fields[0].name == "minimum", "OaHi minimum name");
    failures += Expect(EncounterProbability::Fields[1].name == "maximum", "OaHi maximum name");
    failures += Expect(
        EncounterProbability::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "OaHi minimum type");
    failures += Expect(
        EncounterProbability::Fields[1].type == cg::packet::PacketFieldType::Int62,
        "OaHi maximum type");
    failures += Expect(EncounterProbability::Spec.code == EncounterProbability::Code, "OaHi Spec code");
    failures += Expect(
        EncounterProbability::Spec.direction == EncounterProbability::Direction,
        "OaHi Spec direction");
    failures += Expect(EncounterProbability::Spec.fieldCount == 2U, "OaHi Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("OaHi"), "OaHi packet code");
    failures += Expect(packet.direction == EncounterProbability::Direction, "OaHi packet direction");
    failures += Expect(packet.fields.size() == 2U, "OaHi packet fields");
    failures += ExpectField(packet, 0U, "minimum", "10", "OaHi minimum field");
    failures += ExpectField(packet, 1U, "maximum", "36", "OaHi maximum field");
    return failures;
}

int TestTypedParseAndCanonical()
{
    const cg::packet::PacketResult<EncounterProbability> parsed =
        EncounterProbability::Parse("OaHi a A ");

    int failures = Expect(parsed.HasValue(), "OaHi typed parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().minimum == 10, "OaHi minimum value");
    failures += Expect(parsed.Value().maximum == 36, "OaHi maximum value");

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value().ToPacket(), EncounterProbability::Spec);
    failures += Expect(built.HasValue(), "OaHi typed build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "OaHi a A ", "OaHi typed canonical");
    }
    return failures;
}

int TestGenericRoundtripAndUnrestrictedValues()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("OaHi a A", EncounterProbability::Spec);

    int failures = Expect(parsed.HasValue(), "OaHi generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value(), EncounterProbability::Spec);
    failures += Expect(built.HasValue(), "OaHi generic build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "OaHi a A ", "OaHi generic canonical");
    }

    const cg::packet::PacketResult<EncounterProbability> unrestricted =
        EncounterProbability::Parse("OaHi 1D -5 ");
    failures += Expect(unrestricted.HasValue(), "OaHi unrestricted parse");
    if (unrestricted.HasValue()) {
        failures += Expect(unrestricted.Value().minimum == 101, "OaHi minimum above 100");
        failures += Expect(unrestricted.Value().maximum == -5, "OaHi negative maximum");
        failures += Expect(
            unrestricted.Value().minimum > unrestricted.Value().maximum,
            "OaHi unordered range retained");
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 2U>& tokens)
{
    return "OaHi " + tokens[0] + ' ' + tokens[1] + ' ';
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        EncounterProbability::Parse("bad a A "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "OaHi wrong code");
    failures += ExpectError(
        EncounterProbability::Parse("OaHi a"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "OaHi",
        true,
        1U,
        "OaHi missing field");
    failures += ExpectError(
        EncounterProbability::Parse("OaHi a A 0 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "OaHi",
        true,
        2U,
        "OaHi extra field");

    const std::array<std::string, 2U> valid = {"a", "A"};
    for (std::size_t index = 0U; index < valid.size(); ++index) {
        std::array<std::string, 2U> invalid = valid;
        invalid[index] = "_";
        failures += ExpectError(
            EncounterProbability::Parse(BuildPlain(invalid)),
            cg::packet::PacketErrorCode::DecodeFailed,
            "OaHi",
            true,
            index,
            "OaHi invalid i62");

        std::array<std::string, 2U> overflow = valid;
        overflow[index] = "2lkCB2";
        failures += ExpectError(
            EncounterProbability::Parse(BuildPlain(overflow)),
            cg::packet::PacketErrorCode::DecodeFailed,
            "OaHi",
            true,
            index,
            "OaHi i62 overflow");
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
        std::cerr << failures << " encounter probability packet test(s) failed\n";
        return 1;
    }
    return 0;
}
