#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::BattleHelpModeStatus;

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

int ExpectPacket(
    const cg::packet::Packet& packet,
    std::string_view expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode("sZWu"), message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ServerToClient,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "flg", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == expected, message);
    }
    return failures;
}

int TestSpecAndRoundtrip()
{
    const BattleHelpModeStatus value(1);
    const cg::packet::Packet packet = value.ToPacket();

    int failures = 0;
    failures += Expect(BattleHelpModeStatus::Code == "sZWu", "sZWu code");
    failures += Expect(
        BattleHelpModeStatus::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "sZWu direction");
    failures += Expect(BattleHelpModeStatus::Fields.size() == 1U, "sZWu fields");
    failures += Expect(
        BattleHelpModeStatus::Fields[0].name == "flg",
        "sZWu field name");
    failures += Expect(
        BattleHelpModeStatus::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "sZWu field type");
    failures += Expect(
        BattleHelpModeStatus::Spec.code == BattleHelpModeStatus::Code,
        "sZWu Spec code");
    failures += Expect(
        BattleHelpModeStatus::Spec.direction == BattleHelpModeStatus::Direction,
        "sZWu Spec direction");
    failures += Expect(
        BattleHelpModeStatus::Spec.fieldCount == 1U,
        "sZWu Spec field count");
    failures += Expect(value.helpFlag == 1, "sZWu member");
    failures += ExpectPacket(packet, "1", "sZWu ToPacket");

    const cg::packet::PacketResult<BattleHelpModeStatus> converted =
        BattleHelpModeStatus::FromPacket(packet);
    failures += Expect(converted.HasValue(), "sZWu FromPacket valid");
    if (converted.HasValue()) {
        failures += Expect(converted.Value().helpFlag == 1, "sZWu FromPacket value");
    }

    const cg::packet::PacketResult<BattleHelpModeStatus> typed =
        BattleHelpModeStatus::Parse("sZWu 1");
    failures += Expect(typed.HasValue(), "sZWu typed parse");
    if (typed.HasValue()) {
        failures += Expect(typed.Value().helpFlag == 1, "sZWu typed value");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("sZWu 1", BattleHelpModeStatus::Spec);
    failures += Expect(generic.HasValue(), "sZWu generic parse");
    if (generic.HasValue()) {
        failures += ExpectPacket(generic.Value(), "1", "sZWu generic packet");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(generic.Value(), BattleHelpModeStatus::Spec);
        failures += Expect(rebuilt.HasValue(), "sZWu generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(rebuilt.Value() == "sZWu 1 ", "sZWu canonical");
        }
    }
    return failures;
}

int TestSignedValues()
{
    struct Case
    {
        int helpFlag;
        std::string_view canonical;
    };

    constexpr Case Cases[] = {
        {1, "sZWu 1 "},
        {-1, "sZWu -1 "},
        {std::numeric_limits<int>::min(), "sZWu -2lkCB2 "},
        {std::numeric_limits<int>::max(), "sZWu 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const BattleHelpModeStatus value(testCase.helpFlag);
        failures += ExpectPacket(
            value.ToPacket(),
            std::to_string(testCase.helpFlag),
            "sZWu signed packet");

        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(value.ToPacket(), BattleHelpModeStatus::Spec);
        failures += Expect(built.HasValue(), "sZWu signed build");
        if (built.HasValue()) {
            failures += Expect(built.Value() == testCase.canonical, "sZWu signed canonical");
        }

        const cg::packet::PacketResult<BattleHelpModeStatus> parsed =
            BattleHelpModeStatus::Parse(testCase.canonical);
        failures += Expect(parsed.HasValue(), "sZWu signed parse");
        if (parsed.HasValue()) {
            failures += Expect(
                parsed.Value().helpFlag == testCase.helpFlag,
                "sZWu signed value");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        BattleHelpModeStatus::Parse("bad 1"),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "sZWu wrong code");
    failures += ExpectError(
        BattleHelpModeStatus::Parse("sZWu"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "sZWu",
        true,
        0U,
        "sZWu missing field");
    failures += ExpectError(
        BattleHelpModeStatus::Parse("sZWu 1 0"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "sZWu",
        true,
        1U,
        "sZWu extra field");
    failures += ExpectError(
        BattleHelpModeStatus::Parse("sZWu _"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "sZWu",
        true,
        0U,
        "sZWu invalid i62");
    failures += ExpectError(
        BattleHelpModeStatus::Parse("sZWu 2lkCB2"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "sZWu",
        true,
        0U,
        "sZWu positive overflow");
    failures += ExpectError(
        BattleHelpModeStatus::Parse("sZWu -2lkCB3"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "sZWu",
        true,
        0U,
        "sZWu negative overflow");

    const std::string pollutedCode = "sZWu\x17" "x";
    failures += ExpectError(
        BattleHelpModeStatus::Parse(pollutedCode + " 1"),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "sZWu polluted code");

    const std::string pollutedField = "sZWu 1\x17" "x";
    failures += ExpectError(
        BattleHelpModeStatus::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "sZWu",
        true,
        1U,
        "sZWu polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = BattleHelpModeStatus(1).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        BattleHelpModeStatus::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "sZWu FromPacket wrong code");

    cg::packet::Packet wrongDirection = BattleHelpModeStatus(1).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        BattleHelpModeStatus::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "sZWu",
        false,
        0U,
        "sZWu FromPacket wrong direction");

    cg::packet::Packet missingField(
        cg::packet::PacketCode("sZWu"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        BattleHelpModeStatus::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "sZWu",
        true,
        0U,
        "sZWu FromPacket missing field");

    cg::packet::Packet extraField = BattleHelpModeStatus(1).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "0",
    });
    failures += ExpectError(
        BattleHelpModeStatus::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "sZWu",
        true,
        1U,
        "sZWu FromPacket extra field");

    cg::packet::Packet wrongName = BattleHelpModeStatus(1).ToPacket();
    wrongName.fields[0].name = "wrong";
    failures += ExpectError(
        BattleHelpModeStatus::FromPacket(wrongName),
        cg::packet::PacketErrorCode::DecodeFailed,
        "sZWu",
        true,
        0U,
        "sZWu FromPacket wrong name");

    cg::packet::Packet wrongType = BattleHelpModeStatus(1).ToPacket();
    wrongType.fields[0].type = cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        BattleHelpModeStatus::FromPacket(wrongType),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "sZWu",
        true,
        0U,
        "sZWu FromPacket wrong type");

    cg::packet::Packet invalidValue = BattleHelpModeStatus(1).ToPacket();
    invalidValue.fields[0].value = "not-an-int";
    failures += ExpectError(
        BattleHelpModeStatus::FromPacket(invalidValue),
        cg::packet::PacketErrorCode::DecodeFailed,
        "sZWu",
        true,
        0U,
        "sZWu FromPacket invalid decimal");

    cg::packet::Packet positiveOverflow = BattleHelpModeStatus(1).ToPacket();
    positiveOverflow.fields[0].value = "2147483648";
    failures += ExpectError(
        BattleHelpModeStatus::FromPacket(positiveOverflow),
        cg::packet::PacketErrorCode::DecodeFailed,
        "sZWu",
        true,
        0U,
        "sZWu FromPacket positive decimal overflow");

    cg::packet::Packet negativeOverflow = BattleHelpModeStatus(1).ToPacket();
    negativeOverflow.fields[0].value = "-2147483649";
    failures += ExpectError(
        BattleHelpModeStatus::FromPacket(negativeOverflow),
        cg::packet::PacketErrorCode::DecodeFailed,
        "sZWu",
        true,
        0U,
        "sZWu FromPacket negative decimal overflow");
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
        "code=sZWu\n"
        "direction=server_to_client\n"
        "sample=help-flag-one\n"
        "plain_hex=73 5A 57 75 20 31\n"
        "canonical_hex=73 5A 57 75 20 31 20\n"
        "end_sample\n";

    const std::string fixture = ReadFixture(path);
    int failures = 0;
    failures += Expect(fixture == Expected, "sZWu fixture exact bytes");

    std::size_t lineFeeds = 0U;
    bool asciiOnly = true;
    bool physicalBytesClean = true;
    for (const unsigned char byte : fixture) {
        lineFeeds += byte == 0x0AU ? 1U : 0U;
        asciiOnly = asciiOnly && byte <= 0x7FU;
        physicalBytesClean =
            physicalBytesClean && byte != 0x00U && byte != 0x0DU && byte != 0x17U;
    }
    failures += Expect(asciiOnly, "sZWu fixture ASCII");
    failures += Expect(physicalBytesClean, "sZWu fixture physical bytes");
    failures += Expect(lineFeeds == 7U, "sZWu fixture LF count");
    return failures;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "usage: cg-packet-battle-help-mode-status-test <sZWu-fixture>\n";
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
                  << " battle help mode status packet test(s) failed\n";
        return 1;
    }
    return 0;
}
