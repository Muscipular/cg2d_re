#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::SoundEffectNotification;

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
    failures += Expect(packet.code == cg::packet::PacketCode("TFCu"), message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ServerToClient,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "senumber", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == expected, message);
    }
    return failures;
}

int TestSpecAndRoundtrip()
{
    const SoundEffectNotification value(2);
    const cg::packet::Packet packet = value.ToPacket();

    int failures = 0;
    failures += Expect(
        SoundEffectNotification::Code == "TFCu",
        "TFCu code");
    failures += Expect(
        SoundEffectNotification::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "TFCu direction");
    failures += Expect(
        SoundEffectNotification::Fields.size() == 1U,
        "TFCu fields");
    failures += Expect(
        SoundEffectNotification::Fields[0].name == "senumber",
        "TFCu field name");
    failures += Expect(
        SoundEffectNotification::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "TFCu field type");
    failures += Expect(
        SoundEffectNotification::Spec.code == SoundEffectNotification::Code,
        "TFCu Spec code");
    failures += Expect(
        SoundEffectNotification::Spec.direction ==
            SoundEffectNotification::Direction,
        "TFCu Spec direction");
    failures += Expect(
        SoundEffectNotification::Spec.fieldCount == 1U,
        "TFCu Spec field count");
    failures += Expect(value.soundNumber == 2, "TFCu member");
    failures += ExpectPacket(packet, "2", "TFCu ToPacket");

    const cg::packet::PacketResult<SoundEffectNotification> converted =
        SoundEffectNotification::FromPacket(packet);
    failures += Expect(converted.HasValue(), "TFCu FromPacket valid");
    if (converted.HasValue()) {
        failures += Expect(
            converted.Value().soundNumber == 2,
            "TFCu FromPacket value");
    }

    const cg::packet::PacketResult<SoundEffectNotification> typed =
        SoundEffectNotification::Parse("TFCu 2");
    failures += Expect(typed.HasValue(), "TFCu typed parse");
    if (typed.HasValue()) {
        failures += Expect(
            typed.Value().soundNumber == 2,
            "TFCu typed value");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("TFCu 2", SoundEffectNotification::Spec);
    failures += Expect(generic.HasValue(), "TFCu generic parse");
    if (generic.HasValue()) {
        failures += ExpectPacket(
            generic.Value(),
            "2",
            "TFCu generic packet");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(
                generic.Value(),
                SoundEffectNotification::Spec);
        failures += Expect(rebuilt.HasValue(), "TFCu generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(
                rebuilt.Value() == "TFCu 2 ",
                "TFCu canonical");
        }
    }
    return failures;
}

int TestDomain()
{
    constexpr int ValidValues[] = {0, 2, 8};

    int failures = 0;
    for (const int soundNumber : ValidValues) {
        const SoundEffectNotification value(soundNumber);
        failures += ExpectPacket(
            value.ToPacket(),
            std::to_string(soundNumber),
            "TFCu valid packet");

        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(
                value.ToPacket(),
                SoundEffectNotification::Spec);
        failures += Expect(built.HasValue(), "TFCu valid build");
        if (built.HasValue()) {
            const std::string expected =
                "TFCu " + std::to_string(soundNumber) + " ";
            failures += Expect(
                built.Value() == expected,
                "TFCu valid canonical");

            const cg::packet::PacketResult<SoundEffectNotification> parsed =
                SoundEffectNotification::Parse(expected);
            failures += Expect(parsed.HasValue(), "TFCu valid parse");
            if (parsed.HasValue()) {
                failures += Expect(
                    parsed.Value().soundNumber == soundNumber,
                    "TFCu valid value");
            }
        }
    }

    failures += ExpectError(
        SoundEffectNotification::Parse("TFCu -1"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TFCu",
        true,
        0U,
        "TFCu below domain");
    failures += ExpectError(
        SoundEffectNotification::Parse("TFCu 9"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TFCu",
        true,
        0U,
        "TFCu above domain");

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("TFCu -1", SoundEffectNotification::Spec);
    failures += Expect(generic.HasValue(), "TFCu generic signed parse");
    if (generic.HasValue()) {
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(
                generic.Value(),
                SoundEffectNotification::Spec);
        failures += Expect(rebuilt.HasValue(), "TFCu generic signed build");
        if (rebuilt.HasValue()) {
            failures += Expect(
                rebuilt.Value() == "TFCu -1 ",
                "TFCu generic signed canonical");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        SoundEffectNotification::Parse("bad 2"),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "TFCu wrong code");
    failures += ExpectError(
        SoundEffectNotification::Parse("TFCu"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TFCu",
        true,
        0U,
        "TFCu missing field");
    failures += ExpectError(
        SoundEffectNotification::Parse("TFCu 2 3"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TFCu",
        true,
        1U,
        "TFCu extra field");
    failures += ExpectError(
        SoundEffectNotification::Parse("TFCu _"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TFCu",
        true,
        0U,
        "TFCu invalid i62");
    failures += ExpectError(
        SoundEffectNotification::Parse("TFCu 2lkCB2"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TFCu",
        true,
        0U,
        "TFCu positive overflow");
    failures += ExpectError(
        SoundEffectNotification::Parse("TFCu -2lkCB3"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TFCu",
        true,
        0U,
        "TFCu negative overflow");

    const std::string pollutedCode = "TFCu\x17" "x";
    failures += ExpectError(
        SoundEffectNotification::Parse(pollutedCode + " 2"),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "TFCu polluted code");

    const std::string pollutedField = "TFCu 2\x17" "x";
    failures += ExpectError(
        SoundEffectNotification::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TFCu",
        true,
        1U,
        "TFCu polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = SoundEffectNotification(2).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        SoundEffectNotification::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "TFCu FromPacket wrong code");

    cg::packet::Packet wrongDirection =
        SoundEffectNotification(2).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        SoundEffectNotification::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TFCu",
        false,
        0U,
        "TFCu FromPacket wrong direction");

    cg::packet::Packet missingField(
        cg::packet::PacketCode("TFCu"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        SoundEffectNotification::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TFCu",
        true,
        0U,
        "TFCu FromPacket missing field");

    cg::packet::Packet extraField = SoundEffectNotification(2).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "0",
    });
    failures += ExpectError(
        SoundEffectNotification::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TFCu",
        true,
        1U,
        "TFCu FromPacket extra field");

    cg::packet::Packet wrongName = SoundEffectNotification(2).ToPacket();
    wrongName.fields[0].name = "wrong";
    failures += ExpectError(
        SoundEffectNotification::FromPacket(wrongName),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TFCu",
        true,
        0U,
        "TFCu FromPacket wrong name");

    cg::packet::Packet wrongType = SoundEffectNotification(2).ToPacket();
    wrongType.fields[0].type = cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        SoundEffectNotification::FromPacket(wrongType),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "TFCu",
        true,
        0U,
        "TFCu FromPacket wrong type");

    cg::packet::Packet invalidValue = SoundEffectNotification(2).ToPacket();
    invalidValue.fields[0].value = "not-an-int";
    failures += ExpectError(
        SoundEffectNotification::FromPacket(invalidValue),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TFCu",
        true,
        0U,
        "TFCu FromPacket invalid decimal");

    cg::packet::Packet belowDomain = SoundEffectNotification(2).ToPacket();
    belowDomain.fields[0].value = "-1";
    failures += ExpectError(
        SoundEffectNotification::FromPacket(belowDomain),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TFCu",
        true,
        0U,
        "TFCu FromPacket below domain");

    cg::packet::Packet aboveDomain = SoundEffectNotification(2).ToPacket();
    aboveDomain.fields[0].value = "9";
    failures += ExpectError(
        SoundEffectNotification::FromPacket(aboveDomain),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TFCu",
        true,
        0U,
        "TFCu FromPacket above domain");

    cg::packet::Packet positiveOverflow =
        SoundEffectNotification(2).ToPacket();
    positiveOverflow.fields[0].value = "2147483648";
    failures += ExpectError(
        SoundEffectNotification::FromPacket(positiveOverflow),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TFCu",
        true,
        0U,
        "TFCu FromPacket positive decimal overflow");

    cg::packet::Packet negativeOverflow =
        SoundEffectNotification(2).ToPacket();
    negativeOverflow.fields[0].value = "-2147483649";
    failures += ExpectError(
        SoundEffectNotification::FromPacket(negativeOverflow),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TFCu",
        true,
        0U,
        "TFCu FromPacket negative decimal overflow");
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
        "code=TFCu\n"
        "direction=server_to_client\n"
        "sample=pickup-item\n"
        "plain_hex=54 46 43 75 20 32\n"
        "canonical_hex=54 46 43 75 20 32 20\n"
        "end_sample\n";

    const std::string fixture = ReadFixture(path);
    int failures = 0;
    failures += Expect(fixture == Expected, "TFCu fixture exact bytes");

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
    failures += Expect(asciiOnly, "TFCu fixture ASCII");
    failures += Expect(
        physicalBytesClean,
        "TFCu fixture physical bytes");
    failures += Expect(lineFeeds == 7U, "TFCu fixture LF count");
    return failures;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr
            << "usage: cg-packet-sound-effect-notification-test "
               "<TFCu-fixture>\n";
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
                  << " sound effect notification packet test(s) failed\n";
        return 1;
    }
    return 0;
}
