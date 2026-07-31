#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::TradeCounterpartyOfferChanged;

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
        packet.code == cg::packet::PacketCode("NdM"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ServerToClient,
        message);
    failures += Expect(packet.fields.empty(), message);
    return failures;
}

int TestSpecAndToPacket()
{
    const TradeCounterpartyOfferChanged value;
    const cg::packet::Packet packet = value.ToPacket();

    int failures = 0;
    failures += Expect(
        TradeCounterpartyOfferChanged::Code == "NdM",
        "NdM code");
    failures += Expect(
        TradeCounterpartyOfferChanged::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "NdM direction");
    failures += Expect(
        TradeCounterpartyOfferChanged::Fields.empty(),
        "NdM fields");
    failures += Expect(
        TradeCounterpartyOfferChanged::Spec.code ==
            TradeCounterpartyOfferChanged::Code,
        "NdM Spec code");
    failures += Expect(
        TradeCounterpartyOfferChanged::Spec.direction ==
            TradeCounterpartyOfferChanged::Direction,
        "NdM Spec direction");
    failures += Expect(
        TradeCounterpartyOfferChanged::Spec.fieldCount == 0U,
        "NdM Spec field count");
    failures += ExpectPacket(packet, "NdM ToPacket");

    const cg::packet::PacketResult<TradeCounterpartyOfferChanged> converted =
        TradeCounterpartyOfferChanged::FromPacket(packet);
    failures += Expect(converted.HasValue(), "NdM FromPacket valid");
    return failures;
}

int TestTypedAndGenericCanonical()
{
    int failures = 0;
    for (const std::string_view input : {"NdM", "NdM "}) {
        const cg::packet::PacketResult<TradeCounterpartyOfferChanged> typed =
            TradeCounterpartyOfferChanged::Parse(input);
        failures += Expect(typed.HasValue(), "NdM typed parse");
        if (typed.HasValue()) {
            failures += ExpectPacket(
                typed.Value().ToPacket(),
                "NdM typed packet");
            const cg::packet::PacketResult<std::string> built =
                cg::packet::BuildPacket(
                    typed.Value().ToPacket(),
                    TradeCounterpartyOfferChanged::Spec);
            failures += Expect(built.HasValue(), "NdM typed build");
            if (built.HasValue()) {
                failures += Expect(
                    built.Value() == "NdM ",
                    "NdM typed canonical");
            }
        }

        const cg::packet::PacketResult<cg::packet::Packet> generic =
            cg::packet::ParsePacket(
                input,
                TradeCounterpartyOfferChanged::Spec);
        failures += Expect(generic.HasValue(), "NdM generic parse");
        if (!generic.HasValue()) {
            continue;
        }
        failures += ExpectPacket(generic.Value(), "NdM generic packet");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(
                generic.Value(),
                TradeCounterpartyOfferChanged::Spec);
        failures += Expect(rebuilt.HasValue(), "NdM generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(
                rebuilt.Value() == "NdM ",
                "NdM generic canonical");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        TradeCounterpartyOfferChanged::Parse("bad"),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "NdM typed wrong code");
    failures += ExpectError(
        TradeCounterpartyOfferChanged::Parse("NdM 1"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "NdM",
        true,
        0U,
        "NdM typed extra field");

    const std::string pollutedCode = "NdM\x17" "x";
    failures += ExpectError(
        TradeCounterpartyOfferChanged::Parse(pollutedCode),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "NdM polluted code");

    const std::string pollutedField = "NdM \x17" "x";
    failures += ExpectError(
        TradeCounterpartyOfferChanged::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "NdM",
        true,
        0U,
        "NdM polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    const cg::packet::Packet wrongCode(
        cg::packet::PacketCode("bad"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        TradeCounterpartyOfferChanged::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "NdM FromPacket wrong code");

    const cg::packet::Packet wrongDirection(
        cg::packet::PacketCode("NdM"),
        cg::packet::PacketDirection::ClientToServer);
    failures += ExpectError(
        TradeCounterpartyOfferChanged::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "NdM",
        false,
        0U,
        "NdM FromPacket wrong direction");

    cg::packet::Packet extraField(
        cg::packet::PacketCode("NdM"),
        cg::packet::PacketDirection::ServerToClient);
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        std::string("1"),
    });
    failures += ExpectError(
        TradeCounterpartyOfferChanged::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "NdM",
        true,
        0U,
        "NdM FromPacket extra field");
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
        "code=NdM\n"
        "direction=server_to_client\n"
        "sample=counterparty-offer-changed\n"
        "plain_hex=4E 64 4D\n"
        "canonical_hex=4E 64 4D 20\n"
        "end_sample\n";

    const std::string fixture = ReadFixture(path);
    int failures = 0;
    failures += Expect(fixture == Expected, "NdM fixture exact bytes");

    std::size_t lineFeeds = 0U;
    bool asciiOnly = true;
    bool physicalBytesClean = true;
    for (const unsigned char byte : fixture) {
        lineFeeds += byte == 0x0AU ? 1U : 0U;
        asciiOnly = asciiOnly && byte <= 0x7FU;
        physicalBytesClean =
            physicalBytesClean && byte != 0x00U && byte != 0x0DU && byte != 0x17U;
    }
    failures += Expect(asciiOnly, "NdM fixture ASCII");
    failures += Expect(physicalBytesClean, "NdM fixture physical bytes");
    failures += Expect(lineFeeds == 7U, "NdM fixture LF count");
    return failures;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr
            << "usage: cg-packet-trade-counterparty-offer-changed-test "
               "<NdM-fixture>\n";
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
                  << " trade counterparty offer changed packet test(s) failed\n";
        return 1;
    }
    return 0;
}
