#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::TradeSelectionCancelNotification;

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

int ExpectPacket(const cg::packet::Packet& packet, const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode("xEg"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ServerToClient,
        message);
    failures += Expect(packet.fields.empty(), message);
    return failures;
}

int TestSpecAndToPacket()
{
    const TradeSelectionCancelNotification value;
    const cg::packet::Packet packet = value.ToPacket();

    int failures = 0;
    failures += Expect(
        TradeSelectionCancelNotification::Code == "xEg",
        "xEg code");
    failures += Expect(
        TradeSelectionCancelNotification::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "xEg direction");
    failures += Expect(
        TradeSelectionCancelNotification::Fields.empty(),
        "xEg fields");
    failures += Expect(
        TradeSelectionCancelNotification::Spec.code ==
            TradeSelectionCancelNotification::Code,
        "xEg Spec code");
    failures += Expect(
        TradeSelectionCancelNotification::Spec.direction ==
            TradeSelectionCancelNotification::Direction,
        "xEg Spec direction");
    failures += Expect(
        TradeSelectionCancelNotification::Spec.fieldCount == 0U,
        "xEg Spec field count");
    failures += ExpectPacket(packet, "xEg ToPacket");

    const cg::packet::PacketResult<TradeSelectionCancelNotification> converted =
        TradeSelectionCancelNotification::FromPacket(packet);
    failures += Expect(converted.HasValue(), "xEg FromPacket valid");
    return failures;
}

int TestTypedAndGenericCanonical()
{
    int failures = 0;
    for (const std::string_view input : {"xEg", "xEg "}) {
        const cg::packet::PacketResult<TradeSelectionCancelNotification> typed =
            TradeSelectionCancelNotification::Parse(input);
        failures += Expect(typed.HasValue(), "xEg typed parse");
        if (typed.HasValue()) {
            failures += ExpectPacket(
                typed.Value().ToPacket(),
                "xEg typed packet");
            const cg::packet::PacketResult<std::string> built =
                cg::packet::BuildPacket(
                    typed.Value().ToPacket(),
                    TradeSelectionCancelNotification::Spec);
            failures += Expect(built.HasValue(), "xEg typed build");
            if (built.HasValue()) {
                failures += Expect(
                    built.Value() == "xEg ",
                    "xEg typed canonical");
            }
        }

        const cg::packet::PacketResult<cg::packet::Packet> generic =
            cg::packet::ParsePacket(
                input,
                TradeSelectionCancelNotification::Spec);
        failures += Expect(generic.HasValue(), "xEg generic parse");
        if (!generic.HasValue()) {
            continue;
        }
        failures += ExpectPacket(generic.Value(), "xEg generic packet");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(
                generic.Value(),
                TradeSelectionCancelNotification::Spec);
        failures += Expect(rebuilt.HasValue(), "xEg generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(
                rebuilt.Value() == "xEg ",
                "xEg generic canonical");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        TradeSelectionCancelNotification::Parse("bad"),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "xEg typed wrong code");
    failures += ExpectError(
        TradeSelectionCancelNotification::Parse("xEg 1"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "xEg",
        true,
        0U,
        "xEg typed extra field");

    const std::string pollutedCode = "xEg\x17" "x";
    failures += ExpectError(
        TradeSelectionCancelNotification::Parse(pollutedCode),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "xEg polluted code");

    const std::string pollutedField = "xEg \x17" "x";
    failures += ExpectError(
        TradeSelectionCancelNotification::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "xEg",
        true,
        0U,
        "xEg polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    const cg::packet::Packet wrongCode(
        cg::packet::PacketCode("bad"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        TradeSelectionCancelNotification::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "xEg FromPacket wrong code");

    const cg::packet::Packet wrongDirection(
        cg::packet::PacketCode("xEg"),
        cg::packet::PacketDirection::ClientToServer);
    failures += ExpectError(
        TradeSelectionCancelNotification::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "xEg",
        false,
        0U,
        "xEg FromPacket wrong direction");

    cg::packet::Packet extraField(
        cg::packet::PacketCode("xEg"),
        cg::packet::PacketDirection::ServerToClient);
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        std::string("1"),
    });
    failures += ExpectError(
        TradeSelectionCancelNotification::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "xEg",
        true,
        0U,
        "xEg FromPacket extra field");
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
        "code=xEg\n"
        "direction=server_to_client\n"
        "sample=trade-selection-cancelled\n"
        "plain_hex=78 45 67\n"
        "canonical_hex=78 45 67 20\n"
        "end_sample\n";

    const std::string fixture = ReadFixture(path);
    int failures = 0;
    failures += Expect(fixture == Expected, "xEg fixture exact bytes");

    std::size_t lineFeeds = 0U;
    bool asciiOnly = true;
    bool physicalBytesClean = true;
    for (const unsigned char byte : fixture) {
        lineFeeds += byte == 0x0AU ? 1U : 0U;
        asciiOnly = asciiOnly && byte <= 0x7FU;
        physicalBytesClean =
            physicalBytesClean && byte != 0x00U && byte != 0x0DU && byte != 0x17U;
    }
    failures += Expect(asciiOnly, "xEg fixture ASCII");
    failures += Expect(physicalBytesClean, "xEg fixture physical bytes");
    failures += Expect(lineFeeds == 7U, "xEg fixture LF count");
    return failures;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr
            << "usage: cg-packet-trade-selection-cancel-notification-test "
               "<xEg-fixture>\n";
        return 1;
    }

    const int failures =
        TestSpecAndToPacket()
        + TestTypedAndGenericCanonical()
        + TestParseErrors()
        + TestFromPacketErrors()
        + TestFixturePhysical(argv[1]);
    if (failures != 0) {
        std::cerr << failures
                  << " trade selection cancel notification packet test(s) failed\n";
        return 1;
    }
    return 0;
}
