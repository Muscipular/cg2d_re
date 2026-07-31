#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::PetItemSwap;

constexpr std::array<std::string_view, 4U> FieldNames = {{
    "fromChar",
    "fromSlot",
    "toChar",
    "toSlot",
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
    const PetItemSwap& value,
    const std::array<int, 4U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(value.fromChar == expected[0], message);
    failures += Expect(value.fromSlot == expected[1], message);
    failures += Expect(value.toChar == expected[2], message);
    failures += Expect(value.toSlot == expected[3], message);
    return failures;
}

int ExpectPacket(
    const cg::packet::Packet& packet,
    const std::array<std::string, 4U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode("kiG"),
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
    std::string wire = "kiG";
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        wire.push_back(' ');
        wire.append(index == fieldIndex ? token : std::string_view("0"));
    }
    return wire;
}

int TestSpecAndRoundtrip()
{
    constexpr std::array<int, 4U> Expected = {{0, 27, 2, 5}};
    const PetItemSwap value(
        Expected[0],
        Expected[1],
        Expected[2],
        Expected[3]);
    const cg::packet::Packet packet = value.ToPacket();

    int failures = 0;
    failures += Expect(PetItemSwap::Code == "kiG", "kiG code");
    failures += Expect(
        PetItemSwap::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "kiG direction");
    failures += Expect(
        PetItemSwap::Fields.size() == FieldNames.size(),
        "kiG field count");
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(
            PetItemSwap::Fields[index].name == FieldNames[index],
            "kiG field name order");
        failures += Expect(
            PetItemSwap::Fields[index].type ==
                cg::packet::PacketFieldType::Int62,
            "kiG field type order");
    }
    failures += Expect(
        PetItemSwap::Spec.code == PetItemSwap::Code,
        "kiG Spec code");
    failures += Expect(
        PetItemSwap::Spec.direction == PetItemSwap::Direction,
        "kiG Spec direction");
    failures += Expect(
        PetItemSwap::Spec.fieldCount == FieldNames.size(),
        "kiG Spec field count");
    failures += ExpectValues(value, Expected, "kiG members");
    failures += ExpectPacket(
        packet,
        {{"0", "27", "2", "5"}},
        "kiG ToPacket");

    const cg::packet::PacketResult<PetItemSwap> converted =
        PetItemSwap::FromPacket(packet);
    failures += Expect(converted.HasValue(), "kiG FromPacket valid");
    if (converted.HasValue()) {
        failures += ExpectValues(
            converted.Value(),
            Expected,
            "kiG FromPacket values");
    }

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(packet, PetItemSwap::Spec);
    failures += Expect(built.HasValue(), "kiG build");
    if (built.HasValue()) {
        failures += Expect(
            built.Value() == "kiG 0 r 2 5 ",
            "kiG canonical");
    }

    const cg::packet::PacketResult<PetItemSwap> typed =
        PetItemSwap::Parse("kiG 0 r 2 5");
    failures += Expect(typed.HasValue(), "kiG typed parse");
    if (typed.HasValue()) {
        failures += ExpectValues(
            typed.Value(),
            Expected,
            "kiG typed values");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("kiG 0 r 2 5", PetItemSwap::Spec);
    failures += Expect(generic.HasValue(), "kiG generic parse");
    if (generic.HasValue()) {
        failures += ExpectPacket(
            generic.Value(),
            {{"0", "27", "2", "5"}},
            "kiG generic packet");
        const cg::packet::PacketResult<std::string> genericBuilt =
            cg::packet::BuildPacket(generic.Value(), PetItemSwap::Spec);
        failures += Expect(genericBuilt.HasValue(), "kiG generic build");
        if (genericBuilt.HasValue()) {
            failures += Expect(
                genericBuilt.Value() == "kiG 0 r 2 5 ",
                "kiG generic canonical");
        }
    }
    return failures;
}

int TestSignedAndBusinessBoundaryValues()
{
    struct Case
    {
        std::array<int, 4U> values;
        std::string_view canonical;
    };

    // 人物/宠物和槽位边界仅为样本；越界、负数及 int 边界仍须 wire 保真。
    constexpr Case Cases[] = {
        {{{0, 27, 1, 0}}, "kiG 0 r 1 0 "},
        {{{5, 5, 0, 27}}, "kiG 5 5 0 r "},
        {{{6, 28, -1, -2}}, "kiG 6 s -1 -2 "},
        {{{-7, -29, 8, 100}}, "kiG -7 -t 8 1C "},
        {{{std::numeric_limits<int>::min(),
           std::numeric_limits<int>::max(),
           std::numeric_limits<int>::min(),
           std::numeric_limits<int>::max()}},
         "kiG -2lkCB2 2lkCB1 -2lkCB2 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const PetItemSwap value(
            testCase.values[0],
            testCase.values[1],
            testCase.values[2],
            testCase.values[3]);
        failures += ExpectPacket(
            value.ToPacket(),
            {{
                std::to_string(testCase.values[0]),
                std::to_string(testCase.values[1]),
                std::to_string(testCase.values[2]),
                std::to_string(testCase.values[3]),
            }},
            "kiG signed packet");

        const cg::packet::PacketResult<std::string> encoded =
            cg::packet::BuildPacket(value.ToPacket(), PetItemSwap::Spec);
        failures += Expect(encoded.HasValue(), "kiG signed build");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.canonical,
                "kiG signed canonical");
        }

        const cg::packet::PacketResult<PetItemSwap> parsed =
            PetItemSwap::Parse(testCase.canonical);
        failures += Expect(parsed.HasValue(), "kiG signed parse");
        if (parsed.HasValue()) {
            failures += ExpectValues(
                parsed.Value(),
                testCase.values,
                "kiG signed values");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        PetItemSwap::Parse("bad 0 0 0 0 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "kiG wrong code");

    constexpr std::array<std::string_view, 4U> MissingWires = {{
        "kiG",
        "kiG 0",
        "kiG 0 0",
        "kiG 0 0 0",
    }};
    for (std::size_t index = 0U; index < MissingWires.size(); ++index) {
        failures += ExpectError(
            PetItemSwap::Parse(MissingWires[index]),
            cg::packet::PacketErrorCode::FieldCountMismatch,
            "kiG",
            true,
            index,
            "kiG progressively missing field");
    }

    failures += ExpectError(
        PetItemSwap::Parse("kiG 0 0 0 0 0 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "kiG",
        true,
        4U,
        "kiG extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += ExpectError(
            PetItemSwap::Parse(BuildWireWithField(index, "_")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "kiG",
            true,
            index,
            "kiG invalid i62 field");
        failures += ExpectError(
            PetItemSwap::Parse(BuildWireWithField(index, "2lkCB2")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "kiG",
            true,
            index,
            "kiG positive i62 overflow field");
        failures += ExpectError(
            PetItemSwap::Parse(BuildWireWithField(index, "-2lkCB3")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "kiG",
            true,
            index,
            "kiG negative i62 overflow field");
    }

    const std::string pollutedCode = "kiG\x17" "x";
    const std::string pollutedWire = pollutedCode + " 0 0 0 0";
    failures += ExpectError(
        PetItemSwap::Parse(pollutedWire),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "kiG polluted code");

    const std::string pollutedField = "kiG 0 0\x17" "x 0 0";
    failures += ExpectError(
        PetItemSwap::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "kiG",
        true,
        4U,
        "kiG polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = PetItemSwap(0, 0, 0, 0).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        PetItemSwap::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "kiG FromPacket wrong code");

    cg::packet::Packet wrongDirection = PetItemSwap(0, 0, 0, 0).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        PetItemSwap::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "kiG",
        false,
        0U,
        "kiG FromPacket wrong direction");

    cg::packet::Packet missingField(
        cg::packet::PacketCode("kiG"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        PetItemSwap::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "kiG",
        true,
        0U,
        "kiG FromPacket missing field");

    cg::packet::Packet extraField = PetItemSwap(0, 0, 0, 0).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "0",
    });
    failures += ExpectError(
        PetItemSwap::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "kiG",
        true,
        4U,
        "kiG FromPacket extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        cg::packet::Packet wrongName = PetItemSwap(0, 0, 0, 0).ToPacket();
        wrongName.fields[index].name = "wrong";
        failures += ExpectError(
            PetItemSwap::FromPacket(wrongName),
            cg::packet::PacketErrorCode::DecodeFailed,
            "kiG",
            true,
            index,
            "kiG FromPacket wrong name");

        cg::packet::Packet wrongType = PetItemSwap(0, 0, 0, 0).ToPacket();
        wrongType.fields[index].type =
            cg::packet::PacketFieldType::EscapedString;
        failures += ExpectError(
            PetItemSwap::FromPacket(wrongType),
            cg::packet::PacketErrorCode::InvalidFieldType,
            "kiG",
            true,
            index,
            "kiG FromPacket wrong type");

        cg::packet::Packet invalidValue = PetItemSwap(0, 0, 0, 0).ToPacket();
        invalidValue.fields[index].value = "not-an-int";
        failures += ExpectError(
            PetItemSwap::FromPacket(invalidValue),
            cg::packet::PacketErrorCode::DecodeFailed,
            "kiG",
            true,
            index,
            "kiG FromPacket invalid decimal");

        cg::packet::Packet positiveOverflow =
            PetItemSwap(0, 0, 0, 0).ToPacket();
        positiveOverflow.fields[index].value = "2147483648";
        failures += ExpectError(
            PetItemSwap::FromPacket(positiveOverflow),
            cg::packet::PacketErrorCode::DecodeFailed,
            "kiG",
            true,
            index,
            "kiG FromPacket positive decimal overflow");

        cg::packet::Packet negativeOverflow =
            PetItemSwap(0, 0, 0, 0).ToPacket();
        negativeOverflow.fields[index].value = "-2147483649";
        failures += ExpectError(
            PetItemSwap::FromPacket(negativeOverflow),
            cg::packet::PacketErrorCode::DecodeFailed,
            "kiG",
            true,
            index,
            "kiG FromPacket negative decimal overflow");
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
                  << " pet item swap packet test(s) failed\n";
        return 1;
    }
    return 0;
}
