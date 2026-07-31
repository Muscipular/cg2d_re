#include "packet/Send.hpp"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::CharacterLookRequest;

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
        packet.code == cg::packet::PacketCode("nB"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "dir", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == value, message);
    }
    return failures;
}

int TestSpecToPacketAndEncode()
{
    const CharacterLookRequest request(0);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(CharacterLookRequest::Code == "nB", "nB code");
    failures += Expect(
        CharacterLookRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "nB direction");
    failures += Expect(CharacterLookRequest::Fields.size() == 1U, "nB fields");
    failures += Expect(
        CharacterLookRequest::Fields[0].name == "dir",
        "nB field name");
    failures += Expect(
        CharacterLookRequest::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "nB field type");
    failures += Expect(
        CharacterLookRequest::Spec.code == CharacterLookRequest::Code,
        "nB Spec code");
    failures += Expect(
        CharacterLookRequest::Spec.direction == CharacterLookRequest::Direction,
        "nB Spec direction");
    failures += Expect(
        CharacterLookRequest::Spec.fieldCount == 1U,
        "nB Spec field count");
    failures += Expect(request.direction == 0, "nB member");
    failures += ExpectPacket(packet, "0", "nB ToPacket");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "nB Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "nB 0 ", "nB canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("nB 0", CharacterLookRequest::Spec);

    int failures = Expect(parsed.HasValue(), "nB generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(parsed.Value(), "0", "nB generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), CharacterLookRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "nB generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "nB 0 ", "nB generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int direction;
        std::string_view canonical;
    };

    constexpr Case Cases[] = {
        {0, "nB 0 "},
        {1, "nB 1 "},
        {10, "nB a "},
        {-1, "nB -1 "},
        {-10, "nB -a "},
        {std::numeric_limits<int>::min(), "nB -2lkCB2 "},
        {std::numeric_limits<int>::max(), "nB 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const CharacterLookRequest request(testCase.direction);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.direction),
            "nB signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "nB signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.canonical,
                "nB signed canonical");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad 0", CharacterLookRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "nB wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("nB", CharacterLookRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nB",
        true,
        0U,
        "nB missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("nB 0 1", CharacterLookRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nB",
        true,
        1U,
        "nB extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("nB _", CharacterLookRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "nB",
        true,
        0U,
        "nB invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("nB 2lkCB2", CharacterLookRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "nB",
        true,
        0U,
        "nB positive overflow");
    failures += ExpectError(
        cg::packet::ParsePacket("nB -2lkCB3", CharacterLookRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "nB",
        true,
        0U,
        "nB negative overflow");

    const std::string pollutedCode = "nB\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedCode + " 0",
            CharacterLookRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "nB polluted code");

    const std::string pollutedField = "nB 0\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(pollutedField, CharacterLookRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nB",
        true,
        1U,
        "nB polluted field");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = CharacterLookRequest(0).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        cg::packet::BuildPacket(wrongCode, CharacterLookRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "nB Build wrong code");

    cg::packet::Packet wrongDirection = CharacterLookRequest(0).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongDirection, CharacterLookRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "nB",
        false,
        0U,
        "nB Build wrong direction");
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
        "code=nB\n"
        "direction=client_to_server\n"
        "sample=direction-zero\n"
        "plain_hex=6E 42 20 30\n"
        "canonical_hex=6E 42 20 30 20\n"
        "end_sample\n";

    const std::string fixture = ReadFixture(path);
    int failures = 0;
    failures += Expect(fixture == Expected, "nB fixture exact bytes");

    std::size_t lineFeeds = 0U;
    bool asciiOnly = true;
    bool physicalBytesClean = true;
    for (const unsigned char byte : fixture) {
        lineFeeds += byte == 0x0AU ? 1U : 0U;
        asciiOnly = asciiOnly && byte <= 0x7FU;
        physicalBytesClean =
            physicalBytesClean && byte != 0x00U && byte != 0x0DU && byte != 0x17U;
    }
    failures += Expect(asciiOnly, "nB fixture ASCII");
    failures += Expect(physicalBytesClean, "nB fixture physical bytes");
    failures += Expect(lineFeeds == 7U, "nB fixture LF count");
    return failures;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "usage: cg-packet-character-look-request-test <nB-fixture>\n";
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
                  << " character look request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
