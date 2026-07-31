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

using cg::packet::receive::TradeGoldInfo;

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
    failures += Expect(packet.code == cg::packet::PacketCode("hJq"), message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ServerToClient,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "gold", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == expected, message);
    }
    return failures;
}

int TestSpecAndRoundtrip()
{
    const TradeGoldInfo value(100);
    const cg::packet::Packet packet = value.ToPacket();

    int failures = 0;
    failures += Expect(TradeGoldInfo::Code == "hJq", "hJq code");
    failures += Expect(
        TradeGoldInfo::Direction == cg::packet::PacketDirection::ServerToClient,
        "hJq direction");
    failures += Expect(TradeGoldInfo::Fields.size() == 1U, "hJq fields");
    failures += Expect(
        TradeGoldInfo::Fields[0].name == "gold",
        "hJq field name");
    failures += Expect(
        TradeGoldInfo::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "hJq field type");
    failures += Expect(
        TradeGoldInfo::Spec.code == TradeGoldInfo::Code,
        "hJq Spec code");
    failures += Expect(
        TradeGoldInfo::Spec.direction == TradeGoldInfo::Direction,
        "hJq Spec direction");
    failures += Expect(
        TradeGoldInfo::Spec.fieldCount == 1U,
        "hJq Spec field count");
    failures += Expect(value.gold == 100, "hJq member");
    failures += ExpectPacket(packet, "100", "hJq ToPacket");

    const cg::packet::PacketResult<TradeGoldInfo> converted =
        TradeGoldInfo::FromPacket(packet);
    failures += Expect(converted.HasValue(), "hJq FromPacket valid");
    if (converted.HasValue()) {
        failures += Expect(converted.Value().gold == 100, "hJq FromPacket value");
    }

    const cg::packet::PacketResult<TradeGoldInfo> typed =
        TradeGoldInfo::Parse("hJq 1C");
    failures += Expect(typed.HasValue(), "hJq typed parse");
    if (typed.HasValue()) {
        failures += Expect(typed.Value().gold == 100, "hJq 1C equals 100");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("hJq 1C", TradeGoldInfo::Spec);
    failures += Expect(generic.HasValue(), "hJq generic parse");
    if (generic.HasValue()) {
        failures += ExpectPacket(generic.Value(), "100", "hJq generic packet");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(generic.Value(), TradeGoldInfo::Spec);
        failures += Expect(rebuilt.HasValue(), "hJq generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(rebuilt.Value() == "hJq 1C ", "hJq canonical");
        }
    }
    return failures;
}

int TestSignedValues()
{
    struct Case
    {
        int gold;
        std::string_view canonical;
    };

    constexpr Case Cases[] = {
        {100, "hJq 1C "},
        {-1, "hJq -1 "},
        {std::numeric_limits<int>::min(), "hJq -2lkCB2 "},
        {std::numeric_limits<int>::max(), "hJq 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const TradeGoldInfo value(testCase.gold);
        failures += ExpectPacket(
            value.ToPacket(),
            std::to_string(testCase.gold),
            "hJq signed packet");

        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(value.ToPacket(), TradeGoldInfo::Spec);
        failures += Expect(built.HasValue(), "hJq signed build");
        if (built.HasValue()) {
            failures += Expect(
                built.Value() == testCase.canonical,
                "hJq signed canonical");
        }

        const cg::packet::PacketResult<TradeGoldInfo> parsed =
            TradeGoldInfo::Parse(testCase.canonical);
        failures += Expect(parsed.HasValue(), "hJq signed parse");
        if (parsed.HasValue()) {
            failures += Expect(
                parsed.Value().gold == testCase.gold,
                "hJq signed value");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        TradeGoldInfo::Parse("bad 1C"),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "hJq wrong code");
    failures += ExpectError(
        TradeGoldInfo::Parse("hJq"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "hJq",
        true,
        0U,
        "hJq missing field");
    failures += ExpectError(
        TradeGoldInfo::Parse("hJq 1C 1"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "hJq",
        true,
        1U,
        "hJq extra field");
    failures += ExpectError(
        TradeGoldInfo::Parse("hJq _"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "hJq",
        true,
        0U,
        "hJq invalid i62");
    failures += ExpectError(
        TradeGoldInfo::Parse("hJq 2lkCB2"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "hJq",
        true,
        0U,
        "hJq positive overflow");
    failures += ExpectError(
        TradeGoldInfo::Parse("hJq -2lkCB3"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "hJq",
        true,
        0U,
        "hJq negative overflow");

    const std::string pollutedCode = "hJq\x17" "x";
    failures += ExpectError(
        TradeGoldInfo::Parse(pollutedCode + " 1C"),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "hJq polluted code");

    const std::string pollutedField = "hJq 1C\x17" "x";
    failures += ExpectError(
        TradeGoldInfo::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "hJq",
        true,
        1U,
        "hJq polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = TradeGoldInfo(100).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        TradeGoldInfo::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "hJq FromPacket wrong code");

    cg::packet::Packet wrongDirection = TradeGoldInfo(100).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        TradeGoldInfo::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "hJq",
        false,
        0U,
        "hJq FromPacket wrong direction");

    cg::packet::Packet missingField(
        cg::packet::PacketCode("hJq"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        TradeGoldInfo::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "hJq",
        true,
        0U,
        "hJq FromPacket missing field");

    cg::packet::Packet extraField = TradeGoldInfo(100).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "0",
    });
    failures += ExpectError(
        TradeGoldInfo::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "hJq",
        true,
        1U,
        "hJq FromPacket extra field");

    cg::packet::Packet wrongName = TradeGoldInfo(100).ToPacket();
    wrongName.fields[0].name = "wrong";
    failures += ExpectError(
        TradeGoldInfo::FromPacket(wrongName),
        cg::packet::PacketErrorCode::DecodeFailed,
        "hJq",
        true,
        0U,
        "hJq FromPacket wrong name");

    cg::packet::Packet wrongType = TradeGoldInfo(100).ToPacket();
    wrongType.fields[0].type = cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        TradeGoldInfo::FromPacket(wrongType),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "hJq",
        true,
        0U,
        "hJq FromPacket wrong type");

    cg::packet::Packet invalidValue = TradeGoldInfo(100).ToPacket();
    invalidValue.fields[0].value = "not-an-int";
    failures += ExpectError(
        TradeGoldInfo::FromPacket(invalidValue),
        cg::packet::PacketErrorCode::DecodeFailed,
        "hJq",
        true,
        0U,
        "hJq FromPacket invalid decimal");

    cg::packet::Packet positiveOverflow = TradeGoldInfo(100).ToPacket();
    positiveOverflow.fields[0].value = "2147483648";
    failures += ExpectError(
        TradeGoldInfo::FromPacket(positiveOverflow),
        cg::packet::PacketErrorCode::DecodeFailed,
        "hJq",
        true,
        0U,
        "hJq FromPacket positive decimal overflow");

    cg::packet::Packet negativeOverflow = TradeGoldInfo(100).ToPacket();
    negativeOverflow.fields[0].value = "-2147483649";
    failures += ExpectError(
        TradeGoldInfo::FromPacket(negativeOverflow),
        cg::packet::PacketErrorCode::DecodeFailed,
        "hJq",
        true,
        0U,
        "hJq FromPacket negative decimal overflow");
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
        "code=hJq\n"
        "direction=server_to_client\n"
        "sample=counterparty-gold-one-hundred\n"
        "plain_hex=68 4A 71 20 31 43\n"
        "canonical_hex=68 4A 71 20 31 43 20\n"
        "end_sample\n";

    const std::string fixture = ReadFixture(path);
    int failures = 0;
    failures += Expect(fixture == Expected, "hJq fixture exact bytes");

    std::size_t lineFeeds = 0U;
    bool asciiOnly = true;
    bool physicalBytesClean = true;
    for (const unsigned char byte : fixture) {
        lineFeeds += byte == 0x0AU ? 1U : 0U;
        asciiOnly = asciiOnly && byte <= 0x7FU;
        physicalBytesClean =
            physicalBytesClean && byte != 0x00U && byte != 0x0DU && byte != 0x17U;
    }
    failures += Expect(asciiOnly, "hJq fixture ASCII");
    failures += Expect(physicalBytesClean, "hJq fixture physical bytes");
    failures += Expect(lineFeeds == 7U, "hJq fixture LF count");
    return failures;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr << "usage: cg-packet-trade-gold-info-test <hJq-fixture>\n";
        return 1;
    }

    const int failures =
        TestSpecAndRoundtrip()
        + TestSignedValues()
        + TestParseErrors()
        + TestFromPacketErrors()
        + TestFixturePhysical(argv[1]);
    if (failures != 0) {
        std::cerr << failures << " trade gold info packet test(s) failed\n";
        return 1;
    }
    return 0;
}
