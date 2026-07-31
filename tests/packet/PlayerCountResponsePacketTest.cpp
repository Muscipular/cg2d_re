#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"
#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::PlayerCountResponse;
using cg::packet::send::PlayerCountRequest;

constexpr std::array<std::string_view, 2U> FieldNames = {{
    "logincount",
    "player",
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
    const PlayerCountResponse& value,
    const std::array<int, 2U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        value.clientConnectionCount == expected[0],
        message);
    failures += Expect(value.playerCount == expected[1], message);
    return failures;
}

int ExpectPacket(
    const cg::packet::Packet& packet,
    const std::array<int, 2U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode("Bi"), message);
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
    std::string wire = "Bi";
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        wire.push_back(' ');
        wire.append(index == fieldIndex ? token : std::string_view("0"));
    }
    return wire;
}

int TestSpecAndRoundtrip()
{
    constexpr std::array<int, 2U> Expected = {{2, 1}};
    const PlayerCountResponse value(Expected[0], Expected[1]);
    const cg::packet::Packet packet = value.ToPacket();

    int failures = 0;
    failures += Expect(PlayerCountResponse::Code == "Bi", "Bi code");
    failures += Expect(
        PlayerCountResponse::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "Bi direction");
    failures += Expect(
        PlayerCountResponse::Fields.size() == FieldNames.size(),
        "Bi field count");
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(
            PlayerCountResponse::Fields[index].name == FieldNames[index],
            "Bi field name order");
        failures += Expect(
            PlayerCountResponse::Fields[index].type ==
                cg::packet::PacketFieldType::Int62,
            "Bi field type order");
    }
    failures += Expect(
        PlayerCountResponse::Spec.code == PlayerCountResponse::Code,
        "Bi Spec code");
    failures += Expect(
        PlayerCountResponse::Spec.direction == PlayerCountResponse::Direction,
        "Bi Spec direction");
    failures += Expect(
        PlayerCountResponse::Spec.fieldCount == FieldNames.size(),
        "Bi Spec field count");
    failures += ExpectValues(value, Expected, "Bi members");
    failures += ExpectPacket(packet, Expected, "Bi ToPacket");

    const cg::packet::PacketResult<PlayerCountResponse> converted =
        PlayerCountResponse::FromPacket(packet);
    failures += Expect(converted.HasValue(), "Bi FromPacket valid");
    if (converted.HasValue()) {
        failures += ExpectValues(converted.Value(), Expected, "Bi FromPacket values");
    }

    const cg::packet::PacketResult<PlayerCountResponse> typed =
        PlayerCountResponse::Parse("Bi 2 1");
    failures += Expect(typed.HasValue(), "Bi typed parse");
    if (typed.HasValue()) {
        failures += ExpectValues(typed.Value(), Expected, "Bi typed values");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("Bi 2 1", PlayerCountResponse::Spec);
    failures += Expect(generic.HasValue(), "Bi generic parse");
    if (generic.HasValue()) {
        failures += ExpectPacket(generic.Value(), Expected, "Bi generic packet");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(generic.Value(), PlayerCountResponse::Spec);
        failures += Expect(rebuilt.HasValue(), "Bi generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(rebuilt.Value() == "Bi 2 1 ", "Bi canonical");
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

    // 非负、player<=logincount 与 ConnectLen 均属于服务端业务层。
    constexpr Case Cases[] = {
        {{{2, 3}}, "Bi 2 3 "},
        {{{-1, -2}}, "Bi -1 -2 "},
        {{{std::numeric_limits<int>::min(),
           std::numeric_limits<int>::max()}},
         "Bi -2lkCB2 2lkCB1 "},
        {{{std::numeric_limits<int>::max(),
           std::numeric_limits<int>::min()}},
         "Bi 2lkCB1 -2lkCB2 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const PlayerCountResponse value(testCase.values[0], testCase.values[1]);
        failures += ExpectPacket(
            value.ToPacket(),
            testCase.values,
            "Bi signed packet");

        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(value.ToPacket(), PlayerCountResponse::Spec);
        failures += Expect(built.HasValue(), "Bi signed build");
        if (built.HasValue()) {
            failures += Expect(
                built.Value() == testCase.canonical,
                "Bi signed canonical");
        }

        const cg::packet::PacketResult<PlayerCountResponse> parsed =
            PlayerCountResponse::Parse(testCase.canonical);
        failures += Expect(parsed.HasValue(), "Bi signed parse");
        if (parsed.HasValue()) {
            failures += ExpectValues(
                parsed.Value(),
                testCase.values,
                "Bi signed values");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        PlayerCountResponse::Parse("bad 2 1"),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "Bi wrong code");
    failures += ExpectError(
        PlayerCountResponse::Parse("Bi 2"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Bi",
        true,
        1U,
        "Bi missing field");
    failures += ExpectError(
        PlayerCountResponse::Parse("Bi 2 1 0"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Bi",
        true,
        2U,
        "Bi extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += ExpectError(
            PlayerCountResponse::Parse(BuildWireWithField(index, "_")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "Bi",
            true,
            index,
            "Bi invalid i62 field");
        failures += ExpectError(
            PlayerCountResponse::Parse(BuildWireWithField(index, "2lkCB2")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "Bi",
            true,
            index,
            "Bi positive i62 overflow field");
        failures += ExpectError(
            PlayerCountResponse::Parse(BuildWireWithField(index, "-2lkCB3")),
            cg::packet::PacketErrorCode::DecodeFailed,
            "Bi",
            true,
            index,
            "Bi negative i62 overflow field");
    }

    const std::string pollutedCode = "Bi\x17" "x";
    failures += ExpectError(
        PlayerCountResponse::Parse(pollutedCode + " 2 1"),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "Bi polluted code");

    const std::string pollutedField = "Bi 2\x17" "x 1";
    failures += ExpectError(
        PlayerCountResponse::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Bi",
        true,
        2U,
        "Bi polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = PlayerCountResponse(2, 1).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        PlayerCountResponse::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "Bi FromPacket wrong code");

    cg::packet::Packet wrongDirection = PlayerCountResponse(2, 1).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        PlayerCountResponse::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Bi",
        false,
        0U,
        "Bi FromPacket wrong direction");

    cg::packet::Packet missingField = PlayerCountResponse(2, 1).ToPacket();
    missingField.fields.pop_back();
    failures += ExpectError(
        PlayerCountResponse::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Bi",
        true,
        1U,
        "Bi FromPacket missing field");

    cg::packet::Packet extraField = PlayerCountResponse(2, 1).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "0",
    });
    failures += ExpectError(
        PlayerCountResponse::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Bi",
        true,
        2U,
        "Bi FromPacket extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        cg::packet::Packet wrongName = PlayerCountResponse(2, 1).ToPacket();
        wrongName.fields[index].name = "wrong";
        failures += ExpectError(
            PlayerCountResponse::FromPacket(wrongName),
            cg::packet::PacketErrorCode::DecodeFailed,
            "Bi",
            true,
            index,
            "Bi FromPacket wrong name");

        cg::packet::Packet wrongType = PlayerCountResponse(2, 1).ToPacket();
        wrongType.fields[index].type = cg::packet::PacketFieldType::EscapedString;
        failures += ExpectError(
            PlayerCountResponse::FromPacket(wrongType),
            cg::packet::PacketErrorCode::InvalidFieldType,
            "Bi",
            true,
            index,
            "Bi FromPacket wrong type");

        cg::packet::Packet invalidValue = PlayerCountResponse(2, 1).ToPacket();
        invalidValue.fields[index].value = "not-an-int";
        failures += ExpectError(
            PlayerCountResponse::FromPacket(invalidValue),
            cg::packet::PacketErrorCode::DecodeFailed,
            "Bi",
            true,
            index,
            "Bi FromPacket invalid decimal");

        cg::packet::Packet positiveOverflow = PlayerCountResponse(2, 1).ToPacket();
        positiveOverflow.fields[index].value = "2147483648";
        failures += ExpectError(
            PlayerCountResponse::FromPacket(positiveOverflow),
            cg::packet::PacketErrorCode::DecodeFailed,
            "Bi",
            true,
            index,
            "Bi FromPacket positive decimal overflow");

        cg::packet::Packet negativeOverflow = PlayerCountResponse(2, 1).ToPacket();
        negativeOverflow.fields[index].value = "-2147483649";
        failures += ExpectError(
            PlayerCountResponse::FromPacket(negativeOverflow),
            cg::packet::PacketErrorCode::DecodeFailed,
            "Bi",
            true,
            index,
            "Bi FromPacket negative decimal overflow");
    }
    return failures;
}

int TestRequestResponseIsolation()
{
    int failures = 0;
    failures += Expect(
        PlayerCountRequest::Code == "rvb" &&
            PlayerCountRequest::Direction ==
                cg::packet::PacketDirection::ClientToServer &&
            PlayerCountRequest::Spec.fieldCount == 0U,
        "rvb request shape isolated");
    failures += ExpectError(
        PlayerCountResponse::Parse("rvb"),
        cg::packet::PacketErrorCode::InvalidCode,
        "rvb",
        false,
        0U,
        "Bi rejects rvb request");

    const cg::packet::PacketResult<cg::packet::Packet> request =
        cg::packet::ParsePacket("rvb", PlayerCountRequest::Spec);
    failures += Expect(request.HasValue(), "rvb request parse remains valid");
    failures += ExpectError(
        cg::packet::ParsePacket("Bi 2 1", PlayerCountRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "Bi",
        false,
        0U,
        "rvb rejects Bi response");
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
        "code=Bi\n"
        "direction=server_to_client\n"
        "sample=two-clients-one-player\n"
        "plain_hex=42 69 20 32 20 31\n"
        "canonical_hex=42 69 20 32 20 31 20\n"
        "end_sample\n";

    const std::string fixture = ReadFixture(path);
    int failures = 0;
    failures += Expect(fixture == Expected, "Bi fixture exact bytes");

    std::size_t lineFeeds = 0U;
    bool asciiOnly = true;
    bool physicalBytesClean = true;
    for (const unsigned char byte : fixture) {
        lineFeeds += byte == 0x0AU ? 1U : 0U;
        asciiOnly = asciiOnly && byte <= 0x7FU;
        physicalBytesClean =
            physicalBytesClean && byte != 0x00U && byte != 0x0DU && byte != 0x17U;
    }
    failures += Expect(asciiOnly, "Bi fixture ASCII");
    failures += Expect(physicalBytesClean, "Bi fixture physical bytes");
    failures += Expect(lineFeeds == 7U, "Bi fixture LF count");
    return failures;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "usage: cg-packet-player-count-response-test <Bi-fixture>\n";
        return 1;
    }

    const int failures =
        TestSpecAndRoundtrip()
        + TestSignedValues()
        + TestParseErrors()
        + TestFromPacketErrors()
        + TestRequestResponseIsolation()
        + TestFixturePhysical(argv[1]);
    if (failures != 0) {
        std::cerr << failures << " player count response packet test(s) failed\n";
        return 1;
    }
    return 0;
}
