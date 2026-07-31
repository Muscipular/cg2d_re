#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::BackgroundMusicChange;

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
    failures += Expect(packet.code == cg::packet::PacketCode("wiB"), message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ServerToClient,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "sw", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == expected, message);
    }
    return failures;
}

int TestSpecAndRoundtrip()
{
    const BackgroundMusicChange value(200);
    const cg::packet::Packet packet = value.ToPacket();

    int failures = 0;
    failures += Expect(BackgroundMusicChange::Code == "wiB", "wiB code");
    failures += Expect(
        BackgroundMusicChange::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "wiB direction");
    failures += Expect(
        BackgroundMusicChange::Fields.size() == 1U,
        "wiB fields");
    failures += Expect(
        BackgroundMusicChange::Fields[0].name == "sw",
        "wiB field name");
    failures += Expect(
        BackgroundMusicChange::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "wiB field type");
    failures += Expect(
        BackgroundMusicChange::Spec.code == BackgroundMusicChange::Code,
        "wiB Spec code");
    failures += Expect(
        BackgroundMusicChange::Spec.direction ==
            BackgroundMusicChange::Direction,
        "wiB Spec direction");
    failures += Expect(
        BackgroundMusicChange::Spec.fieldCount == 1U,
        "wiB Spec field count");
    failures += Expect(
        value.backgroundMusicNumber == 200,
        "wiB member");
    failures += ExpectPacket(packet, "200", "wiB ToPacket");

    const cg::packet::PacketResult<BackgroundMusicChange> converted =
        BackgroundMusicChange::FromPacket(packet);
    failures += Expect(converted.HasValue(), "wiB FromPacket valid");
    if (converted.HasValue()) {
        failures += Expect(
            converted.Value().backgroundMusicNumber == 200,
            "wiB FromPacket value");
    }

    const cg::packet::PacketResult<BackgroundMusicChange> typed =
        BackgroundMusicChange::Parse("wiB 3e");
    failures += Expect(typed.HasValue(), "wiB typed parse");
    if (typed.HasValue()) {
        failures += Expect(
            typed.Value().backgroundMusicNumber == 200,
            "wiB typed value");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("wiB 3e", BackgroundMusicChange::Spec);
    failures += Expect(generic.HasValue(), "wiB generic parse");
    if (generic.HasValue()) {
        failures += ExpectPacket(generic.Value(), "200", "wiB generic packet");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(
                generic.Value(),
                BackgroundMusicChange::Spec);
        failures += Expect(rebuilt.HasValue(), "wiB generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(
                rebuilt.Value() == "wiB 3e ",
                "wiB canonical");
        }
    }
    return failures;
}

int TestDomain()
{
    constexpr int ValidValues[] = {200, 249};

    int failures = 0;
    for (const int backgroundMusicNumber : ValidValues) {
        const BackgroundMusicChange value(backgroundMusicNumber);
        failures += ExpectPacket(
            value.ToPacket(),
            std::to_string(backgroundMusicNumber),
            "wiB valid packet");

        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(
                value.ToPacket(),
                BackgroundMusicChange::Spec);
        failures += Expect(built.HasValue(), "wiB valid build");
        if (built.HasValue()) {
            const cg::packet::PacketResult<BackgroundMusicChange> parsed =
                BackgroundMusicChange::Parse(built.Value());
            failures += Expect(parsed.HasValue(), "wiB valid parse");
            if (parsed.HasValue()) {
                failures += Expect(
                    parsed.Value().backgroundMusicNumber ==
                        backgroundMusicNumber,
                    "wiB valid value");
            }
        }
    }

    failures += ExpectError(
        BackgroundMusicChange::Parse("wiB 3d"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "wiB",
        true,
        0U,
        "wiB below domain");
    failures += ExpectError(
        BackgroundMusicChange::Parse("wiB 42"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "wiB",
        true,
        0U,
        "wiB above domain");

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("wiB -1", BackgroundMusicChange::Spec);
    failures += Expect(generic.HasValue(), "wiB generic signed parse");
    if (generic.HasValue()) {
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(
                generic.Value(),
                BackgroundMusicChange::Spec);
        failures += Expect(rebuilt.HasValue(), "wiB generic signed build");
        if (rebuilt.HasValue()) {
            failures += Expect(
                rebuilt.Value() == "wiB -1 ",
                "wiB generic signed canonical");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        BackgroundMusicChange::Parse("bad 3e"),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "wiB wrong code");
    failures += ExpectError(
        BackgroundMusicChange::Parse("TFCu 3e"),
        cg::packet::PacketErrorCode::InvalidCode,
        "TFCu",
        false,
        0U,
        "wiB TFCu isolation");
    failures += ExpectError(
        BackgroundMusicChange::Parse("bJg 3e 1"),
        cg::packet::PacketErrorCode::InvalidCode,
        "bJg",
        false,
        0U,
        "wiB bJg isolation");
    failures += ExpectError(
        BackgroundMusicChange::Parse("wiB"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "wiB",
        true,
        0U,
        "wiB missing field");
    failures += ExpectError(
        BackgroundMusicChange::Parse("wiB 3e 1"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "wiB",
        true,
        1U,
        "wiB extra field");
    failures += ExpectError(
        BackgroundMusicChange::Parse("wiB _"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "wiB",
        true,
        0U,
        "wiB invalid i62");
    failures += ExpectError(
        BackgroundMusicChange::Parse("wiB 2lkCB2"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "wiB",
        true,
        0U,
        "wiB positive overflow");
    failures += ExpectError(
        BackgroundMusicChange::Parse("wiB -2lkCB3"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "wiB",
        true,
        0U,
        "wiB negative overflow");

    const std::string pollutedCode = "wiB\x17" "x";
    failures += ExpectError(
        BackgroundMusicChange::Parse(pollutedCode + " 3e"),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "wiB polluted code");

    const std::string pollutedField = "wiB 3e\x17" "x";
    failures += ExpectError(
        BackgroundMusicChange::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "wiB",
        true,
        1U,
        "wiB polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = BackgroundMusicChange(200).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        BackgroundMusicChange::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "wiB FromPacket wrong code");

    cg::packet::Packet wrongDirection = BackgroundMusicChange(200).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        BackgroundMusicChange::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "wiB",
        false,
        0U,
        "wiB FromPacket wrong direction");

    cg::packet::Packet missingField(
        cg::packet::PacketCode("wiB"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        BackgroundMusicChange::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "wiB",
        true,
        0U,
        "wiB FromPacket missing field");

    cg::packet::Packet extraField = BackgroundMusicChange(200).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "0",
    });
    failures += ExpectError(
        BackgroundMusicChange::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "wiB",
        true,
        1U,
        "wiB FromPacket extra field");

    cg::packet::Packet wrongName = BackgroundMusicChange(200).ToPacket();
    wrongName.fields[0].name = "wrong";
    failures += ExpectError(
        BackgroundMusicChange::FromPacket(wrongName),
        cg::packet::PacketErrorCode::DecodeFailed,
        "wiB",
        true,
        0U,
        "wiB FromPacket wrong name");

    cg::packet::Packet wrongType = BackgroundMusicChange(200).ToPacket();
    wrongType.fields[0].type = cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        BackgroundMusicChange::FromPacket(wrongType),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "wiB",
        true,
        0U,
        "wiB FromPacket wrong type");

    cg::packet::Packet invalidValue = BackgroundMusicChange(200).ToPacket();
    invalidValue.fields[0].value = "not-an-int";
    failures += ExpectError(
        BackgroundMusicChange::FromPacket(invalidValue),
        cg::packet::PacketErrorCode::DecodeFailed,
        "wiB",
        true,
        0U,
        "wiB FromPacket invalid decimal");

    cg::packet::Packet belowDomain = BackgroundMusicChange(200).ToPacket();
    belowDomain.fields[0].value = "199";
    failures += ExpectError(
        BackgroundMusicChange::FromPacket(belowDomain),
        cg::packet::PacketErrorCode::DecodeFailed,
        "wiB",
        true,
        0U,
        "wiB FromPacket below domain");

    cg::packet::Packet aboveDomain = BackgroundMusicChange(200).ToPacket();
    aboveDomain.fields[0].value = "250";
    failures += ExpectError(
        BackgroundMusicChange::FromPacket(aboveDomain),
        cg::packet::PacketErrorCode::DecodeFailed,
        "wiB",
        true,
        0U,
        "wiB FromPacket above domain");

    cg::packet::Packet positiveOverflow =
        BackgroundMusicChange(200).ToPacket();
    positiveOverflow.fields[0].value = "2147483648";
    failures += ExpectError(
        BackgroundMusicChange::FromPacket(positiveOverflow),
        cg::packet::PacketErrorCode::DecodeFailed,
        "wiB",
        true,
        0U,
        "wiB FromPacket positive decimal overflow");

    cg::packet::Packet negativeOverflow =
        BackgroundMusicChange(200).ToPacket();
    negativeOverflow.fields[0].value = "-2147483649";
    failures += ExpectError(
        BackgroundMusicChange::FromPacket(negativeOverflow),
        cg::packet::PacketErrorCode::DecodeFailed,
        "wiB",
        true,
        0U,
        "wiB FromPacket negative decimal overflow");
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
        "code=wiB\n"
        "direction=server_to_client\n"
        "sample=background-music-lower-bound\n"
        "plain_hex=77 69 42 20 33 65\n"
        "canonical_hex=77 69 42 20 33 65 20\n"
        "end_sample\n";

    const std::string fixture = ReadFixture(path);
    int failures = 0;
    failures += Expect(fixture == Expected, "wiB fixture exact bytes");

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
    failures += Expect(asciiOnly, "wiB fixture ASCII");
    failures += Expect(physicalBytesClean, "wiB fixture physical bytes");
    failures += Expect(lineFeeds == 7U, "wiB fixture LF count");
    return failures;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr
            << "usage: cg-packet-background-music-change-test "
               "<wiB-fixture>\n";
        return 1;
    }

    const int failures =
        TestSpecAndRoundtrip()
        + TestDomain()
        + TestParseErrors()
        + TestFromPacketErrors()
        + TestFixturePhysical(argv[1]);
    if (failures != 0) {
        std::cerr << failures
                  << " background music change packet test(s) failed\n";
        return 1;
    }
    return 0;
}
