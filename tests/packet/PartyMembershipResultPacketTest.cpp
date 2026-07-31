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

using cg::packet::receive::PartyMembershipResult;

constexpr std::array<std::string_view, 2U> FieldNames = {{
    "request",
    "result",
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
    const PartyMembershipResult& value,
    const std::array<int, 2U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(value.request == expected[0], message);
    failures += Expect(value.result == expected[1], message);
    return failures;
}

int ExpectPacket(
    const cg::packet::Packet& packet,
    const std::array<std::string, 2U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode("bNY"),
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
    std::string wire = "bNY";
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        wire.push_back(' ');
        wire.append(index == fieldIndex ? token : std::string_view("0"));
    }
    return wire;
}

int TestSpecAndRoundtrip()
{
    constexpr std::array<int, 2U> Expected = {{1, 0}};
    const PartyMembershipResult value(Expected[0], Expected[1]);
    const cg::packet::Packet packet = value.ToPacket();

    int failures = 0;
    failures += Expect(PartyMembershipResult::Code == "bNY", "bNY code");
    failures += Expect(
        PartyMembershipResult::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "bNY direction");
    failures += Expect(
        PartyMembershipResult::Fields.size() == FieldNames.size(),
        "bNY field count");
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(
            PartyMembershipResult::Fields[index].name == FieldNames[index],
            "bNY field name order");
        failures += Expect(
            PartyMembershipResult::Fields[index].type ==
                cg::packet::PacketFieldType::Int62,
            "bNY field type order");
    }
    failures += Expect(
        PartyMembershipResult::Spec.code == PartyMembershipResult::Code,
        "bNY Spec code");
    failures += Expect(
        PartyMembershipResult::Spec.direction == PartyMembershipResult::Direction,
        "bNY Spec direction");
    failures += Expect(
        PartyMembershipResult::Spec.fieldCount == FieldNames.size(),
        "bNY Spec field count");
    failures += ExpectValues(value, Expected, "bNY members");
    failures += ExpectPacket(packet, {{"1", "0"}}, "bNY ToPacket");

    const cg::packet::PacketResult<PartyMembershipResult> converted =
        PartyMembershipResult::FromPacket(packet);
    failures += Expect(converted.HasValue(), "bNY FromPacket valid");
    if (converted.HasValue()) {
        failures += ExpectValues(converted.Value(), Expected, "bNY FromPacket values");
    }

    const cg::packet::PacketResult<PartyMembershipResult> typed =
        PartyMembershipResult::Parse("bNY 1 0");
    failures += Expect(typed.HasValue(), "bNY typed parse");
    if (typed.HasValue()) {
        failures += ExpectValues(typed.Value(), Expected, "bNY typed values");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("bNY 1 0", PartyMembershipResult::Spec);
    failures += Expect(generic.HasValue(), "bNY generic parse");
    if (generic.HasValue()) {
        failures += ExpectPacket(generic.Value(), {{"1", "0"}}, "bNY generic packet");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(generic.Value(), PartyMembershipResult::Spec);
        failures += Expect(rebuilt.HasValue(), "bNY generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(rebuilt.Value() == "bNY 1 0 ", "bNY canonical");
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
        {{{1, 0}}, "bNY 1 0 "},
        {{{0, 1}}, "bNY 0 1 "},
        {{{-1, -1}}, "bNY -1 -1 "},
        {{{10, -10}}, "bNY a -a "},
        {{{std::numeric_limits<int>::min(),
           std::numeric_limits<int>::max()}},
         "bNY -2lkCB2 2lkCB1 "},
        {{{std::numeric_limits<int>::max(),
           std::numeric_limits<int>::min()}},
         "bNY 2lkCB1 -2lkCB2 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const PartyMembershipResult value(testCase.values[0], testCase.values[1]);
        failures += ExpectPacket(
            value.ToPacket(),
            {{
                std::to_string(testCase.values[0]),
                std::to_string(testCase.values[1]),
            }},
            "bNY signed packet");

        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(value.ToPacket(), PartyMembershipResult::Spec);
        failures += Expect(built.HasValue(), "bNY signed build");
        if (built.HasValue()) {
            failures += Expect(built.Value() == testCase.canonical, "bNY signed canonical");
        }

        const cg::packet::PacketResult<PartyMembershipResult> parsed =
            PartyMembershipResult::Parse(testCase.canonical);
        failures += Expect(parsed.HasValue(), "bNY signed parse");
        if (parsed.HasValue()) {
            failures += ExpectValues(parsed.Value(), testCase.values, "bNY signed values");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        PartyMembershipResult::Parse("bad 1 0"),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "bNY wrong code");

    constexpr std::array<std::string_view, 2U> MissingWires = {{
        "bNY",
        "bNY 1",
    }};
    for (std::size_t index = 0U; index < MissingWires.size(); ++index) {
        failures += ExpectError(
            PartyMembershipResult::Parse(MissingWires[index]),
            cg::packet::PacketErrorCode::FieldCountMismatch,
            "bNY",
            true,
            index,
            "bNY progressively missing field");
    }

    failures += ExpectError(
        PartyMembershipResult::Parse("bNY 1 0 2"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "bNY",
        true,
        2U,
        "bNY extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += ExpectError(
            PartyMembershipResult::Parse(BuildWireWithField(index, "_")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "bNY",
            true,
            index,
            "bNY invalid i62 field");
        failures += ExpectError(
            PartyMembershipResult::Parse(BuildWireWithField(index, "2lkCB2")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "bNY",
            true,
            index,
            "bNY positive i62 overflow field");
        failures += ExpectError(
            PartyMembershipResult::Parse(BuildWireWithField(index, "-2lkCB3")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "bNY",
            true,
            index,
            "bNY negative i62 overflow field");
    }

    const std::string pollutedCode = "bNY\x17" "x";
    failures += ExpectError(
        PartyMembershipResult::Parse(pollutedCode + " 1 0"),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "bNY polluted code");

    const std::string pollutedField = "bNY 1\x17" "x 0";
    failures += ExpectError(
        PartyMembershipResult::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "bNY",
        true,
        2U,
        "bNY polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = PartyMembershipResult(1, 0).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        PartyMembershipResult::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "bNY FromPacket wrong code");

    cg::packet::Packet wrongDirection = PartyMembershipResult(1, 0).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        PartyMembershipResult::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "bNY",
        false,
        0U,
        "bNY FromPacket wrong direction");

    cg::packet::Packet missingField(
        cg::packet::PacketCode("bNY"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        PartyMembershipResult::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "bNY",
        true,
        0U,
        "bNY FromPacket missing field");

    cg::packet::Packet extraField = PartyMembershipResult(1, 0).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "0",
    });
    failures += ExpectError(
        PartyMembershipResult::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "bNY",
        true,
        2U,
        "bNY FromPacket extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        cg::packet::Packet wrongName = PartyMembershipResult(1, 0).ToPacket();
        wrongName.fields[index].name = "wrong";
        failures += ExpectError(
            PartyMembershipResult::FromPacket(wrongName),
            cg::packet::PacketErrorCode::DecodeFailed,
            "bNY",
            true,
            index,
            "bNY FromPacket wrong name");

        cg::packet::Packet wrongType = PartyMembershipResult(1, 0).ToPacket();
        wrongType.fields[index].type = cg::packet::PacketFieldType::EscapedString;
        failures += ExpectError(
            PartyMembershipResult::FromPacket(wrongType),
            cg::packet::PacketErrorCode::InvalidFieldType,
            "bNY",
            true,
            index,
            "bNY FromPacket wrong type");

        cg::packet::Packet invalidValue = PartyMembershipResult(1, 0).ToPacket();
        invalidValue.fields[index].value = "not-an-int";
        failures += ExpectError(
            PartyMembershipResult::FromPacket(invalidValue),
            cg::packet::PacketErrorCode::DecodeFailed,
            "bNY",
            true,
            index,
            "bNY FromPacket invalid decimal");

        cg::packet::Packet positiveOverflow = PartyMembershipResult(1, 0).ToPacket();
        positiveOverflow.fields[index].value = "2147483648";
        failures += ExpectError(
            PartyMembershipResult::FromPacket(positiveOverflow),
            cg::packet::PacketErrorCode::DecodeFailed,
            "bNY",
            true,
            index,
            "bNY FromPacket positive decimal overflow");

        cg::packet::Packet negativeOverflow = PartyMembershipResult(1, 0).ToPacket();
        negativeOverflow.fields[index].value = "-2147483649";
        failures += ExpectError(
            PartyMembershipResult::FromPacket(negativeOverflow),
            cg::packet::PacketErrorCode::DecodeFailed,
            "bNY",
            true,
            index,
            "bNY FromPacket negative decimal overflow");
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
        "code=bNY\n"
        "direction=server_to_client\n"
        "sample=party-join-failure\n"
        "plain_hex=62 4E 59 20 31 20 30\n"
        "canonical_hex=62 4E 59 20 31 20 30 20\n"
        "end_sample\n";

    const std::string fixture = ReadFixture(path);
    int failures = 0;
    failures += Expect(fixture == Expected, "bNY fixture exact bytes");

    std::size_t lineFeeds = 0U;
    bool asciiOnly = true;
    bool physicalBytesClean = true;
    for (const unsigned char byte : fixture) {
        lineFeeds += byte == 0x0AU ? 1U : 0U;
        asciiOnly = asciiOnly && byte <= 0x7FU;
        physicalBytesClean =
            physicalBytesClean && byte != 0x00U && byte != 0x0DU && byte != 0x17U;
    }
    failures += Expect(asciiOnly, "bNY fixture ASCII");
    failures += Expect(physicalBytesClean, "bNY fixture physical bytes");
    failures += Expect(lineFeeds == 7U, "bNY fixture LF count");
    return failures;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "usage: cg-packet-party-membership-result-test <bNY-fixture>\n";
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
                  << " party membership result packet test(s) failed\n";
        return 1;
    }
    return 0;
}
