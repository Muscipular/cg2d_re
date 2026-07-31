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

using cg::packet::receive::ItemMoveNotification;

constexpr std::array<std::string_view, 2U> FieldNames = {{
    "fromindex",
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
    const ItemMoveNotification& value,
    const std::array<int, 2U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(value.fromIndex == expected[0], message);
    failures += Expect(value.toIndex == expected[1], message);
    return failures;
}

int ExpectPacket(
    const cg::packet::Packet& packet,
    const std::array<std::string, 2U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode("vWpn"),
        message);
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
        failures += Expect(packet.fields[index].value == expected[index], message);
    }
    return failures;
}

std::string BuildWireWithField(std::size_t fieldIndex, std::string_view token)
{
    std::string wire = "vWpn";
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        wire.push_back(' ');
        wire.append(index == fieldIndex ? token : std::string_view("0"));
    }
    return wire;
}

int TestSpecAndRoundtrip()
{
    constexpr std::array<int, 2U> Expected = {{0, 1}};
    const ItemMoveNotification value(Expected[0], Expected[1]);
    const cg::packet::Packet packet = value.ToPacket();

    int failures = 0;
    failures += Expect(ItemMoveNotification::Code == "vWpn", "vWpn code");
    failures += Expect(
        ItemMoveNotification::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "vWpn direction");
    failures += Expect(
        ItemMoveNotification::Fields.size() == FieldNames.size(),
        "vWpn field count");
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(
            ItemMoveNotification::Fields[index].name == FieldNames[index],
            "vWpn field name order");
        failures += Expect(
            ItemMoveNotification::Fields[index].type ==
                cg::packet::PacketFieldType::Int62,
            "vWpn field type order");
    }
    failures += Expect(
        ItemMoveNotification::Spec.code == ItemMoveNotification::Code,
        "vWpn Spec code");
    failures += Expect(
        ItemMoveNotification::Spec.direction == ItemMoveNotification::Direction,
        "vWpn Spec direction");
    failures += Expect(
        ItemMoveNotification::Spec.fieldCount == FieldNames.size(),
        "vWpn Spec field count");
    failures += ExpectValues(value, Expected, "vWpn members");
    failures += ExpectPacket(packet, {{"0", "1"}}, "vWpn ToPacket");

    const cg::packet::PacketResult<ItemMoveNotification> converted =
        ItemMoveNotification::FromPacket(packet);
    failures += Expect(converted.HasValue(), "vWpn FromPacket valid");
    if (converted.HasValue()) {
        failures += ExpectValues(converted.Value(), Expected, "vWpn FromPacket values");
    }

    const cg::packet::PacketResult<ItemMoveNotification> typed =
        ItemMoveNotification::Parse("vWpn 0 1");
    failures += Expect(typed.HasValue(), "vWpn typed parse");
    if (typed.HasValue()) {
        failures += ExpectValues(typed.Value(), Expected, "vWpn typed values");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("vWpn 0 1", ItemMoveNotification::Spec);
    failures += Expect(generic.HasValue(), "vWpn generic parse");
    if (generic.HasValue()) {
        failures += ExpectPacket(generic.Value(), {{"0", "1"}}, "vWpn generic packet");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(generic.Value(), ItemMoveNotification::Spec);
        failures += Expect(rebuilt.HasValue(), "vWpn generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(rebuilt.Value() == "vWpn 0 1 ", "vWpn canonical");
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
        {{{0, 1}}, "vWpn 0 1 "},
        {{{-1, -1}}, "vWpn -1 -1 "},
        {{{10, -10}}, "vWpn a -a "},
        {{{std::numeric_limits<int>::min(),
           std::numeric_limits<int>::max()}},
         "vWpn -2lkCB2 2lkCB1 "},
        {{{std::numeric_limits<int>::max(),
           std::numeric_limits<int>::min()}},
         "vWpn 2lkCB1 -2lkCB2 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const ItemMoveNotification value(testCase.values[0], testCase.values[1]);
        failures += ExpectPacket(
            value.ToPacket(),
            {{
                std::to_string(testCase.values[0]),
                std::to_string(testCase.values[1]),
            }},
            "vWpn signed packet");

        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(value.ToPacket(), ItemMoveNotification::Spec);
        failures += Expect(built.HasValue(), "vWpn signed build");
        if (built.HasValue()) {
            failures += Expect(built.Value() == testCase.canonical, "vWpn signed canonical");
        }

        const cg::packet::PacketResult<ItemMoveNotification> parsed =
            ItemMoveNotification::Parse(testCase.canonical);
        failures += Expect(parsed.HasValue(), "vWpn signed parse");
        if (parsed.HasValue()) {
            failures += ExpectValues(parsed.Value(), testCase.values, "vWpn signed values");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        ItemMoveNotification::Parse("bad 0 1"),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "vWpn wrong code");

    constexpr std::array<std::string_view, 2U> MissingWires = {{
        "vWpn",
        "vWpn 0",
    }};
    for (std::size_t index = 0U; index < MissingWires.size(); ++index) {
        failures += ExpectError(
            ItemMoveNotification::Parse(MissingWires[index]),
            cg::packet::PacketErrorCode::FieldCountMismatch,
            "vWpn",
            true,
            index,
            "vWpn progressively missing field");
    }

    failures += ExpectError(
        ItemMoveNotification::Parse("vWpn 0 1 2"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "vWpn",
        true,
        2U,
        "vWpn extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += ExpectError(
            ItemMoveNotification::Parse(BuildWireWithField(index, "_")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "vWpn",
            true,
            index,
            "vWpn invalid i62 field");
        failures += ExpectError(
            ItemMoveNotification::Parse(BuildWireWithField(index, "2lkCB2")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "vWpn",
            true,
            index,
            "vWpn positive i62 overflow field");
        failures += ExpectError(
            ItemMoveNotification::Parse(BuildWireWithField(index, "-2lkCB3")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "vWpn",
            true,
            index,
            "vWpn negative i62 overflow field");
    }

    const std::string pollutedCode = "vWpn\x17" "x";
    failures += ExpectError(
        ItemMoveNotification::Parse(pollutedCode + " 0 1"),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "vWpn polluted code");

    const std::string pollutedField = "vWpn 0\x17" "x 1";
    failures += ExpectError(
        ItemMoveNotification::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "vWpn",
        true,
        2U,
        "vWpn polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = ItemMoveNotification(0, 1).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        ItemMoveNotification::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "vWpn FromPacket wrong code");

    cg::packet::Packet wrongDirection = ItemMoveNotification(0, 1).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        ItemMoveNotification::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "vWpn",
        false,
        0U,
        "vWpn FromPacket wrong direction");

    cg::packet::Packet missingField(
        cg::packet::PacketCode("vWpn"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        ItemMoveNotification::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "vWpn",
        true,
        0U,
        "vWpn FromPacket missing field");

    cg::packet::Packet extraField = ItemMoveNotification(0, 1).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "0",
    });
    failures += ExpectError(
        ItemMoveNotification::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "vWpn",
        true,
        2U,
        "vWpn FromPacket extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        cg::packet::Packet wrongName = ItemMoveNotification(0, 1).ToPacket();
        wrongName.fields[index].name = "wrong";
        failures += ExpectError(
            ItemMoveNotification::FromPacket(wrongName),
            cg::packet::PacketErrorCode::DecodeFailed,
            "vWpn",
            true,
            index,
            "vWpn FromPacket wrong name");

        cg::packet::Packet wrongType = ItemMoveNotification(0, 1).ToPacket();
        wrongType.fields[index].type = cg::packet::PacketFieldType::EscapedString;
        failures += ExpectError(
            ItemMoveNotification::FromPacket(wrongType),
            cg::packet::PacketErrorCode::InvalidFieldType,
            "vWpn",
            true,
            index,
            "vWpn FromPacket wrong type");

        cg::packet::Packet invalidValue = ItemMoveNotification(0, 1).ToPacket();
        invalidValue.fields[index].value = "not-an-int";
        failures += ExpectError(
            ItemMoveNotification::FromPacket(invalidValue),
            cg::packet::PacketErrorCode::DecodeFailed,
            "vWpn",
            true,
            index,
            "vWpn FromPacket invalid decimal");

        cg::packet::Packet positiveOverflow = ItemMoveNotification(0, 1).ToPacket();
        positiveOverflow.fields[index].value = "2147483648";
        failures += ExpectError(
            ItemMoveNotification::FromPacket(positiveOverflow),
            cg::packet::PacketErrorCode::DecodeFailed,
            "vWpn",
            true,
            index,
            "vWpn FromPacket positive decimal overflow");

        cg::packet::Packet negativeOverflow = ItemMoveNotification(0, 1).ToPacket();
        negativeOverflow.fields[index].value = "-2147483649";
        failures += ExpectError(
            ItemMoveNotification::FromPacket(negativeOverflow),
            cg::packet::PacketErrorCode::DecodeFailed,
            "vWpn",
            true,
            index,
            "vWpn FromPacket negative decimal overflow");
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
        "code=vWpn\n"
        "direction=server_to_client\n"
        "sample=item-slots-zero-one\n"
        "plain_hex=76 57 70 6E 20 30 20 31\n"
        "canonical_hex=76 57 70 6E 20 30 20 31 20\n"
        "end_sample\n";

    const std::string fixture = ReadFixture(path);
    int failures = 0;
    failures += Expect(fixture == Expected, "vWpn fixture exact bytes");

    std::size_t lineFeeds = 0U;
    bool asciiOnly = true;
    bool physicalBytesClean = true;
    for (const unsigned char byte : fixture) {
        lineFeeds += byte == 0x0AU ? 1U : 0U;
        asciiOnly = asciiOnly && byte <= 0x7FU;
        physicalBytesClean =
            physicalBytesClean && byte != 0x00U && byte != 0x0DU && byte != 0x17U;
    }
    failures += Expect(asciiOnly, "vWpn fixture ASCII");
    failures += Expect(physicalBytesClean, "vWpn fixture physical bytes");
    failures += Expect(lineFeeds == 7U, "vWpn fixture LF count");
    return failures;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "usage: cg-packet-item-move-notification-test <vWpn-fixture>\n";
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
                  << " item move notification packet test(s) failed\n";
        return 1;
    }
    return 0;
}
