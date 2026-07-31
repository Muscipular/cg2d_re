#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"
#include "packet/Send.hpp"

#include <cstddef>
#include <fstream>
#include <iostream>
#include <iterator>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::AlbumRegistrationNotification;
using cg::packet::send::AlbumInfoRequest;

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
    failures += Expect(packet.code == cg::packet::PacketCode("IYo"), message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ServerToClient,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "albumId", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == expected, message);
    }
    return failures;
}

int TestSpecAndRoundtrip()
{
    const AlbumRegistrationNotification value(1);
    const cg::packet::Packet packet = value.ToPacket();

    int failures = 0;
    failures += Expect(
        AlbumRegistrationNotification::Code == "IYo",
        "IYo code");
    failures += Expect(
        AlbumRegistrationNotification::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "IYo direction");
    failures += Expect(
        AlbumRegistrationNotification::Fields.size() == 1U,
        "IYo fields");
    failures += Expect(
        AlbumRegistrationNotification::Fields[0].name == "albumId",
        "IYo field name");
    failures += Expect(
        AlbumRegistrationNotification::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "IYo field type");
    failures += Expect(
        AlbumRegistrationNotification::Spec.code ==
            AlbumRegistrationNotification::Code,
        "IYo Spec code");
    failures += Expect(
        AlbumRegistrationNotification::Spec.direction ==
            AlbumRegistrationNotification::Direction,
        "IYo Spec direction");
    failures += Expect(
        AlbumRegistrationNotification::Spec.fieldCount == 1U,
        "IYo Spec field count");
    failures += Expect(value.albumId == 1, "IYo member");
    failures += ExpectPacket(packet, "1", "IYo ToPacket");

    const cg::packet::PacketResult<AlbumRegistrationNotification> converted =
        AlbumRegistrationNotification::FromPacket(packet);
    failures += Expect(converted.HasValue(), "IYo FromPacket valid");
    if (converted.HasValue()) {
        failures += Expect(converted.Value().albumId == 1, "IYo FromPacket value");
    }

    const cg::packet::PacketResult<AlbumRegistrationNotification> typed =
        AlbumRegistrationNotification::Parse("IYo 1");
    failures += Expect(typed.HasValue(), "IYo typed parse");
    if (typed.HasValue()) {
        failures += Expect(typed.Value().albumId == 1, "IYo typed value");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket("IYo 1", AlbumRegistrationNotification::Spec);
    failures += Expect(generic.HasValue(), "IYo generic parse");
    if (generic.HasValue()) {
        failures += ExpectPacket(generic.Value(), "1", "IYo generic packet");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(
                generic.Value(),
                AlbumRegistrationNotification::Spec);
        failures += Expect(rebuilt.HasValue(), "IYo generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(rebuilt.Value() == "IYo 1 ", "IYo canonical");
        }
    }
    return failures;
}

int TestSignedValues()
{
    struct Case
    {
        int albumId;
        std::string_view canonical;
    };

    constexpr Case Cases[] = {
        {1, "IYo 1 "},
        {-1, "IYo -1 "},
        {std::numeric_limits<int>::min(), "IYo -2lkCB2 "},
        {std::numeric_limits<int>::max(), "IYo 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const AlbumRegistrationNotification value(testCase.albumId);
        failures += ExpectPacket(
            value.ToPacket(),
            std::to_string(testCase.albumId),
            "IYo signed packet");

        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(
                value.ToPacket(),
                AlbumRegistrationNotification::Spec);
        failures += Expect(built.HasValue(), "IYo signed build");
        if (built.HasValue()) {
            failures += Expect(
                built.Value() == testCase.canonical,
                "IYo signed canonical");
        }

        const cg::packet::PacketResult<AlbumRegistrationNotification> parsed =
            AlbumRegistrationNotification::Parse(testCase.canonical);
        failures += Expect(parsed.HasValue(), "IYo signed parse");
        if (parsed.HasValue()) {
            failures += Expect(
                parsed.Value().albumId == testCase.albumId,
                "IYo signed value");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        AlbumRegistrationNotification::Parse("bad 1"),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "IYo wrong code");
    failures += ExpectError(
        AlbumRegistrationNotification::Parse("IYo"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IYo",
        true,
        0U,
        "IYo missing field");
    failures += ExpectError(
        AlbumRegistrationNotification::Parse("IYo 1 2"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IYo",
        true,
        1U,
        "IYo extra field");
    failures += ExpectError(
        AlbumRegistrationNotification::Parse("IYo _"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IYo",
        true,
        0U,
        "IYo invalid i62");
    failures += ExpectError(
        AlbumRegistrationNotification::Parse("IYo 2lkCB2"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IYo",
        true,
        0U,
        "IYo positive overflow");
    failures += ExpectError(
        AlbumRegistrationNotification::Parse("IYo -2lkCB3"),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IYo",
        true,
        0U,
        "IYo negative overflow");

    const std::string pollutedCode = "IYo\x17" "x";
    failures += ExpectError(
        AlbumRegistrationNotification::Parse(pollutedCode + " 1"),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "IYo polluted code");

    const std::string pollutedField = "IYo 1\x17" "x";
    failures += ExpectError(
        AlbumRegistrationNotification::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IYo",
        true,
        1U,
        "IYo polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = AlbumRegistrationNotification(1).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        AlbumRegistrationNotification::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "IYo FromPacket wrong code");

    cg::packet::Packet wrongDirection =
        AlbumRegistrationNotification(1).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        AlbumRegistrationNotification::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IYo",
        false,
        0U,
        "IYo FromPacket wrong direction");

    cg::packet::Packet missingField(
        cg::packet::PacketCode("IYo"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        AlbumRegistrationNotification::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IYo",
        true,
        0U,
        "IYo FromPacket missing field");

    cg::packet::Packet extraField = AlbumRegistrationNotification(1).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "0",
    });
    failures += ExpectError(
        AlbumRegistrationNotification::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IYo",
        true,
        1U,
        "IYo FromPacket extra field");

    cg::packet::Packet wrongName = AlbumRegistrationNotification(1).ToPacket();
    wrongName.fields[0].name = "wrong";
    failures += ExpectError(
        AlbumRegistrationNotification::FromPacket(wrongName),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IYo",
        true,
        0U,
        "IYo FromPacket wrong name");

    cg::packet::Packet wrongType = AlbumRegistrationNotification(1).ToPacket();
    wrongType.fields[0].type = cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        AlbumRegistrationNotification::FromPacket(wrongType),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "IYo",
        true,
        0U,
        "IYo FromPacket wrong type");

    cg::packet::Packet invalidValue = AlbumRegistrationNotification(1).ToPacket();
    invalidValue.fields[0].value = "not-an-int";
    failures += ExpectError(
        AlbumRegistrationNotification::FromPacket(invalidValue),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IYo",
        true,
        0U,
        "IYo FromPacket invalid decimal");

    cg::packet::Packet positiveOverflow =
        AlbumRegistrationNotification(1).ToPacket();
    positiveOverflow.fields[0].value = "2147483648";
    failures += ExpectError(
        AlbumRegistrationNotification::FromPacket(positiveOverflow),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IYo",
        true,
        0U,
        "IYo FromPacket positive decimal overflow");

    cg::packet::Packet negativeOverflow =
        AlbumRegistrationNotification(1).ToPacket();
    negativeOverflow.fields[0].value = "-2147483649";
    failures += ExpectError(
        AlbumRegistrationNotification::FromPacket(negativeOverflow),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IYo",
        true,
        0U,
        "IYo FromPacket negative decimal overflow");
    return failures;
}

int TestAlbumPacketIsolation()
{
    int failures = 0;
    failures += ExpectError(
        AlbumRegistrationNotification::Parse("dM"),
        cg::packet::PacketErrorCode::InvalidCode,
        "dM",
        false,
        0U,
        "IYo rejects dM");
    failures += Expect(
        AlbumInfoRequest::Code == "kza" &&
            AlbumInfoRequest::Direction ==
                cg::packet::PacketDirection::ClientToServer,
        "kza request direction isolated");
    failures += ExpectError(
        AlbumRegistrationNotification::Parse("kza 1 1"),
        cg::packet::PacketErrorCode::InvalidCode,
        "kza",
        false,
        0U,
        "IYo rejects kza");
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
        "code=IYo\n"
        "direction=server_to_client\n"
        "sample=album-id-one\n"
        "plain_hex=49 59 6F 20 31\n"
        "canonical_hex=49 59 6F 20 31 20\n"
        "end_sample\n";

    const std::string fixture = ReadFixture(path);
    int failures = 0;
    failures += Expect(fixture == Expected, "IYo fixture exact bytes");

    std::size_t lineFeeds = 0U;
    bool asciiOnly = true;
    bool physicalBytesClean = true;
    for (const unsigned char byte : fixture) {
        lineFeeds += byte == 0x0AU ? 1U : 0U;
        asciiOnly = asciiOnly && byte <= 0x7FU;
        physicalBytesClean =
            physicalBytesClean && byte != 0x00U && byte != 0x0DU && byte != 0x17U;
    }
    failures += Expect(asciiOnly, "IYo fixture ASCII");
    failures += Expect(physicalBytesClean, "IYo fixture physical bytes");
    failures += Expect(lineFeeds == 7U, "IYo fixture LF count");
    return failures;
}

} // namespace

int main(int argc, char** argv)
{
    if (argc != 2) {
        std::cerr
            << "usage: cg-packet-album-registration-notification-test "
               "<IYo-fixture>\n";
        return 1;
    }

    const int failures =
        TestSpecAndRoundtrip()
        + TestSignedValues()
        + TestParseErrors()
        + TestFromPacketErrors()
        + TestAlbumPacketIsolation()
        + TestFixturePhysical(argv[1]);
    if (failures != 0) {
        std::cerr << failures
                  << " album registration notification packet test(s) failed\n";
        return 1;
    }
    return 0;
}
