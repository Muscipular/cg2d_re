#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::NpcSoundEffect;

constexpr std::array<std::string_view, 3U> FieldNames = {{
    "seno",
    "x",
    "y",
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
    const NpcSoundEffect& value,
    const std::array<int, 3U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(value.soundNumber == expected[0], message);
    failures += Expect(value.x == expected[1], message);
    failures += Expect(value.y == expected[2], message);
    return failures;
}

int ExpectPacket(
    const cg::packet::Packet& packet,
    const std::array<std::string, 3U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode("ftaN"),
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
    std::string wire = "ftaN";
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        wire.push_back(' ');
        wire.append(index == fieldIndex ? token : std::string_view("0"));
    }
    return wire;
}

int TestSpecAndRoundtrip()
{
    constexpr std::array<int, 3U> Expected = {{13, 27, 30}};
    const NpcSoundEffect value(Expected[0], Expected[1], Expected[2]);
    const cg::packet::Packet packet = value.ToPacket();

    int failures = 0;
    failures += Expect(NpcSoundEffect::Code == "ftaN", "ftaN code");
    failures += Expect(
        NpcSoundEffect::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "ftaN direction");
    failures += Expect(
        NpcSoundEffect::Fields.size() == FieldNames.size(),
        "ftaN field count");
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(
            NpcSoundEffect::Fields[index].name == FieldNames[index],
            "ftaN field name order");
        failures += Expect(
            NpcSoundEffect::Fields[index].type ==
                cg::packet::PacketFieldType::Int62,
            "ftaN field type order");
    }
    failures += Expect(
        NpcSoundEffect::Spec.code == NpcSoundEffect::Code,
        "ftaN Spec code");
    failures += Expect(
        NpcSoundEffect::Spec.direction == NpcSoundEffect::Direction,
        "ftaN Spec direction");
    failures += Expect(
        NpcSoundEffect::Spec.fieldCount == FieldNames.size(),
        "ftaN Spec field count");
    failures += ExpectValues(value, Expected, "ftaN members");
    failures += ExpectPacket(packet, {{"13", "27", "30"}}, "ftaN ToPacket");

    const cg::packet::PacketResult<NpcSoundEffect> converted =
        NpcSoundEffect::FromPacket(packet);
    failures += Expect(converted.HasValue(), "ftaN FromPacket valid");
    if (converted.HasValue()) {
        failures += ExpectValues(
            converted.Value(),
            Expected,
            "ftaN FromPacket values");
    }

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(packet, NpcSoundEffect::Spec);
    failures += Expect(built.HasValue(), "ftaN build");
    if (built.HasValue()) {
        failures += Expect(
            built.Value() == "ftaN d r u ",
            "ftaN canonical");
    }

    const cg::packet::PacketResult<NpcSoundEffect> typed =
        NpcSoundEffect::Parse("ftaN d r u");
    failures += Expect(typed.HasValue(), "ftaN typed parse");
    if (typed.HasValue()) {
        failures += ExpectValues(
            typed.Value(),
            Expected,
            "ftaN typed values");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("ftaN d r u", NpcSoundEffect::Spec);
    failures += Expect(generic.HasValue(), "ftaN generic parse");
    if (generic.HasValue()) {
        failures += ExpectPacket(
            generic.Value(),
            {{"13", "27", "30"}},
            "ftaN generic packet");
        const cg::packet::PacketResult<std::string> genericBuilt =
            cg::packet::BuildPacket(generic.Value(), NpcSoundEffect::Spec);
        failures += Expect(genericBuilt.HasValue(), "ftaN generic build");
        if (genericBuilt.HasValue()) {
            failures += Expect(
                genericBuilt.Value() == "ftaN d r u ",
                "ftaN generic canonical");
        }
    }
    return failures;
}

int TestSignedAndBusinessBoundaryValues()
{
    struct Case
    {
        std::array<int, 3U> values;
        std::string_view canonical;
    };

    // 声号范围、-1 坐标替换、投影与择近播放均属业务层。
    constexpr Case Cases[] = {
        {{{0, -1, -1}}, "ftaN 0 -1 -1 "},
        {{{13, 27, 30}}, "ftaN d r u "},
        {{{62, 12345, -789}}, "ftaN 10 3d7 -cJ "},
        {{{-13, -27, -30}}, "ftaN -d -r -u "},
        {{{std::numeric_limits<int>::min(),
           std::numeric_limits<int>::max(),
           std::numeric_limits<int>::min()}},
         "ftaN -2lkCB2 2lkCB1 -2lkCB2 "},
        {{{std::numeric_limits<int>::max(),
           std::numeric_limits<int>::min(),
           std::numeric_limits<int>::max()}},
         "ftaN 2lkCB1 -2lkCB2 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const NpcSoundEffect value(
            testCase.values[0],
            testCase.values[1],
            testCase.values[2]);
        failures += ExpectPacket(
            value.ToPacket(),
            {{
                std::to_string(testCase.values[0]),
                std::to_string(testCase.values[1]),
                std::to_string(testCase.values[2]),
            }},
            "ftaN signed packet");

        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(value.ToPacket(), NpcSoundEffect::Spec);
        failures += Expect(built.HasValue(), "ftaN signed build");
        if (built.HasValue()) {
            failures += Expect(
                built.Value() == testCase.canonical,
                "ftaN signed canonical");
        }

        const cg::packet::PacketResult<NpcSoundEffect> parsed =
            NpcSoundEffect::Parse(testCase.canonical);
        failures += Expect(parsed.HasValue(), "ftaN signed parse");
        if (parsed.HasValue()) {
            failures += ExpectValues(
                parsed.Value(),
                testCase.values,
                "ftaN signed values");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        NpcSoundEffect::Parse("bad 0 0 0 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "ftaN wrong code");

    constexpr std::array<std::string_view, 3U> MissingWires = {{
        "ftaN",
        "ftaN 0",
        "ftaN 0 0",
    }};
    for (std::size_t index = 0U; index < MissingWires.size(); ++index) {
        failures += ExpectError(
            NpcSoundEffect::Parse(MissingWires[index]),
            cg::packet::PacketErrorCode::FieldCountMismatch,
            "ftaN",
            true,
            index,
            "ftaN progressively missing field");
    }

    failures += ExpectError(
        NpcSoundEffect::Parse("ftaN 0 0 0 0 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ftaN",
        true,
        3U,
        "ftaN extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += ExpectError(
            NpcSoundEffect::Parse(BuildWireWithField(index, "_")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "ftaN",
            true,
            index,
            "ftaN invalid i62 field");
        failures += ExpectError(
            NpcSoundEffect::Parse(BuildWireWithField(index, "2lkCB2")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "ftaN",
            true,
            index,
            "ftaN positive i62 overflow field");
        failures += ExpectError(
            NpcSoundEffect::Parse(BuildWireWithField(index, "-2lkCB3")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "ftaN",
            true,
            index,
            "ftaN negative i62 overflow field");
    }

    const std::string pollutedCode = "ftaN\x17" "x";
    failures += ExpectError(
        NpcSoundEffect::Parse(pollutedCode + " 0 0 0"),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "ftaN polluted code");

    const std::string pollutedField = "ftaN 0\x17" "x 0 0";
    failures += ExpectError(
        NpcSoundEffect::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ftaN",
        true,
        3U,
        "ftaN polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = NpcSoundEffect(0, 0, 0).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        NpcSoundEffect::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "ftaN FromPacket wrong code");

    cg::packet::Packet wrongDirection = NpcSoundEffect(0, 0, 0).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        NpcSoundEffect::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ftaN",
        false,
        0U,
        "ftaN FromPacket wrong direction");

    cg::packet::Packet missingField(
        cg::packet::PacketCode("ftaN"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        NpcSoundEffect::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ftaN",
        true,
        0U,
        "ftaN FromPacket missing field");

    cg::packet::Packet extraField = NpcSoundEffect(0, 0, 0).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "0",
    });
    failures += ExpectError(
        NpcSoundEffect::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ftaN",
        true,
        3U,
        "ftaN FromPacket extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        cg::packet::Packet wrongName = NpcSoundEffect(0, 0, 0).ToPacket();
        wrongName.fields[index].name = "wrong";
        failures += ExpectError(
            NpcSoundEffect::FromPacket(wrongName),
            cg::packet::PacketErrorCode::DecodeFailed,
            "ftaN",
            true,
            index,
            "ftaN FromPacket wrong name");

        cg::packet::Packet wrongType = NpcSoundEffect(0, 0, 0).ToPacket();
        wrongType.fields[index].type =
            cg::packet::PacketFieldType::EscapedString;
        failures += ExpectError(
            NpcSoundEffect::FromPacket(wrongType),
            cg::packet::PacketErrorCode::InvalidFieldType,
            "ftaN",
            true,
            index,
            "ftaN FromPacket wrong type");

        cg::packet::Packet invalidValue = NpcSoundEffect(0, 0, 0).ToPacket();
        invalidValue.fields[index].value = "not-an-int";
        failures += ExpectError(
            NpcSoundEffect::FromPacket(invalidValue),
            cg::packet::PacketErrorCode::DecodeFailed,
            "ftaN",
            true,
            index,
            "ftaN FromPacket invalid decimal");

        cg::packet::Packet positiveOverflow =
            NpcSoundEffect(0, 0, 0).ToPacket();
        positiveOverflow.fields[index].value = "2147483648";
        failures += ExpectError(
            NpcSoundEffect::FromPacket(positiveOverflow),
            cg::packet::PacketErrorCode::DecodeFailed,
            "ftaN",
            true,
            index,
            "ftaN FromPacket positive decimal overflow");

        cg::packet::Packet negativeOverflow =
            NpcSoundEffect(0, 0, 0).ToPacket();
        negativeOverflow.fields[index].value = "-2147483649";
        failures += ExpectError(
            NpcSoundEffect::FromPacket(negativeOverflow),
            cg::packet::PacketErrorCode::DecodeFailed,
            "ftaN",
            true,
            index,
            "ftaN FromPacket negative decimal overflow");
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
        std::cerr << failures << " NPC sound effect packet test(s) failed\n";
        return 1;
    }
    return 0;
}
