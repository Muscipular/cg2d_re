#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::FeverStatus;

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
    const FeverStatus status(62, 1);
    const cg::packet::Packet packet = status.ToPacket();

    int failures = 0;
    failures += Expect(FeverStatus::Code == "MTg", "MTg code");
    failures += Expect(
        FeverStatus::Direction == cg::packet::PacketDirection::ServerToClient,
        "MTg direction");
    failures += Expect(FeverStatus::Fields.size() == 2U, "MTg fields");
    failures += Expect(FeverStatus::Fields[0].name == "FeverHaveTime", "MTg time field name");
    failures += Expect(FeverStatus::Fields[1].name == "isFever", "MTg flag field name");
    failures += Expect(
        FeverStatus::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "MTg time field type");
    failures += Expect(
        FeverStatus::Fields[1].type == cg::packet::PacketFieldType::Int62,
        "MTg flag field type");
    failures += Expect(FeverStatus::Spec.code == FeverStatus::Code, "MTg Spec code");
    failures += Expect(FeverStatus::Spec.direction == FeverStatus::Direction, "MTg Spec direction");
    failures += Expect(FeverStatus::Spec.fieldCount == 2U, "MTg Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("MTg"), "MTg packet code");
    failures += Expect(packet.direction == FeverStatus::Direction, "MTg packet direction");
    failures += Expect(packet.fields.size() == 2U, "MTg packet fields");
    failures += ExpectField(packet, 0U, "FeverHaveTime", "62", "MTg time packet field");
    failures += ExpectField(packet, 1U, "isFever", "1", "MTg flag packet field");
    return failures;
}

int TestTypedParseAndCanonical()
{
    const cg::packet::PacketResult<FeverStatus> parsed =
        FeverStatus::Parse("MTg 10 1 ");

    int failures = Expect(parsed.HasValue(), "MTg typed parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().feverHaveTime == 62, "MTg time value");
    failures += Expect(parsed.Value().isFever == 1, "MTg flag value");

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value().ToPacket(), FeverStatus::Spec);
    failures += Expect(built.HasValue(), "MTg typed build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "MTg 10 1 ", "MTg typed canonical");
    }
    return failures;
}

int TestGenericRoundtripAndUnrestrictedValues()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("MTg 10 1", FeverStatus::Spec);

    int failures = Expect(parsed.HasValue(), "MTg generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(parsed.Value(), FeverStatus::Spec);
    failures += Expect(built.HasValue(), "MTg generic build");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "MTg 10 1 ", "MTg generic canonical");
    }

    const cg::packet::PacketResult<FeverStatus> unrestricted =
        FeverStatus::Parse("MTg -1 2 ");
    failures += Expect(unrestricted.HasValue(), "MTg unrestricted parse");
    if (unrestricted.HasValue()) {
        failures += Expect(unrestricted.Value().feverHaveTime == -1, "MTg negative time retained");
        failures += Expect(unrestricted.Value().isFever == 2, "MTg non-boolean flag retained");
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 2U>& tokens)
{
    return "MTg " + tokens[0] + ' ' + tokens[1] + ' ';
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        FeverStatus::Parse("bad 10 1 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "MTg wrong code");
    failures += ExpectError(
        FeverStatus::Parse("MTg 10"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "MTg",
        true,
        1U,
        "MTg missing field");
    failures += ExpectError(
        FeverStatus::Parse("MTg 10 1 0 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "MTg",
        true,
        2U,
        "MTg extra field");

    const std::array<std::string, 2U> valid = {"10", "1"};
    for (std::size_t index = 0U; index < valid.size(); ++index) {
        std::array<std::string, 2U> invalid = valid;
        invalid[index] = "_";
        failures += ExpectError(
            FeverStatus::Parse(BuildPlain(invalid)),
            cg::packet::PacketErrorCode::DecodeFailed,
            "MTg",
            true,
            index,
            "MTg invalid i62");

        std::array<std::string, 2U> overflow = valid;
        overflow[index] = "2lkCB2";
        failures += ExpectError(
            FeverStatus::Parse(BuildPlain(overflow)),
            cg::packet::PacketErrorCode::DecodeFailed,
            "MTg",
            true,
            index,
            "MTg i62 overflow");
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
        std::cerr << failures << " fever status packet test(s) failed\n";
        return 1;
    }
    return 0;
}
