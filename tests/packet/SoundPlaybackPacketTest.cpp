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

using cg::packet::receive::NpcSoundEffect;
using cg::packet::receive::SoundPlayback;

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
    const SoundPlayback& value,
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
        packet.code == cg::packet::PacketCode("murD"),
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
    std::string wire = "murD";
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        wire.push_back(' ');
        wire.append(index == fieldIndex ? token : std::string_view("0"));
    }
    return wire;
}

int TestSpecAndRoundtrip()
{
    constexpr std::array<int, 3U> Expected = {{13, 27, 30}};
    const SoundPlayback value(Expected[0], Expected[1], Expected[2]);
    const cg::packet::Packet packet = value.ToPacket();

    int failures = 0;
    failures += Expect(SoundPlayback::Code == "murD", "murD code");
    failures += Expect(
        SoundPlayback::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "murD direction");
    failures += Expect(
        SoundPlayback::Fields.size() == FieldNames.size(),
        "murD field count");
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(
            SoundPlayback::Fields[index].name == FieldNames[index],
            "murD field name order");
        failures += Expect(
            SoundPlayback::Fields[index].type ==
                cg::packet::PacketFieldType::Int62,
            "murD field type order");
    }
    failures += Expect(
        SoundPlayback::Spec.code == SoundPlayback::Code,
        "murD Spec code");
    failures += Expect(
        SoundPlayback::Spec.direction == SoundPlayback::Direction,
        "murD Spec direction");
    failures += Expect(
        SoundPlayback::Spec.fieldCount == FieldNames.size(),
        "murD Spec field count");
    failures += ExpectValues(value, Expected, "murD members");
    failures += ExpectPacket(packet, {{"13", "27", "30"}}, "murD ToPacket");

    const cg::packet::PacketResult<SoundPlayback> converted =
        SoundPlayback::FromPacket(packet);
    failures += Expect(converted.HasValue(), "murD FromPacket valid");
    if (converted.HasValue()) {
        failures += ExpectValues(
            converted.Value(),
            Expected,
            "murD FromPacket values");
    }

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(packet, SoundPlayback::Spec);
    failures += Expect(built.HasValue(), "murD build");
    if (built.HasValue()) {
        failures += Expect(
            built.Value() == "murD d r u ",
            "murD canonical");
    }

    const cg::packet::PacketResult<SoundPlayback> typed =
        SoundPlayback::Parse("murD d r u");
    failures += Expect(typed.HasValue(), "murD typed parse");
    if (typed.HasValue()) {
        failures += ExpectValues(
            typed.Value(),
            Expected,
            "murD typed values");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("murD d r u", SoundPlayback::Spec);
    failures += Expect(generic.HasValue(), "murD generic parse");
    if (generic.HasValue()) {
        failures += ExpectPacket(
            generic.Value(),
            {{"13", "27", "30"}},
            "murD generic packet");
        const cg::packet::PacketResult<std::string> genericBuilt =
            cg::packet::BuildPacket(generic.Value(), SoundPlayback::Spec);
        failures += Expect(genericBuilt.HasValue(), "murD generic build");
        if (genericBuilt.HasValue()) {
            failures += Expect(
                genericBuilt.Value() == "murD d r u ",
                "murD generic canonical");
        }
    }
    return failures;
}

int TestSignedValues()
{
    struct Case
    {
        std::array<int, 3U> values;
        std::string_view canonical;
    };

    // 小于/大于 1000 的声号分流及坐标消费属于客户端业务层。
    constexpr Case Cases[] = {
        {{{13, 27, 30}}, "murD d r u "},
        {{{-13, -27, -30}}, "murD -d -r -u "},
        {{{999, -1, 0}}, "murD g7 -1 0 "},
        {{{1000, 12345, -789}}, "murD g8 3d7 -cJ "},
        {{{std::numeric_limits<int>::min(),
           std::numeric_limits<int>::max(),
           std::numeric_limits<int>::min()}},
         "murD -2lkCB2 2lkCB1 -2lkCB2 "},
        {{{std::numeric_limits<int>::max(),
           std::numeric_limits<int>::min(),
           std::numeric_limits<int>::max()}},
         "murD 2lkCB1 -2lkCB2 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const SoundPlayback value(
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
            "murD signed packet");

        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(value.ToPacket(), SoundPlayback::Spec);
        failures += Expect(built.HasValue(), "murD signed build");
        if (built.HasValue()) {
            failures += Expect(
                built.Value() == testCase.canonical,
                "murD signed canonical");
        }

        const cg::packet::PacketResult<SoundPlayback> parsed =
            SoundPlayback::Parse(testCase.canonical);
        failures += Expect(parsed.HasValue(), "murD signed parse");
        if (parsed.HasValue()) {
            failures += ExpectValues(
                parsed.Value(),
                testCase.values,
                "murD signed values");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        SoundPlayback::Parse("bad 0 0 0 "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "murD wrong code");

    constexpr std::array<std::string_view, 3U> MissingWires = {{
        "murD",
        "murD 0",
        "murD 0 0",
    }};
    for (std::size_t index = 0U; index < MissingWires.size(); ++index) {
        failures += ExpectError(
            SoundPlayback::Parse(MissingWires[index]),
            cg::packet::PacketErrorCode::FieldCountMismatch,
            "murD",
            true,
            index,
            "murD progressively missing field");
    }

    failures += ExpectError(
        SoundPlayback::Parse("murD 0 0 0 0 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "murD",
        true,
        3U,
        "murD extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += ExpectError(
            SoundPlayback::Parse(BuildWireWithField(index, "_")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "murD",
            true,
            index,
            "murD invalid i62 field");
        failures += ExpectError(
            SoundPlayback::Parse(BuildWireWithField(index, "2lkCB2")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "murD",
            true,
            index,
            "murD positive i62 overflow field");
        failures += ExpectError(
            SoundPlayback::Parse(BuildWireWithField(index, "-2lkCB3")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "murD",
            true,
            index,
            "murD negative i62 overflow field");
    }

    const std::string pollutedCode = "murD\x17" "x";
    failures += ExpectError(
        SoundPlayback::Parse(pollutedCode + " 0 0 0"),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "murD polluted code");

    const std::string pollutedField = "murD 0\x17" "x 0 0";
    failures += ExpectError(
        SoundPlayback::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "murD",
        true,
        3U,
        "murD polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = SoundPlayback(0, 0, 0).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        SoundPlayback::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "murD FromPacket wrong code");

    cg::packet::Packet wrongDirection = SoundPlayback(0, 0, 0).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        SoundPlayback::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "murD",
        false,
        0U,
        "murD FromPacket wrong direction");

    cg::packet::Packet missingField(
        cg::packet::PacketCode("murD"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        SoundPlayback::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "murD",
        true,
        0U,
        "murD FromPacket missing field");

    cg::packet::Packet extraField = SoundPlayback(0, 0, 0).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "0",
    });
    failures += ExpectError(
        SoundPlayback::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "murD",
        true,
        3U,
        "murD FromPacket extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        cg::packet::Packet wrongName = SoundPlayback(0, 0, 0).ToPacket();
        wrongName.fields[index].name = "wrong";
        failures += ExpectError(
            SoundPlayback::FromPacket(wrongName),
            cg::packet::PacketErrorCode::DecodeFailed,
            "murD",
            true,
            index,
            "murD FromPacket wrong name");

        cg::packet::Packet wrongType = SoundPlayback(0, 0, 0).ToPacket();
        wrongType.fields[index].type =
            cg::packet::PacketFieldType::EscapedString;
        failures += ExpectError(
            SoundPlayback::FromPacket(wrongType),
            cg::packet::PacketErrorCode::InvalidFieldType,
            "murD",
            true,
            index,
            "murD FromPacket wrong type");

        cg::packet::Packet invalidValue = SoundPlayback(0, 0, 0).ToPacket();
        invalidValue.fields[index].value = "not-an-int";
        failures += ExpectError(
            SoundPlayback::FromPacket(invalidValue),
            cg::packet::PacketErrorCode::DecodeFailed,
            "murD",
            true,
            index,
            "murD FromPacket invalid decimal");

        cg::packet::Packet positiveOverflow =
            SoundPlayback(0, 0, 0).ToPacket();
        positiveOverflow.fields[index].value = "2147483648";
        failures += ExpectError(
            SoundPlayback::FromPacket(positiveOverflow),
            cg::packet::PacketErrorCode::DecodeFailed,
            "murD",
            true,
            index,
            "murD FromPacket positive decimal overflow");

        cg::packet::Packet negativeOverflow =
            SoundPlayback(0, 0, 0).ToPacket();
        negativeOverflow.fields[index].value = "-2147483649";
        failures += ExpectError(
            SoundPlayback::FromPacket(negativeOverflow),
            cg::packet::PacketErrorCode::DecodeFailed,
            "murD",
            true,
            index,
            "murD FromPacket negative decimal overflow");
    }
    return failures;
}

int TestAdjacentCodeIsolation()
{
    int failures = 0;
    failures += ExpectError(
        SoundPlayback::Parse("ftaN d r u"),
        cg::packet::PacketErrorCode::InvalidCode,
        "ftaN",
        false,
        0U,
        "murD rejects ftaN");
    failures += ExpectError(
        NpcSoundEffect::Parse("murD d r u"),
        cg::packet::PacketErrorCode::InvalidCode,
        "murD",
        false,
        0U,
        "ftaN rejects murD");
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
        "code=murD\n"
        "direction=server_to_client\n"
        "sample=sound-thirteen-at-position\n"
        "plain_hex=6D 75 72 44 20 64 20 72 20 75\n"
        "canonical_hex=6D 75 72 44 20 64 20 72 20 75 20\n"
        "end_sample\n";

    const std::string fixture = ReadFixture(path);
    int failures = 0;
    failures += Expect(fixture == Expected, "murD fixture exact bytes");

    std::size_t lineFeeds = 0U;
    bool asciiOnly = true;
    bool physicalBytesClean = true;
    for (const unsigned char byte : fixture) {
        lineFeeds += byte == 0x0AU ? 1U : 0U;
        asciiOnly = asciiOnly && byte <= 0x7FU;
        physicalBytesClean =
            physicalBytesClean && byte != 0x00U && byte != 0x0DU &&
            byte != 0x17U;
    }
    failures += Expect(asciiOnly, "murD fixture ASCII");
    failures += Expect(physicalBytesClean, "murD fixture physical bytes");
    failures += Expect(lineFeeds == 7U, "murD fixture LF count");
    return failures;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr
            << "usage: cg-packet-sound-playback-test <murD-fixture>\n";
        return 1;
    }

    const int failures =
        TestSpecAndRoundtrip()
        + TestSignedValues()
        + TestParseErrors()
        + TestFromPacketErrors()
        + TestAdjacentCodeIsolation()
        + TestFixturePhysical(argv[1]);
    if (failures != 0) {
        std::cerr << failures << " sound playback packet test(s) failed\n";
        return 1;
    }
    return 0;
}
