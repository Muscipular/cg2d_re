#include "packet/Send.hpp"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::BattleHelpModeRequest;

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
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode("FVm"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "flg", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == value, message);
    }
    return failures;
}

int TestSpecToPacketAndEncode()
{
    const BattleHelpModeRequest request(1);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(BattleHelpModeRequest::Code == "FVm", "FVm code");
    failures += Expect(
        BattleHelpModeRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "FVm direction");
    failures += Expect(BattleHelpModeRequest::Fields.size() == 1U, "FVm fields");
    failures += Expect(
        BattleHelpModeRequest::Fields[0].name == "flg",
        "FVm field name");
    failures += Expect(
        BattleHelpModeRequest::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "FVm field type");
    failures += Expect(
        BattleHelpModeRequest::Spec.code == BattleHelpModeRequest::Code,
        "FVm Spec code");
    failures += Expect(
        BattleHelpModeRequest::Spec.direction == BattleHelpModeRequest::Direction,
        "FVm Spec direction");
    failures += Expect(
        BattleHelpModeRequest::Spec.fieldCount == 1U,
        "FVm Spec field count");
    failures += Expect(request.helpFlag == 1, "FVm member");
    failures += ExpectPacket(packet, "1", "FVm ToPacket");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "FVm Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "FVm 1 ", "FVm canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("FVm 1", BattleHelpModeRequest::Spec);

    int failures = Expect(parsed.HasValue(), "FVm generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(parsed.Value(), "1", "FVm generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), BattleHelpModeRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "FVm generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "FVm 1 ", "FVm generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int helpFlag;
        std::string_view canonical;
    };

    constexpr Case Cases[] = {
        {0, "FVm 0 "},
        {1, "FVm 1 "},
        {10, "FVm a "},
        {-1, "FVm -1 "},
        {-10, "FVm -a "},
        {std::numeric_limits<int>::min(), "FVm -2lkCB2 "},
        {std::numeric_limits<int>::max(), "FVm 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const BattleHelpModeRequest request(testCase.helpFlag);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.helpFlag),
            "FVm signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "FVm signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.canonical,
                "FVm signed canonical");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad 1", BattleHelpModeRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "FVm wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("FVm", BattleHelpModeRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "FVm",
        true,
        0U,
        "FVm missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("FVm 1 0", BattleHelpModeRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "FVm",
        true,
        1U,
        "FVm extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("FVm _", BattleHelpModeRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "FVm",
        true,
        0U,
        "FVm invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("FVm 2lkCB2", BattleHelpModeRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "FVm",
        true,
        0U,
        "FVm positive overflow");
    failures += ExpectError(
        cg::packet::ParsePacket("FVm -2lkCB3", BattleHelpModeRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "FVm",
        true,
        0U,
        "FVm negative overflow");

    const std::string pollutedCode = "FVm\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedCode + " 1",
            BattleHelpModeRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "FVm polluted code");

    const std::string pollutedField = "FVm 1\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(pollutedField, BattleHelpModeRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "FVm",
        true,
        1U,
        "FVm polluted field");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = BattleHelpModeRequest(1).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        cg::packet::BuildPacket(wrongCode, BattleHelpModeRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "FVm Build wrong code");

    cg::packet::Packet wrongDirection = BattleHelpModeRequest(1).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongDirection, BattleHelpModeRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "FVm",
        false,
        0U,
        "FVm Build wrong direction");
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
        "code=FVm\n"
        "direction=client_to_server\n"
        "sample=help-flag-one\n"
        "plain_hex=46 56 6D 20 31\n"
        "canonical_hex=46 56 6D 20 31 20\n"
        "end_sample\n";

    const std::string fixture = ReadFixture(path);
    int failures = 0;
    failures += Expect(fixture == Expected, "FVm fixture exact bytes");

    std::size_t lineFeeds = 0U;
    bool asciiOnly = true;
    bool physicalBytesClean = true;
    for (const unsigned char byte : fixture) {
        lineFeeds += byte == 0x0AU ? 1U : 0U;
        asciiOnly = asciiOnly && byte <= 0x7FU;
        physicalBytesClean =
            physicalBytesClean && byte != 0x00U && byte != 0x0DU && byte != 0x17U;
    }
    failures += Expect(asciiOnly, "FVm fixture ASCII");
    failures += Expect(physicalBytesClean, "FVm fixture physical bytes");
    failures += Expect(lineFeeds == 7U, "FVm fixture LF count");
    return failures;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "usage: cg-packet-battle-help-mode-request-test <FVm-fixture>\n";
        return 1;
    }

    const int failures =
        TestSpecToPacketAndEncode()
        + TestGenericRoundtrip()
        + TestSignedAndBoundaryValues()
        + TestParseErrors()
        + TestBuildErrors()
        + TestFixturePhysical(argv[1]);
    if (failures != 0) {
        std::cerr << failures
                  << " battle help mode request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
