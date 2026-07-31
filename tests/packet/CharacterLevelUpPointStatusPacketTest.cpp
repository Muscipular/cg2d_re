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

using cg::packet::receive::CharacterLevelUpPointStatus;

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
    failures += Expect(packet.code == cg::packet::PacketCode("kg"), message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ServerToClient,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "point", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == expected, message);
    }
    return failures;
}

int TestSpecAndRoundtrip()
{
    const CharacterLevelUpPointStatus value(100);
    const cg::packet::Packet packet = value.ToPacket();

    int failures = 0;
    failures += Expect(
        CharacterLevelUpPointStatus::Code == "kg",
        "kg code");
    failures += Expect(
        CharacterLevelUpPointStatus::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "kg direction");
    failures += Expect(
        CharacterLevelUpPointStatus::Fields.size() == 1U,
        "kg fields");
    failures += Expect(
        CharacterLevelUpPointStatus::Fields[0].name == "point",
        "kg field name");
    failures += Expect(
        CharacterLevelUpPointStatus::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "kg field type");
    failures += Expect(
        CharacterLevelUpPointStatus::Spec.code ==
            CharacterLevelUpPointStatus::Code,
        "kg Spec code");
    failures += Expect(
        CharacterLevelUpPointStatus::Spec.direction ==
            CharacterLevelUpPointStatus::Direction,
        "kg Spec direction");
    failures += Expect(
        CharacterLevelUpPointStatus::Spec.fieldCount == 1U,
        "kg Spec field count");
    failures += Expect(value.point == 100, "kg member");
    failures += ExpectPacket(packet, "100", "kg ToPacket");

    const cg::packet::PacketResult<CharacterLevelUpPointStatus> converted =
        CharacterLevelUpPointStatus::FromPacket(packet);
    failures += Expect(converted.HasValue(), "kg FromPacket valid");
    if (converted.HasValue()) {
        failures += Expect(
            converted.Value().point == 100,
            "kg FromPacket value");
    }

    const cg::packet::PacketResult<CharacterLevelUpPointStatus> typed =
        CharacterLevelUpPointStatus::Parse("kg 1C");
    failures += Expect(typed.HasValue(), "kg typed parse");
    if (typed.HasValue()) {
        failures += Expect(typed.Value().point == 100, "kg 1C equals 100");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("kg 1C", CharacterLevelUpPointStatus::Spec);
    failures += Expect(generic.HasValue(), "kg generic parse");
    if (generic.HasValue()) {
        failures += ExpectPacket(generic.Value(), "100", "kg generic packet");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(
                generic.Value(),
                CharacterLevelUpPointStatus::Spec);
        failures += Expect(rebuilt.HasValue(), "kg generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(rebuilt.Value() == "kg 1C ", "kg canonical");
        }
    }
    return failures;
}

int TestSignedValues()
{
    struct Case
    {
        int point;
        std::string_view canonical;
    };

    constexpr Case Cases[] = {
        {100, "kg 1C "},
        {-1, "kg -1 "},
        {std::numeric_limits<int>::min(), "kg -2lkCB2 "},
        {std::numeric_limits<int>::max(), "kg 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const CharacterLevelUpPointStatus value(testCase.point);
        failures += ExpectPacket(
            value.ToPacket(),
            std::to_string(testCase.point),
            "kg signed packet");

        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(
                value.ToPacket(),
                CharacterLevelUpPointStatus::Spec);
        failures += Expect(built.HasValue(), "kg signed build");
        if (built.HasValue()) {
            failures += Expect(
                built.Value() == testCase.canonical,
                "kg signed canonical");
        }

        const cg::packet::PacketResult<CharacterLevelUpPointStatus> parsed =
            CharacterLevelUpPointStatus::Parse(testCase.canonical);
        failures += Expect(parsed.HasValue(), "kg signed parse");
        if (parsed.HasValue()) {
            failures += Expect(
                parsed.Value().point == testCase.point,
                "kg signed value");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        CharacterLevelUpPointStatus::Parse("bad 1C"),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "kg wrong code");
    failures += ExpectError(
        CharacterLevelUpPointStatus::Parse("kg"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "kg",
        true,
        0U,
        "kg missing field");
    failures += ExpectError(
        CharacterLevelUpPointStatus::Parse("kg 1C 1"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "kg",
        true,
        1U,
        "kg extra field");
    failures += ExpectError(
        CharacterLevelUpPointStatus::Parse("kg _"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "kg",
        true,
        0U,
        "kg invalid i62");
    failures += ExpectError(
        CharacterLevelUpPointStatus::Parse("kg 2lkCB2"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "kg",
        true,
        0U,
        "kg positive overflow");
    failures += ExpectError(
        CharacterLevelUpPointStatus::Parse("kg -2lkCB3"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "kg",
        true,
        0U,
        "kg negative overflow");

    const std::string pollutedCode = "kg\x17" "x";
    failures += ExpectError(
        CharacterLevelUpPointStatus::Parse(pollutedCode + " 1C"),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "kg polluted code");

    const std::string pollutedField = "kg 1C\x17" "x";
    failures += ExpectError(
        CharacterLevelUpPointStatus::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "kg",
        true,
        1U,
        "kg polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = CharacterLevelUpPointStatus(100).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        CharacterLevelUpPointStatus::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "kg FromPacket wrong code");

    cg::packet::Packet wrongDirection =
        CharacterLevelUpPointStatus(100).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        CharacterLevelUpPointStatus::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "kg",
        false,
        0U,
        "kg FromPacket wrong direction");

    cg::packet::Packet missingField(
        cg::packet::PacketCode("kg"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        CharacterLevelUpPointStatus::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "kg",
        true,
        0U,
        "kg FromPacket missing field");

    cg::packet::Packet extraField = CharacterLevelUpPointStatus(100).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "0",
    });
    failures += ExpectError(
        CharacterLevelUpPointStatus::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "kg",
        true,
        1U,
        "kg FromPacket extra field");

    cg::packet::Packet wrongName = CharacterLevelUpPointStatus(100).ToPacket();
    wrongName.fields[0].name = "wrong";
    failures += ExpectError(
        CharacterLevelUpPointStatus::FromPacket(wrongName),
        cg::packet::PacketErrorCode::DecodeFailed,
        "kg",
        true,
        0U,
        "kg FromPacket wrong name");

    cg::packet::Packet wrongType = CharacterLevelUpPointStatus(100).ToPacket();
    wrongType.fields[0].type = cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        CharacterLevelUpPointStatus::FromPacket(wrongType),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "kg",
        true,
        0U,
        "kg FromPacket wrong type");

    cg::packet::Packet invalidValue = CharacterLevelUpPointStatus(100).ToPacket();
    invalidValue.fields[0].value = "not-an-int";
    failures += ExpectError(
        CharacterLevelUpPointStatus::FromPacket(invalidValue),
        cg::packet::PacketErrorCode::DecodeFailed,
        "kg",
        true,
        0U,
        "kg FromPacket invalid decimal");

    cg::packet::Packet positiveOverflow =
        CharacterLevelUpPointStatus(100).ToPacket();
    positiveOverflow.fields[0].value = "2147483648";
    failures += ExpectError(
        CharacterLevelUpPointStatus::FromPacket(positiveOverflow),
        cg::packet::PacketErrorCode::DecodeFailed,
        "kg",
        true,
        0U,
        "kg FromPacket positive decimal overflow");

    cg::packet::Packet negativeOverflow =
        CharacterLevelUpPointStatus(100).ToPacket();
    negativeOverflow.fields[0].value = "-2147483649";
    failures += ExpectError(
        CharacterLevelUpPointStatus::FromPacket(negativeOverflow),
        cg::packet::PacketErrorCode::DecodeFailed,
        "kg",
        true,
        0U,
        "kg FromPacket negative decimal overflow");
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
        "code=kg\n"
        "direction=server_to_client\n"
        "sample=available-points-one-hundred\n"
        "plain_hex=6B 67 20 31 43\n"
        "canonical_hex=6B 67 20 31 43 20\n"
        "end_sample\n";

    const std::string fixture = ReadFixture(path);
    int failures = 0;
    failures += Expect(fixture == Expected, "kg fixture exact bytes");

    std::size_t lineFeeds = 0U;
    bool asciiOnly = true;
    bool physicalBytesClean = true;
    for (const unsigned char byte : fixture) {
        lineFeeds += byte == 0x0AU ? 1U : 0U;
        asciiOnly = asciiOnly && byte <= 0x7FU;
        physicalBytesClean =
            physicalBytesClean && byte != 0x00U && byte != 0x0DU && byte != 0x17U;
    }
    failures += Expect(asciiOnly, "kg fixture ASCII");
    failures += Expect(physicalBytesClean, "kg fixture physical bytes");
    failures += Expect(lineFeeds == 7U, "kg fixture LF count");
    return failures;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr
            << "usage: cg-packet-character-level-up-point-status-test "
               "<kg-fixture>\n";
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
                  << " character level-up point status packet test(s) failed\n";
        return 1;
    }
    return 0;
}
