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

using cg::packet::receive::PetSkillUseResult;

constexpr std::array<std::string_view, 4U> FieldNames = {{
    "result",
    "havepetindex",
    "havepetskill",
    "toindex",
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
    const PetSkillUseResult& value,
    const std::array<int, 4U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(value.result == expected[0], message);
    failures += Expect(value.havePetIndex == expected[1], message);
    failures += Expect(value.havePetSkill == expected[2], message);
    failures += Expect(value.toIndex == expected[3], message);
    return failures;
}

int ExpectPacket(
    const cg::packet::Packet& packet,
    const std::array<int, 4U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode("BHe"), message);
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
    std::string wire = "BHe";
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        wire.push_back(' ');
        wire.append(index == fieldIndex ? token : std::string_view("0"));
    }
    return wire;
}

int TestSpecAndRoundtrip()
{
    constexpr std::array<int, 4U> Expected = {{1, 0, 2, 3}};
    const PetSkillUseResult value(
        Expected[0], Expected[1], Expected[2], Expected[3]);
    const cg::packet::Packet packet = value.ToPacket();

    int failures = 0;
    failures += Expect(PetSkillUseResult::Code == "BHe", "BHe code");
    failures += Expect(
        PetSkillUseResult::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "BHe direction");
    failures += Expect(
        PetSkillUseResult::Fields.size() == FieldNames.size(),
        "BHe field count");
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(
            PetSkillUseResult::Fields[index].name == FieldNames[index],
            "BHe field name order");
        failures += Expect(
            PetSkillUseResult::Fields[index].type ==
                cg::packet::PacketFieldType::Int62,
            "BHe field type order");
    }
    failures += Expect(
        PetSkillUseResult::Spec.code == PetSkillUseResult::Code,
        "BHe Spec code");
    failures += Expect(
        PetSkillUseResult::Spec.direction == PetSkillUseResult::Direction,
        "BHe Spec direction");
    failures += Expect(
        PetSkillUseResult::Spec.fieldCount == FieldNames.size(),
        "BHe Spec field count");
    failures += ExpectValues(value, Expected, "BHe members");
    failures += ExpectPacket(packet, Expected, "BHe ToPacket");

    const cg::packet::PacketResult<PetSkillUseResult> converted =
        PetSkillUseResult::FromPacket(packet);
    failures += Expect(converted.HasValue(), "BHe FromPacket valid");
    if (converted.HasValue()) {
        failures += ExpectValues(converted.Value(), Expected, "BHe FromPacket values");
    }

    const cg::packet::PacketResult<PetSkillUseResult> typed =
        PetSkillUseResult::Parse("BHe 1 0 2 3");
    failures += Expect(typed.HasValue(), "BHe typed parse");
    if (typed.HasValue()) {
        failures += ExpectValues(typed.Value(), Expected, "BHe typed values");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("BHe 1 0 2 3", PetSkillUseResult::Spec);
    failures += Expect(generic.HasValue(), "BHe generic parse");
    if (generic.HasValue()) {
        failures += ExpectPacket(generic.Value(), Expected, "BHe generic packet");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(generic.Value(), PetSkillUseResult::Spec);
        failures += Expect(rebuilt.HasValue(), "BHe generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(
                rebuilt.Value() == "BHe 1 0 2 3 ",
                "BHe canonical");
        }
    }
    return failures;
}

int TestSignedValues()
{
    struct Case
    {
        std::array<int, 4U> values;
        std::string_view canonical;
    };

    constexpr Case Cases[] = {
        {{{-1, -2, -3, -4}}, "BHe -1 -2 -3 -4 "},
        {{{std::numeric_limits<int>::min(),
           std::numeric_limits<int>::max(),
           std::numeric_limits<int>::min(),
           std::numeric_limits<int>::max()}},
         "BHe -2lkCB2 2lkCB1 -2lkCB2 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const PetSkillUseResult value(
            testCase.values[0],
            testCase.values[1],
            testCase.values[2],
            testCase.values[3]);
        failures += ExpectPacket(
            value.ToPacket(),
            testCase.values,
            "BHe signed packet");

        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(value.ToPacket(), PetSkillUseResult::Spec);
        failures += Expect(built.HasValue(), "BHe signed build");
        if (built.HasValue()) {
            failures += Expect(
                built.Value() == testCase.canonical,
                "BHe signed canonical");
        }

        const cg::packet::PacketResult<PetSkillUseResult> parsed =
            PetSkillUseResult::Parse(testCase.canonical);
        failures += Expect(parsed.HasValue(), "BHe signed parse");
        if (parsed.HasValue()) {
            failures += ExpectValues(
                parsed.Value(),
                testCase.values,
                "BHe signed values");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        PetSkillUseResult::Parse("bad 1 0 2 3"),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "BHe wrong code");
    failures += ExpectError(
        PetSkillUseResult::Parse("BHe 1 0 2"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "BHe",
        true,
        3U,
        "BHe missing field");
    failures += ExpectError(
        PetSkillUseResult::Parse("BHe 1 0 2 3 4"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "BHe",
        true,
        4U,
        "BHe extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += ExpectError(
            PetSkillUseResult::Parse(BuildWireWithField(index, "_")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "BHe",
            true,
            index,
            "BHe invalid i62 field");
        failures += ExpectError(
            PetSkillUseResult::Parse(BuildWireWithField(index, "2lkCB2")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "BHe",
            true,
            index,
            "BHe positive i62 overflow field");
        failures += ExpectError(
            PetSkillUseResult::Parse(BuildWireWithField(index, "-2lkCB3")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "BHe",
            true,
            index,
            "BHe negative i62 overflow field");
    }

    const std::string pollutedCode = "BHe\x17" "x";
    failures += ExpectError(
        PetSkillUseResult::Parse(pollutedCode + " 1 0 2 3"),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "BHe polluted code");

    const std::string pollutedField = "BHe 1\x17" "x 0 2 3";
    failures += ExpectError(
        PetSkillUseResult::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "BHe",
        true,
        4U,
        "BHe polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = PetSkillUseResult(1, 0, 2, 3).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        PetSkillUseResult::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "BHe FromPacket wrong code");

    cg::packet::Packet wrongDirection = PetSkillUseResult(1, 0, 2, 3).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        PetSkillUseResult::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "BHe",
        false,
        0U,
        "BHe FromPacket wrong direction");

    cg::packet::Packet missingField = PetSkillUseResult(1, 0, 2, 3).ToPacket();
    missingField.fields.pop_back();
    failures += ExpectError(
        PetSkillUseResult::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "BHe",
        true,
        3U,
        "BHe FromPacket missing field");

    cg::packet::Packet extraField = PetSkillUseResult(1, 0, 2, 3).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "0",
    });
    failures += ExpectError(
        PetSkillUseResult::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "BHe",
        true,
        4U,
        "BHe FromPacket extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        cg::packet::Packet wrongName = PetSkillUseResult(1, 0, 2, 3).ToPacket();
        wrongName.fields[index].name = "wrong";
        failures += ExpectError(
            PetSkillUseResult::FromPacket(wrongName),
            cg::packet::PacketErrorCode::DecodeFailed,
            "BHe",
            true,
            index,
            "BHe FromPacket wrong name");

        cg::packet::Packet wrongType = PetSkillUseResult(1, 0, 2, 3).ToPacket();
        wrongType.fields[index].type = cg::packet::PacketFieldType::EscapedString;
        failures += ExpectError(
            PetSkillUseResult::FromPacket(wrongType),
            cg::packet::PacketErrorCode::InvalidFieldType,
            "BHe",
            true,
            index,
            "BHe FromPacket wrong type");

        cg::packet::Packet invalidValue = PetSkillUseResult(1, 0, 2, 3).ToPacket();
        invalidValue.fields[index].value = "not-an-int";
        failures += ExpectError(
            PetSkillUseResult::FromPacket(invalidValue),
            cg::packet::PacketErrorCode::DecodeFailed,
            "BHe",
            true,
            index,
            "BHe FromPacket invalid decimal");

        cg::packet::Packet positiveOverflow =
            PetSkillUseResult(1, 0, 2, 3).ToPacket();
        positiveOverflow.fields[index].value = "2147483648";
        failures += ExpectError(
            PetSkillUseResult::FromPacket(positiveOverflow),
            cg::packet::PacketErrorCode::DecodeFailed,
            "BHe",
            true,
            index,
            "BHe FromPacket positive decimal overflow");

        cg::packet::Packet negativeOverflow =
            PetSkillUseResult(1, 0, 2, 3).ToPacket();
        negativeOverflow.fields[index].value = "-2147483649";
        failures += ExpectError(
            PetSkillUseResult::FromPacket(negativeOverflow),
            cg::packet::PacketErrorCode::DecodeFailed,
            "BHe",
            true,
            index,
            "BHe FromPacket negative decimal overflow");
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
        "code=BHe\n"
        "direction=server_to_client\n"
        "sample=pet-skill-use-result\n"
        "plain_hex=42 48 65 20 31 20 30 20 32 20 33\n"
        "canonical_hex=42 48 65 20 31 20 30 20 32 20 33 20\n"
        "end_sample\n";

    const std::string fixture = ReadFixture(path);
    int failures = 0;
    failures += Expect(fixture == Expected, "BHe fixture exact bytes");

    std::size_t lineFeeds = 0U;
    bool asciiOnly = true;
    bool physicalBytesClean = true;
    for (const unsigned char byte : fixture) {
        lineFeeds += byte == 0x0AU ? 1U : 0U;
        asciiOnly = asciiOnly && byte <= 0x7FU;
        physicalBytesClean =
            physicalBytesClean && byte != 0x00U && byte != 0x0DU && byte != 0x17U;
    }
    failures += Expect(asciiOnly, "BHe fixture ASCII");
    failures += Expect(physicalBytesClean, "BHe fixture physical bytes");
    failures += Expect(lineFeeds == 7U, "BHe fixture LF count");
    return failures;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr
            << "usage: cg-packet-pet-skill-use-result-test <BHe-fixture>\n";
        return 1;
    }

    const int failures =
        TestSpecAndRoundtrip()
        + TestSignedValues()
        + TestParseErrors()
        + TestFromPacketErrors()
        + TestFixturePhysical(argv[1]);
    if (failures != 0) {
        std::cerr << failures
                  << " pet skill use result packet test(s) failed\n";
        return 1;
    }
    return 0;
}
