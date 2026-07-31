#include "packet/PacketBuilder.hpp"
#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::CheatDetectionReport;

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
        packet.code == cg::packet::PacketCode("PGXE"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "type", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == value, message);
    }
    return failures;
}

int TestSpecAndRoundtrip()
{
    const CheatDetectionReport report(1);
    const cg::packet::Packet packet = report.ToPacket();

    int failures = 0;
    failures += Expect(
        CheatDetectionReport::Code == "PGXE",
        "PGXE code");
    failures += Expect(
        CheatDetectionReport::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "PGXE direction");
    failures += Expect(
        CheatDetectionReport::Fields.size() == 1U,
        "PGXE field count");
    failures += Expect(
        CheatDetectionReport::Fields[0].name == "type",
        "PGXE field name");
    failures += Expect(
        CheatDetectionReport::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "PGXE field type");
    failures += Expect(
        CheatDetectionReport::Spec.code == CheatDetectionReport::Code,
        "PGXE Spec code");
    failures += Expect(
        CheatDetectionReport::Spec.direction ==
            CheatDetectionReport::Direction,
        "PGXE Spec direction");
    failures += Expect(
        CheatDetectionReport::Spec.fieldCount == 1U,
        "PGXE Spec field count");
    failures += Expect(report.type == 1, "PGXE member");
    failures += ExpectPacket(packet, "1", "PGXE ToPacket");

    const cg::packet::PacketResult<std::string> encoded = report.Encode();
    failures += Expect(encoded.HasValue(), "PGXE Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "PGXE 1 ",
            "PGXE canonical");
    }

    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("PGXE 1", CheatDetectionReport::Spec);
    failures += Expect(parsed.HasValue(), "PGXE generic parse");
    if (parsed.HasValue()) {
        failures += ExpectPacket(
            parsed.Value(),
            "1",
            "PGXE generic packet");

        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(
                parsed.Value(),
                CheatDetectionReport::Spec);
        failures += Expect(rebuilt.HasValue(), "PGXE generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(
                rebuilt.Value() == "PGXE 1 ",
                "PGXE generic canonical");
        }
    }
    return failures;
}

int TestSignedValues()
{
    struct Case
    {
        int type;
        std::string_view canonical;
    };

    constexpr Case Cases[] = {
        {0, "PGXE 0 "},
        {1, "PGXE 1 "},
        {2, "PGXE 2 "},
        {3, "PGXE 3 "},
        {4, "PGXE 4 "},
        {5, "PGXE 5 "},
        {7, "PGXE 7 "},
        {9, "PGXE 9 "},
        {11, "PGXE b "},
        {13, "PGXE d "},
        {15, "PGXE f "},
        {42, "PGXE G "},
        {101, "PGXE 1D "},
        {-1, "PGXE -1 "},
        {std::numeric_limits<int>::min(), "PGXE -2lkCB2 "},
        {std::numeric_limits<int>::max(), "PGXE 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const CheatDetectionReport report(testCase.type);
        failures += ExpectPacket(
            report.ToPacket(),
            std::to_string(testCase.type),
            "PGXE signed packet");

        const cg::packet::PacketResult<std::string> encoded = report.Encode();
        failures += Expect(encoded.HasValue(), "PGXE signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.canonical,
                "PGXE signed canonical");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad 1", CheatDetectionReport::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "PGXE wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("PGXE", CheatDetectionReport::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "PGXE",
        true,
        0U,
        "PGXE missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("PGXE 1 2", CheatDetectionReport::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "PGXE",
        true,
        1U,
        "PGXE extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("PGXE _", CheatDetectionReport::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "PGXE",
        true,
        0U,
        "PGXE invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("PGXE 2lkCB2", CheatDetectionReport::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "PGXE",
        true,
        0U,
        "PGXE positive i62 overflow");
    failures += ExpectError(
        cg::packet::ParsePacket("PGXE -2lkCB3", CheatDetectionReport::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "PGXE",
        true,
        0U,
        "PGXE negative i62 overflow");

    const std::string pollutedCode = "PGXE\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedCode + " 1",
            CheatDetectionReport::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "PGXE polluted code");

    const std::string pollutedField = "PGXE 1\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedField,
            CheatDetectionReport::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "PGXE",
        true,
        1U,
        "PGXE polluted field");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = CheatDetectionReport(1).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        cg::packet::BuildPacket(
            wrongCode,
            CheatDetectionReport::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "PGXE Build wrong code");

    cg::packet::Packet wrongDirection = CheatDetectionReport(1).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        cg::packet::BuildPacket(
            wrongDirection,
            CheatDetectionReport::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "PGXE",
        false,
        0U,
        "PGXE Build wrong direction");

    cg::packet::Packet wrongName = CheatDetectionReport(1).ToPacket();
    wrongName.fields[0].name = "wrong";
    failures += ExpectError(
        cg::packet::BuildPacket(
            wrongName,
            CheatDetectionReport::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "PGXE",
        true,
        0U,
        "PGXE Build wrong field name");

    cg::packet::Packet wrongType = CheatDetectionReport(1).ToPacket();
    wrongType.fields[0].type = cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        cg::packet::BuildPacket(
            wrongType,
            CheatDetectionReport::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "PGXE",
        true,
        0U,
        "PGXE Build wrong field type");

    cg::packet::Packet emptyValue = CheatDetectionReport(1).ToPacket();
    emptyValue.fields[0].value.clear();
    failures += ExpectError(
        cg::packet::BuildPacket(
            emptyValue,
            CheatDetectionReport::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "PGXE",
        true,
        0U,
        "PGXE Build empty decimal");

    cg::packet::Packet invalidValue = CheatDetectionReport(1).ToPacket();
    invalidValue.fields[0].value = "not-an-int";
    failures += ExpectError(
        cg::packet::BuildPacket(
            invalidValue,
            CheatDetectionReport::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "PGXE",
        true,
        0U,
        "PGXE Build non-decimal");

    cg::packet::Packet positiveOverflow = CheatDetectionReport(1).ToPacket();
    positiveOverflow.fields[0].value = "2147483648";
    failures += ExpectError(
        cg::packet::BuildPacket(
            positiveOverflow,
            CheatDetectionReport::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "PGXE",
        true,
        0U,
        "PGXE Build positive overflow");

    cg::packet::Packet negativeOverflow = CheatDetectionReport(1).ToPacket();
    negativeOverflow.fields[0].value = "-2147483649";
    failures += ExpectError(
        cg::packet::BuildPacket(
            negativeOverflow,
            CheatDetectionReport::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "PGXE",
        true,
        0U,
        "PGXE Build negative overflow");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndRoundtrip()
        + TestSignedValues()
        + TestParseErrors()
        + TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures
                  << " cheat detection report packet test(s) failed\n";
        return 1;
    }
    return 0;
}
