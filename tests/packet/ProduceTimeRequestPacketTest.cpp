#include "packet/PacketBuilder.hpp"
#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::ProduceTimeRequest;

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
    std::string_view time,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode("IPy"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "time", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == time, message);
    }
    return failures;
}

int TestSpecAndRoundtrip()
{
    const ProduceTimeRequest request(10);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(ProduceTimeRequest::Code == "IPy", "IPy code");
    failures += Expect(
        ProduceTimeRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "IPy direction");
    failures += Expect(
        ProduceTimeRequest::Fields.size() == 1U,
        "IPy field count");
    failures += Expect(
        ProduceTimeRequest::Fields[0].name == "time",
        "IPy field name");
    failures += Expect(
        ProduceTimeRequest::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "IPy field type");
    failures += Expect(
        ProduceTimeRequest::Spec.code == ProduceTimeRequest::Code,
        "IPy Spec code");
    failures += Expect(
        ProduceTimeRequest::Spec.direction == ProduceTimeRequest::Direction,
        "IPy Spec direction");
    failures += Expect(
        ProduceTimeRequest::Spec.fieldCount == 1U,
        "IPy Spec field count");
    failures += Expect(request.timeSeconds == 10, "IPy member");
    failures += ExpectPacket(packet, "10", "IPy ToPacket");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "IPy Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "IPy a ", "IPy canonical");
    }

    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("IPy a", ProduceTimeRequest::Spec);
    failures += Expect(parsed.HasValue(), "IPy generic parse");
    if (parsed.HasValue()) {
        failures += ExpectPacket(
            parsed.Value(),
            "10",
            "IPy generic packet");

        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(parsed.Value(), ProduceTimeRequest::Spec);
        failures += Expect(rebuilt.HasValue(), "IPy generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(
                rebuilt.Value() == "IPy a ",
                "IPy generic canonical");
        }
    }
    return failures;
}

int TestSignedValues()
{
    struct Case
    {
        int timeSeconds;
        std::string_view canonical;
    };

    constexpr Case Cases[] = {
        {0, "IPy 0 "},
        {1, "IPy 1 "},
        {10, "IPy a "},
        {42, "IPy G "},
        {-1, "IPy -1 "},
        {-123, "IPy -1Z "},
        {std::numeric_limits<int>::min(), "IPy -2lkCB2 "},
        {std::numeric_limits<int>::max(), "IPy 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const ProduceTimeRequest request(testCase.timeSeconds);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.timeSeconds),
            "IPy signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "IPy signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.canonical,
                "IPy signed canonical");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a", ProduceTimeRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "IPy wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("IPy", ProduceTimeRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IPy",
        true,
        0U,
        "IPy missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("IPy a 1", ProduceTimeRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IPy",
        true,
        1U,
        "IPy extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("IPy _", ProduceTimeRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IPy",
        true,
        0U,
        "IPy invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("IPy 2lkCB2", ProduceTimeRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IPy",
        true,
        0U,
        "IPy positive i62 overflow");
    failures += ExpectError(
        cg::packet::ParsePacket("IPy -2lkCB3", ProduceTimeRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IPy",
        true,
        0U,
        "IPy negative i62 overflow");

    const std::string pollutedCode = "IPy\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedCode + " a",
            ProduceTimeRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "IPy polluted code");

    const std::string pollutedField = "IPy a\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(pollutedField, ProduceTimeRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IPy",
        true,
        1U,
        "IPy polluted field");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = ProduceTimeRequest(10).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        cg::packet::BuildPacket(wrongCode, ProduceTimeRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "IPy Build wrong code");

    cg::packet::Packet wrongDirection = ProduceTimeRequest(10).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongDirection, ProduceTimeRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "IPy",
        false,
        0U,
        "IPy Build wrong direction");

    cg::packet::Packet wrongName = ProduceTimeRequest(10).ToPacket();
    wrongName.fields[0].name = "wrong";
    failures += ExpectError(
        cg::packet::BuildPacket(wrongName, ProduceTimeRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "IPy",
        true,
        0U,
        "IPy Build wrong field name");

    cg::packet::Packet wrongType = ProduceTimeRequest(10).ToPacket();
    wrongType.fields[0].type = cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongType, ProduceTimeRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "IPy",
        true,
        0U,
        "IPy Build wrong field type");

    cg::packet::Packet emptyValue = ProduceTimeRequest(10).ToPacket();
    emptyValue.fields[0].value.clear();
    failures += ExpectError(
        cg::packet::BuildPacket(emptyValue, ProduceTimeRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "IPy",
        true,
        0U,
        "IPy Build empty decimal");

    cg::packet::Packet invalidValue = ProduceTimeRequest(10).ToPacket();
    invalidValue.fields[0].value = "not-an-int";
    failures += ExpectError(
        cg::packet::BuildPacket(invalidValue, ProduceTimeRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "IPy",
        true,
        0U,
        "IPy Build non-decimal");

    cg::packet::Packet positiveOverflow = ProduceTimeRequest(10).ToPacket();
    positiveOverflow.fields[0].value = "2147483648";
    failures += ExpectError(
        cg::packet::BuildPacket(positiveOverflow, ProduceTimeRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "IPy",
        true,
        0U,
        "IPy Build positive overflow");

    cg::packet::Packet negativeOverflow = ProduceTimeRequest(10).ToPacket();
    negativeOverflow.fields[0].value = "-2147483649";
    failures += ExpectError(
        cg::packet::BuildPacket(negativeOverflow, ProduceTimeRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "IPy",
        true,
        0U,
        "IPy Build negative overflow");
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
                  << " produce time request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
