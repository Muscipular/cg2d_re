#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::PaletteChange;

constexpr std::array<std::string_view, 2U> FieldNames = {{
    "palnumber",
    "frameCnt",
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
    const PaletteChange& value,
    const std::array<int, 2U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(value.palNumber == expected[0], message);
    failures += Expect(value.frameCount == expected[1], message);
    return failures;
}

int ExpectPacket(
    const cg::packet::Packet& packet,
    const std::array<std::string, 2U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode("bJg"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ServerToClient,
        message);
    failures += Expect(packet.fields.size() == FieldNames.size(), message);
    if (packet.fields.size() != FieldNames.size()) {
        return failures;
    }

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(
            packet.fields[index].name == FieldNames[index],
            message);
        failures += Expect(
            packet.fields[index].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(
            packet.fields[index].value == expected[index],
            message);
    }
    return failures;
}

std::string BuildWireWithField(std::size_t fieldIndex, std::string_view token)
{
    std::string wire = "bJg";
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        wire.push_back(' ');
        wire.append(index == fieldIndex ? token : std::string_view("0"));
    }
    return wire;
}

int TestSpecAndRoundtrip()
{
    constexpr std::array<int, 2U> Expected = {{13, 180}};
    const PaletteChange value(Expected[0], Expected[1]);
    const cg::packet::Packet packet = value.ToPacket();

    int failures = 0;
    failures += Expect(PaletteChange::Code == "bJg", "bJg code");
    failures += Expect(
        PaletteChange::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "bJg direction");
    failures += Expect(
        PaletteChange::Fields.size() == FieldNames.size(),
        "bJg field count");
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(
            PaletteChange::Fields[index].name == FieldNames[index],
            "bJg field name order");
        failures += Expect(
            PaletteChange::Fields[index].type ==
                cg::packet::PacketFieldType::Int62,
            "bJg field type order");
    }
    failures += Expect(
        PaletteChange::Spec.code == PaletteChange::Code,
        "bJg Spec code");
    failures += Expect(
        PaletteChange::Spec.direction == PaletteChange::Direction,
        "bJg Spec direction");
    failures += Expect(
        PaletteChange::Spec.fieldCount == FieldNames.size(),
        "bJg Spec field count");
    failures += ExpectValues(value, Expected, "bJg members");
    failures += ExpectPacket(
        packet,
        {{"13", "180"}},
        "bJg ToPacket");

    const cg::packet::PacketResult<PaletteChange> converted =
        PaletteChange::FromPacket(packet);
    failures += Expect(converted.HasValue(), "bJg FromPacket valid");
    if (converted.HasValue()) {
        failures += ExpectValues(
            converted.Value(),
            Expected,
            "bJg FromPacket values");
    }

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(packet, PaletteChange::Spec);
    failures += Expect(built.HasValue(), "bJg build");
    if (built.HasValue()) {
        failures += Expect(
            built.Value() == "bJg d 2U ",
            "bJg canonical");
    }

    const cg::packet::PacketResult<PaletteChange> typed =
        PaletteChange::Parse("bJg d 2U");
    failures += Expect(typed.HasValue(), "bJg typed parse");
    if (typed.HasValue()) {
        failures += ExpectValues(
            typed.Value(),
            Expected,
            "bJg typed values");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("bJg d 2U", PaletteChange::Spec);
    failures += Expect(generic.HasValue(), "bJg generic parse");
    if (generic.HasValue()) {
        failures += ExpectPacket(
            generic.Value(),
            {{"13", "180"}},
            "bJg generic packet");
        const cg::packet::PacketResult<std::string> genericBuilt =
            cg::packet::BuildPacket(generic.Value(), PaletteChange::Spec);
        failures += Expect(genericBuilt.HasValue(), "bJg generic build");
        if (genericBuilt.HasValue()) {
            failures += Expect(
                genericBuilt.Value() == "bJg d 2U ",
                "bJg generic canonical");
        }
    }
    return failures;
}

int TestSignedAndBusinessBoundaryValues()
{
    struct Case
    {
        std::array<int, 2U> values;
        std::string_view canonical;
    };

    // 调色板范围与非正帧数修正仅属业务层，封包协议保持 wire 原值。
    constexpr Case Cases[] = {
        {{{38, 1}}, "bJg C 1 "},
        {{{39, 0}}, "bJg D 0 "},
        {{{-1, -2}}, "bJg -1 -2 "},
        {{{std::numeric_limits<int>::min(),
           std::numeric_limits<int>::max()}},
         "bJg -2lkCB2 2lkCB1 "},
        {{{std::numeric_limits<int>::max(),
           std::numeric_limits<int>::min()}},
         "bJg 2lkCB1 -2lkCB2 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const PaletteChange value(
            testCase.values[0],
            testCase.values[1]);
        failures += ExpectPacket(
            value.ToPacket(),
            {{
                std::to_string(testCase.values[0]),
                std::to_string(testCase.values[1]),
            }},
            "bJg signed packet");

        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(value.ToPacket(), PaletteChange::Spec);
        failures += Expect(built.HasValue(), "bJg signed build");
        if (built.HasValue()) {
            failures += Expect(
                built.Value() == testCase.canonical,
                "bJg signed canonical");
        }

        const cg::packet::PacketResult<PaletteChange> parsed =
            PaletteChange::Parse(testCase.canonical);
        failures += Expect(parsed.HasValue(), "bJg signed parse");
        if (parsed.HasValue()) {
            failures += ExpectValues(
                parsed.Value(),
                testCase.values,
                "bJg signed values");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        PaletteChange::Parse("bad 0 0 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "bJg wrong code");

    constexpr std::array<std::string_view, 2U> MissingWires = {{
        "bJg",
        "bJg 0",
    }};
    for (std::size_t index = 0U; index < MissingWires.size(); ++index) {
        failures += ExpectError(
            PaletteChange::Parse(MissingWires[index]),
            cg::packet::PacketErrorCode::FieldCountMismatch,
            "bJg",
            true,
            index,
            "bJg progressively missing field");
    }

    failures += ExpectError(
        PaletteChange::Parse("bJg 0 0 0 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "bJg",
        true,
        2U,
        "bJg extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += ExpectError(
            PaletteChange::Parse(BuildWireWithField(index, "_")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "bJg",
            true,
            index,
            "bJg invalid i62 field");
        failures += ExpectError(
            PaletteChange::Parse(BuildWireWithField(index, "2lkCB2")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "bJg",
            true,
            index,
            "bJg positive i62 overflow field");
        failures += ExpectError(
            PaletteChange::Parse(BuildWireWithField(index, "-2lkCB3")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "bJg",
            true,
            index,
            "bJg negative i62 overflow field");
    }

    const std::string pollutedCode = "bJg\x17" "x";
    failures += ExpectError(
        PaletteChange::Parse(pollutedCode + " 0 0"),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "bJg polluted code");

    const std::string pollutedField = "bJg 0\x17" "x 0";
    failures += ExpectError(
        PaletteChange::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "bJg",
        true,
        2U,
        "bJg polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = PaletteChange(0, 0).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        PaletteChange::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "bJg FromPacket wrong code");

    cg::packet::Packet wrongDirection = PaletteChange(0, 0).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        PaletteChange::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "bJg",
        false,
        0U,
        "bJg FromPacket wrong direction");

    cg::packet::Packet missingField(
        cg::packet::PacketCode("bJg"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        PaletteChange::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "bJg",
        true,
        0U,
        "bJg FromPacket missing field");

    cg::packet::Packet extraField = PaletteChange(0, 0).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "0",
    });
    failures += ExpectError(
        PaletteChange::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "bJg",
        true,
        2U,
        "bJg FromPacket extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        cg::packet::Packet wrongName = PaletteChange(0, 0).ToPacket();
        wrongName.fields[index].name = "wrong";
        failures += ExpectError(
            PaletteChange::FromPacket(wrongName),
            cg::packet::PacketErrorCode::DecodeFailed,
            "bJg",
            true,
            index,
            "bJg FromPacket wrong name");

        cg::packet::Packet wrongType = PaletteChange(0, 0).ToPacket();
        wrongType.fields[index].type =
            cg::packet::PacketFieldType::EscapedString;
        failures += ExpectError(
            PaletteChange::FromPacket(wrongType),
            cg::packet::PacketErrorCode::InvalidFieldType,
            "bJg",
            true,
            index,
            "bJg FromPacket wrong type");

        cg::packet::Packet invalidValue = PaletteChange(0, 0).ToPacket();
        invalidValue.fields[index].value = "not-an-int";
        failures += ExpectError(
            PaletteChange::FromPacket(invalidValue),
            cg::packet::PacketErrorCode::DecodeFailed,
            "bJg",
            true,
            index,
            "bJg FromPacket invalid decimal");

        cg::packet::Packet positiveOverflow =
            PaletteChange(0, 0).ToPacket();
        positiveOverflow.fields[index].value = "2147483648";
        failures += ExpectError(
            PaletteChange::FromPacket(positiveOverflow),
            cg::packet::PacketErrorCode::DecodeFailed,
            "bJg",
            true,
            index,
            "bJg FromPacket positive decimal overflow");

        cg::packet::Packet negativeOverflow =
            PaletteChange(0, 0).ToPacket();
        negativeOverflow.fields[index].value = "-2147483649";
        failures += ExpectError(
            PaletteChange::FromPacket(negativeOverflow),
            cg::packet::PacketErrorCode::DecodeFailed,
            "bJg",
            true,
            index,
            "bJg FromPacket negative decimal overflow");
    }
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndRoundtrip()
        + TestSignedAndBusinessBoundaryValues()
        + TestParseErrors()
        + TestFromPacketErrors();
    if (failures != 0) {
        std::cerr << failures
                  << " palette change packet test(s) failed\n";
        return 1;
    }
    return 0;
}
