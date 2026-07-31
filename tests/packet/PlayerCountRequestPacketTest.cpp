#include "packet/Send.hpp"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::PlayerCountRequest;

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

int TestSpecToPacketAndEncode()
{
    const PlayerCountRequest request;
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(PlayerCountRequest::Code == "rvb", "rvb code");
    failures += Expect(
        PlayerCountRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "rvb direction");
    failures += Expect(PlayerCountRequest::Fields.empty(), "rvb fields");
    failures += Expect(
        PlayerCountRequest::Spec.code == PlayerCountRequest::Code,
        "rvb Spec code");
    failures += Expect(
        PlayerCountRequest::Spec.direction == PlayerCountRequest::Direction,
        "rvb Spec direction");
    failures += Expect(
        PlayerCountRequest::Spec.fieldCount == 0U,
        "rvb Spec field count");
    failures += Expect(
        packet.code == cg::packet::PacketCode("rvb"),
        "rvb packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "rvb packet direction");
    failures += Expect(packet.fields.empty(), "rvb packet fields");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "rvb Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "rvb ", "rvb canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> plain =
        cg::packet::ParsePacket("rvb", PlayerCountRequest::Spec);

    int failures = Expect(plain.HasValue(), "rvb plain ParsePacket");
    if (!plain.HasValue()) {
        return failures;
    }
    failures += Expect(plain.Value().fields.empty(), "rvb parsed fields");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(plain.Value(), PlayerCountRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "rvb BuildPacket");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "rvb ", "rvb generic canonical");
    }

    const cg::packet::PacketResult<cg::packet::Packet> canonical =
        cg::packet::ParsePacket("rvb ", PlayerCountRequest::Spec);
    failures += Expect(canonical.HasValue(), "rvb canonical ParsePacket");
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad", PlayerCountRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "rvb wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("rvb 1", PlayerCountRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "rvb",
        true,
        0U,
        "rvb extra field");

    cg::packet::Packet wrongDirection = PlayerCountRequest {}.ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongDirection, PlayerCountRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "rvb",
        false,
        0U,
        "rvb Build wrong direction");

    const std::string pollutedCode = "rvb\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(pollutedCode, PlayerCountRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "rvb polluted code");

    const std::string pollutedField = "rvb \x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(pollutedField, PlayerCountRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "rvb",
        true,
        0U,
        "rvb polluted field");
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
        "code=rvb\n"
        "direction=client_to_server\n"
        "sample=player-count-request\n"
        "plain_hex=72 76 62\n"
        "canonical_hex=72 76 62 20\n"
        "end_sample\n";

    const std::string fixture = ReadFixture(path);
    int failures = 0;
    failures += Expect(fixture == Expected, "rvb fixture exact bytes");

    std::size_t lineFeeds = 0U;
    bool asciiOnly = true;
    bool physicalBytesClean = true;
    for (const unsigned char byte : fixture) {
        lineFeeds += byte == 0x0AU ? 1U : 0U;
        asciiOnly = asciiOnly && byte <= 0x7FU;
        physicalBytesClean =
            physicalBytesClean && byte != 0x00U && byte != 0x0DU && byte != 0x17U;
    }
    failures += Expect(asciiOnly, "rvb fixture ASCII");
    failures += Expect(physicalBytesClean, "rvb fixture physical bytes");
    failures += Expect(lineFeeds == 7U, "rvb fixture LF count");
    return failures;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "usage: cg-packet-player-count-request-test <rvb-fixture>\n";
        return 1;
    }

    const int failures =
        TestSpecToPacketAndEncode()
        + TestGenericRoundtrip()
        + TestErrors()
        + TestFixturePhysical(argv[1]);
    if (failures != 0) {
        std::cerr << failures
                  << " player count request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
