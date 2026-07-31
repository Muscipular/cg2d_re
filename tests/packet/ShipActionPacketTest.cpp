#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::ShipAction;

constexpr std::array<std::string_view, 5U> FieldNames = {{
    "type",
    "mapId",
    "floorId",
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
    const ShipAction& value,
    const std::array<int, 5U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(value.type == expected[0], message);
    failures += Expect(value.mapId == expected[1], message);
    failures += Expect(value.floorId == expected[2], message);
    failures += Expect(value.x == expected[3], message);
    failures += Expect(value.y == expected[4], message);
    return failures;
}

int ExpectPacket(
    const cg::packet::Packet& packet,
    const std::array<std::string, 5U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode("PVfz"),
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
    std::string wire = "PVfz";
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        wire.push_back(' ');
        wire.append(index == fieldIndex ? token : std::string_view("0"));
    }
    return wire;
}

int TestSpecAndRoundtrip()
{
    constexpr std::array<int, 5U> Expected = {{-102, 1000, 10, 20, 30}};
    const ShipAction value(
        Expected[0],
        Expected[1],
        Expected[2],
        Expected[3],
        Expected[4]);
    const cg::packet::Packet packet = value.ToPacket();

    int failures = 0;
    failures += Expect(ShipAction::Code == "PVfz", "PVfz code");
    failures += Expect(
        ShipAction::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "PVfz direction");
    failures += Expect(
        ShipAction::Fields.size() == FieldNames.size(),
        "PVfz field count");
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(
            ShipAction::Fields[index].name == FieldNames[index],
            "PVfz field name order");
        failures += Expect(
            ShipAction::Fields[index].type ==
                cg::packet::PacketFieldType::Int62,
            "PVfz field type order");
    }
    failures += Expect(
        ShipAction::Spec.code == ShipAction::Code,
        "PVfz Spec code");
    failures += Expect(
        ShipAction::Spec.direction == ShipAction::Direction,
        "PVfz Spec direction");
    failures += Expect(
        ShipAction::Spec.fieldCount == FieldNames.size(),
        "PVfz Spec field count");
    failures += ExpectValues(value, Expected, "PVfz members");
    failures += ExpectPacket(
        packet,
        {{"-102", "1000", "10", "20", "30"}},
        "PVfz ToPacket");

    const cg::packet::PacketResult<ShipAction> converted =
        ShipAction::FromPacket(packet);
    failures += Expect(converted.HasValue(), "PVfz FromPacket valid");
    if (converted.HasValue()) {
        failures += ExpectValues(
            converted.Value(),
            Expected,
            "PVfz FromPacket values");
    }

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(packet, ShipAction::Spec);
    failures += Expect(built.HasValue(), "PVfz build");
    if (built.HasValue()) {
        failures += Expect(
            built.Value() == "PVfz -1E g8 a k u ",
            "PVfz canonical");
    }

    const cg::packet::PacketResult<ShipAction> typed =
        ShipAction::Parse("PVfz -1E g8 a k u");
    failures += Expect(typed.HasValue(), "PVfz typed parse");
    if (typed.HasValue()) {
        failures += ExpectValues(
            typed.Value(),
            Expected,
            "PVfz typed values");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket(
            "PVfz -1E g8 a k u",
            ShipAction::Spec);
    failures += Expect(generic.HasValue(), "PVfz generic parse");
    if (generic.HasValue()) {
        failures += ExpectPacket(
            generic.Value(),
            {{"-102", "1000", "10", "20", "30"}},
            "PVfz generic packet");
        const cg::packet::PacketResult<std::string> genericBuilt =
            cg::packet::BuildPacket(generic.Value(), ShipAction::Spec);
        failures += Expect(genericBuilt.HasValue(), "PVfz generic build");
        if (genericBuilt.HasValue()) {
            failures += Expect(
                genericBuilt.Value() == "PVfz -1E g8 a k u ",
                "PVfz generic canonical");
        }
    }
    return failures;
}

int TestSignedAndBusinessBoundaryValues()
{
    struct Case
    {
        std::array<int, 5U> values;
        std::string_view canonical;
    };

    // type 分段、subtype 0 字段重载、资源与坐标均属业务层。
    constexpr Case Cases[] = {
        {{{0, 10, 20, 30, 40}}, "PVfz 0 a k u E "},
        {{{1, 10, 20, 30, 40}}, "PVfz 1 a k u E "},
        {{{2, -10, -20, -30, -40}}, "PVfz 2 -a -k -u -E "},
        {{{3, 12345, -12345, 456, -789}},
         "PVfz 3 3d7 -3d7 7m -cJ "},
        {{{4, -1, 0, 1, 2}}, "PVfz 4 -1 0 1 2 "},
        {{{100, 62, 1, 0, 0}}, "PVfz 1C 10 1 0 0 "},
        {{{-100, -62, -1, -20, -30}}, "PVfz -1C -10 -1 -k -u "},
        {{{std::numeric_limits<int>::min(),
           std::numeric_limits<int>::max(),
           std::numeric_limits<int>::min(),
           std::numeric_limits<int>::max(),
           std::numeric_limits<int>::min()}},
         "PVfz -2lkCB2 2lkCB1 -2lkCB2 2lkCB1 -2lkCB2 "},
        {{{std::numeric_limits<int>::max(),
           std::numeric_limits<int>::min(),
           std::numeric_limits<int>::max(),
           std::numeric_limits<int>::min(),
           std::numeric_limits<int>::max()}},
         "PVfz 2lkCB1 -2lkCB2 2lkCB1 -2lkCB2 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const ShipAction value(
            testCase.values[0],
            testCase.values[1],
            testCase.values[2],
            testCase.values[3],
            testCase.values[4]);
        failures += ExpectPacket(
            value.ToPacket(),
            {{
                std::to_string(testCase.values[0]),
                std::to_string(testCase.values[1]),
                std::to_string(testCase.values[2]),
                std::to_string(testCase.values[3]),
                std::to_string(testCase.values[4]),
            }},
            "PVfz signed packet");

        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(value.ToPacket(), ShipAction::Spec);
        failures += Expect(built.HasValue(), "PVfz signed build");
        if (built.HasValue()) {
            failures += Expect(
                built.Value() == testCase.canonical,
                "PVfz signed canonical");
        }

        const cg::packet::PacketResult<ShipAction> parsed =
            ShipAction::Parse(testCase.canonical);
        failures += Expect(parsed.HasValue(), "PVfz signed parse");
        if (parsed.HasValue()) {
            failures += ExpectValues(
                parsed.Value(),
                testCase.values,
                "PVfz signed values");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ShipAction::Parse("bad 0 0 0 0 0 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "PVfz wrong code");

    constexpr std::array<std::string_view, 5U> MissingWires = {{
        "PVfz",
        "PVfz 0",
        "PVfz 0 0",
        "PVfz 0 0 0",
        "PVfz 0 0 0 0",
    }};
    for (std::size_t index = 0U; index < MissingWires.size(); ++index) {
        failures += ExpectError(
            ShipAction::Parse(MissingWires[index]),
            cg::packet::PacketErrorCode::FieldCountMismatch,
            "PVfz",
            true,
            index,
            "PVfz progressively missing field");
    }

    failures += ExpectError(
        ShipAction::Parse("PVfz 0 0 0 0 0 0 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "PVfz",
        true,
        5U,
        "PVfz extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += ExpectError(
            ShipAction::Parse(BuildWireWithField(index, "_")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "PVfz",
            true,
            index,
            "PVfz invalid i62 field");
        failures += ExpectError(
            ShipAction::Parse(BuildWireWithField(index, "2lkCB2")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "PVfz",
            true,
            index,
            "PVfz positive i62 overflow field");
        failures += ExpectError(
            ShipAction::Parse(BuildWireWithField(index, "-2lkCB3")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "PVfz",
            true,
            index,
            "PVfz negative i62 overflow field");
    }

    const std::string pollutedCode = "PVfz\x17" "x";
    failures += ExpectError(
        ShipAction::Parse(pollutedCode + " 0 0 0 0 0"),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "PVfz polluted code");

    const std::string pollutedField = "PVfz 0 0\x17" "x 0 0 0";
    failures += ExpectError(
        ShipAction::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "PVfz",
        true,
        5U,
        "PVfz polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = ShipAction(0, 0, 0, 0, 0).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        ShipAction::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "PVfz FromPacket wrong code");

    cg::packet::Packet wrongDirection =
        ShipAction(0, 0, 0, 0, 0).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        ShipAction::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "PVfz",
        false,
        0U,
        "PVfz FromPacket wrong direction");

    cg::packet::Packet missingField(
        cg::packet::PacketCode("PVfz"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        ShipAction::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "PVfz",
        true,
        0U,
        "PVfz FromPacket missing field");

    cg::packet::Packet extraField =
        ShipAction(0, 0, 0, 0, 0).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "0",
    });
    failures += ExpectError(
        ShipAction::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "PVfz",
        true,
        5U,
        "PVfz FromPacket extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        cg::packet::Packet wrongName =
            ShipAction(0, 0, 0, 0, 0).ToPacket();
        wrongName.fields[index].name = "wrong";
        failures += ExpectError(
            ShipAction::FromPacket(wrongName),
            cg::packet::PacketErrorCode::DecodeFailed,
            "PVfz",
            true,
            index,
            "PVfz FromPacket wrong name");

        cg::packet::Packet wrongType =
            ShipAction(0, 0, 0, 0, 0).ToPacket();
        wrongType.fields[index].type =
            cg::packet::PacketFieldType::EscapedString;
        failures += ExpectError(
            ShipAction::FromPacket(wrongType),
            cg::packet::PacketErrorCode::InvalidFieldType,
            "PVfz",
            true,
            index,
            "PVfz FromPacket wrong type");

        cg::packet::Packet invalidValue =
            ShipAction(0, 0, 0, 0, 0).ToPacket();
        invalidValue.fields[index].value = "not-an-int";
        failures += ExpectError(
            ShipAction::FromPacket(invalidValue),
            cg::packet::PacketErrorCode::DecodeFailed,
            "PVfz",
            true,
            index,
            "PVfz FromPacket invalid decimal");

        cg::packet::Packet positiveOverflow =
            ShipAction(0, 0, 0, 0, 0).ToPacket();
        positiveOverflow.fields[index].value = "2147483648";
        failures += ExpectError(
            ShipAction::FromPacket(positiveOverflow),
            cg::packet::PacketErrorCode::DecodeFailed,
            "PVfz",
            true,
            index,
            "PVfz FromPacket positive decimal overflow");

        cg::packet::Packet negativeOverflow =
            ShipAction(0, 0, 0, 0, 0).ToPacket();
        negativeOverflow.fields[index].value = "-2147483649";
        failures += ExpectError(
            ShipAction::FromPacket(negativeOverflow),
            cg::packet::PacketErrorCode::DecodeFailed,
            "PVfz",
            true,
            index,
            "PVfz FromPacket negative decimal overflow");
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
        std::cerr << failures << " ship action packet test(s) failed\n";
        return 1;
    }
    return 0;
}
