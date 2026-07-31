#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <array>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::PetLevelUpPoints;

constexpr std::array<std::string_view, 2U> FieldNames = {{
    "petSlot",
    "levelUpPoints",
}};

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

int ExpectValues(
    const PetLevelUpPoints& value,
    const std::array<int, 2U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(value.petSlot == expected[0], message);
    failures += Expect(value.levelUpPoints == expected[1], message);
    return failures;
}

int ExpectPacket(
    const cg::packet::Packet& packet,
    const std::array<int, 2U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode("sA"), message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ServerToClient,
        message);
    failures += Expect(packet.fields.size() == FieldNames.size(), message);
    if (packet.fields.size() != FieldNames.size()) {
        return failures;
    }

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(packet.fields[index].name == FieldNames[index], message);
        failures += Expect(
            packet.fields[index].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(
            packet.fields[index].value == std::to_string(expected[index]),
            message);
    }
    return failures;
}

std::string BuildWireWithField(std::size_t fieldIndex, std::string_view token)
{
    std::string wire = "sA";
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        wire.push_back(' ');
        wire.append(index == fieldIndex ? token : std::string_view("0"));
    }
    return wire;
}

int TestSpecAndRoundtrip()
{
    constexpr std::array<int, 2U> Expected = {{2, 4}};
    const PetLevelUpPoints value(Expected[0], Expected[1]);
    const cg::packet::Packet packet = value.ToPacket();

    int failures = 0;
    failures += Expect(PetLevelUpPoints::Code == "sA", "sA code");
    failures += Expect(
        PetLevelUpPoints::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "sA direction");
    failures += Expect(
        PetLevelUpPoints::Fields.size() == FieldNames.size(),
        "sA field count");
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(
            PetLevelUpPoints::Fields[index].name == FieldNames[index],
            "sA field name order");
        failures += Expect(
            PetLevelUpPoints::Fields[index].type ==
                cg::packet::PacketFieldType::Int62,
            "sA field type order");
    }
    failures += Expect(
        PetLevelUpPoints::Spec.code == PetLevelUpPoints::Code,
        "sA Spec code");
    failures += Expect(
        PetLevelUpPoints::Spec.direction == PetLevelUpPoints::Direction,
        "sA Spec direction");
    failures += Expect(
        PetLevelUpPoints::Spec.fieldCount == FieldNames.size(),
        "sA Spec field count");
    failures += ExpectValues(value, Expected, "sA members");
    failures += ExpectPacket(packet, Expected, "sA ToPacket");

    const cg::packet::PacketResult<PetLevelUpPoints> converted =
        PetLevelUpPoints::FromPacket(packet);
    failures += Expect(converted.HasValue(), "sA FromPacket valid");
    if (converted.HasValue()) {
        failures += ExpectValues(converted.Value(), Expected, "sA FromPacket values");
    }

    const cg::packet::PacketResult<PetLevelUpPoints> typed =
        PetLevelUpPoints::Parse("sA 2 4");
    failures += Expect(typed.HasValue(), "sA typed parse");
    if (typed.HasValue()) {
        failures += ExpectValues(typed.Value(), Expected, "sA typed values");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("sA 2 4", PetLevelUpPoints::Spec);
    failures += Expect(generic.HasValue(), "sA generic parse");
    if (generic.HasValue()) {
        failures += ExpectPacket(generic.Value(), Expected, "sA generic packet");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(generic.Value(), PetLevelUpPoints::Spec);
        failures += Expect(rebuilt.HasValue(), "sA generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(rebuilt.Value() == "sA 2 4 ", "sA canonical");
        }
    }
    return failures;
}

int TestSignedValues()
{
    struct Case
    {
        std::array<int, 2U> values;
        std::string_view canonical;
    };

    constexpr Case Cases[] = {
        {{{-1, -2}}, "sA -1 -2 "},
        {{{std::numeric_limits<int>::min(),
           std::numeric_limits<int>::max()}},
         "sA -2lkCB2 2lkCB1 "},
        {{{std::numeric_limits<int>::max(),
           std::numeric_limits<int>::min()}},
         "sA 2lkCB1 -2lkCB2 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const PetLevelUpPoints value(testCase.values[0], testCase.values[1]);
        failures += ExpectPacket(
            value.ToPacket(),
            testCase.values,
            "sA signed packet");

        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(value.ToPacket(), PetLevelUpPoints::Spec);
        failures += Expect(built.HasValue(), "sA signed build");
        if (built.HasValue()) {
            failures += Expect(
                built.Value() == testCase.canonical,
                "sA signed canonical");
        }

        const cg::packet::PacketResult<PetLevelUpPoints> parsed =
            PetLevelUpPoints::Parse(testCase.canonical);
        failures += Expect(parsed.HasValue(), "sA signed parse");
        if (parsed.HasValue()) {
            failures += ExpectValues(
                parsed.Value(),
                testCase.values,
                "sA signed values");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        PetLevelUpPoints::Parse("bad 2 4"),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "sA wrong code");
    failures += ExpectError(
        PetLevelUpPoints::Parse("sA 2"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "sA",
        true,
        1U,
        "sA missing field");
    failures += ExpectError(
        PetLevelUpPoints::Parse("sA 2 4 6"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "sA",
        true,
        2U,
        "sA extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += ExpectError(
            PetLevelUpPoints::Parse(BuildWireWithField(index, "_")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "sA",
            true,
            index,
            "sA invalid i62 field");
        failures += ExpectError(
            PetLevelUpPoints::Parse(BuildWireWithField(index, "2lkCB2")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "sA",
            true,
            index,
            "sA positive i62 overflow field");
        failures += ExpectError(
            PetLevelUpPoints::Parse(BuildWireWithField(index, "-2lkCB3")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "sA",
            true,
            index,
            "sA negative i62 overflow field");
    }

    const std::string pollutedCode = "sA\x17" "x";
    failures += ExpectError(
        PetLevelUpPoints::Parse(pollutedCode + " 2 4"),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "sA polluted code");

    const std::string pollutedField = "sA 2\x17" "x 4";
    failures += ExpectError(
        PetLevelUpPoints::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "sA",
        true,
        2U,
        "sA polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = PetLevelUpPoints(2, 4).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        PetLevelUpPoints::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "sA FromPacket wrong code");

    cg::packet::Packet wrongDirection = PetLevelUpPoints(2, 4).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        PetLevelUpPoints::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "sA",
        false,
        0U,
        "sA FromPacket wrong direction");

    cg::packet::Packet missingField = PetLevelUpPoints(2, 4).ToPacket();
    missingField.fields.pop_back();
    failures += ExpectError(
        PetLevelUpPoints::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "sA",
        true,
        1U,
        "sA FromPacket missing field");

    cg::packet::Packet extraField = PetLevelUpPoints(2, 4).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "0",
    });
    failures += ExpectError(
        PetLevelUpPoints::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "sA",
        true,
        2U,
        "sA FromPacket extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        cg::packet::Packet wrongName = PetLevelUpPoints(2, 4).ToPacket();
        wrongName.fields[index].name = "wrong";
        failures += ExpectError(
            PetLevelUpPoints::FromPacket(wrongName),
            cg::packet::PacketErrorCode::DecodeFailed,
            "sA",
            true,
            index,
            "sA FromPacket wrong name");

        cg::packet::Packet wrongType = PetLevelUpPoints(2, 4).ToPacket();
        wrongType.fields[index].type = cg::packet::PacketFieldType::EscapedString;
        failures += ExpectError(
            PetLevelUpPoints::FromPacket(wrongType),
            cg::packet::PacketErrorCode::InvalidFieldType,
            "sA",
            true,
            index,
            "sA FromPacket wrong type");

        cg::packet::Packet invalidValue = PetLevelUpPoints(2, 4).ToPacket();
        invalidValue.fields[index].value = "not-an-int";
        failures += ExpectError(
            PetLevelUpPoints::FromPacket(invalidValue),
            cg::packet::PacketErrorCode::DecodeFailed,
            "sA",
            true,
            index,
            "sA FromPacket invalid decimal");

        cg::packet::Packet positiveOverflow = PetLevelUpPoints(2, 4).ToPacket();
        positiveOverflow.fields[index].value = "2147483648";
        failures += ExpectError(
            PetLevelUpPoints::FromPacket(positiveOverflow),
            cg::packet::PacketErrorCode::DecodeFailed,
            "sA",
            true,
            index,
            "sA FromPacket positive decimal overflow");

        cg::packet::Packet negativeOverflow = PetLevelUpPoints(2, 4).ToPacket();
        negativeOverflow.fields[index].value = "-2147483649";
        failures += ExpectError(
            PetLevelUpPoints::FromPacket(negativeOverflow),
            cg::packet::PacketErrorCode::DecodeFailed,
            "sA",
            true,
            index,
            "sA FromPacket negative decimal overflow");
    }
    return failures;
}

std::string ReadFixture(const char* path)
{
    std::ifstream file(path, std::ios::binary);
    if (!file.is_open()) {
        return {};
    }
    return std::string(
        std::istreambuf_iterator<char>(file),
        std::istreambuf_iterator<char>());
}

int TestFixturePhysical(const char* path)
{
    constexpr std::string_view Expected =
        "pkt_fixture=1\n"
        "code=sA\n"
        "direction=server_to_client\n"
        "sample=pet-slot-two-points-four\n"
        "plain_hex=73 41 20 32 20 34\n"
        "canonical_hex=73 41 20 32 20 34 20\n"
        "end_sample\n";

    const std::string fixture = ReadFixture(path);
    int failures = 0;
    failures += Expect(fixture == Expected, "sA fixture exact bytes");

    std::size_t lineFeeds = 0U;
    bool asciiOnly = true;
    bool physicalBytesClean = true;
    for (const unsigned char byte : fixture) {
        lineFeeds += byte == 0x0AU ? 1U : 0U;
        asciiOnly = asciiOnly && byte <= 0x7FU;
        physicalBytesClean =
            physicalBytesClean && byte != 0x00U && byte != 0x0DU && byte != 0x17U;
    }
    failures += Expect(asciiOnly, "sA fixture ASCII");
    failures += Expect(physicalBytesClean, "sA fixture physical bytes");
    failures += Expect(lineFeeds == 7U, "sA fixture LF count");
    return failures;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "usage: cg-packet-pet-level-up-points-test <sA-fixture>\n";
        return 1;
    }

    const int failures =
        TestSpecAndRoundtrip()
        + TestSignedValues()
        + TestParseErrors()
        + TestFromPacketErrors()
        + TestFixturePhysical(argv[1]);
    if (failures != 0) {
        std::cerr << failures << " pet level-up points packet test(s) failed\n";
        return 1;
    }
    return 0;
}
