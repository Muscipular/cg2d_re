#include "packet/PacketBuilder.hpp"
#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::StallBrowseRequest;

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
    std::string_view objectIndex,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode("ElVN"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "objIndex", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == objectIndex, message);
    }
    return failures;
}

int TestSpecAndRoundtrip()
{
    const StallBrowseRequest request(10);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(
        StallBrowseRequest::Code == "ElVN",
        "ElVN code");
    failures += Expect(
        StallBrowseRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "ElVN direction");
    failures += Expect(
        StallBrowseRequest::Fields.size() == 1U,
        "ElVN field count");
    failures += Expect(
        StallBrowseRequest::Fields[0].name == "objIndex",
        "ElVN field name");
    failures += Expect(
        StallBrowseRequest::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "ElVN field type");
    failures += Expect(
        StallBrowseRequest::Spec.code == StallBrowseRequest::Code,
        "ElVN Spec code");
    failures += Expect(
        StallBrowseRequest::Spec.direction ==
            StallBrowseRequest::Direction,
        "ElVN Spec direction");
    failures += Expect(
        StallBrowseRequest::Spec.fieldCount == 1U,
        "ElVN Spec field count");
    failures += Expect(request.objectIndex == 10, "ElVN member");
    failures += ExpectPacket(packet, "10", "ElVN ToPacket");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "ElVN Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "ElVN a ",
            "ElVN canonical");
    }

    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("ElVN a", StallBrowseRequest::Spec);
    failures += Expect(parsed.HasValue(), "ElVN generic parse");
    if (parsed.HasValue()) {
        failures += ExpectPacket(
            parsed.Value(),
            "10",
            "ElVN generic packet");

        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(
                parsed.Value(),
                StallBrowseRequest::Spec);
        failures += Expect(rebuilt.HasValue(), "ElVN generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(
                rebuilt.Value() == "ElVN a ",
                "ElVN generic canonical");
        }
    }
    return failures;
}

int TestSignedValues()
{
    struct Case
    {
        int objectIndex;
        std::string_view canonical;
    };

    constexpr Case Cases[] = {
        {0, "ElVN 0 "},
        {1, "ElVN 1 "},
        {10, "ElVN a "},
        {42, "ElVN G "},
        {-1, "ElVN -1 "},
        {-123, "ElVN -1Z "},
        {std::numeric_limits<int>::min(), "ElVN -2lkCB2 "},
        {std::numeric_limits<int>::max(), "ElVN 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const StallBrowseRequest request(testCase.objectIndex);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.objectIndex),
            "ElVN signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "ElVN signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.canonical,
                "ElVN signed canonical");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a", StallBrowseRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "ElVN wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("ElVN", StallBrowseRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ElVN",
        true,
        0U,
        "ElVN missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("ElVN a 1", StallBrowseRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ElVN",
        true,
        1U,
        "ElVN extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("ElVN _", StallBrowseRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ElVN",
        true,
        0U,
        "ElVN invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("ElVN 2lkCB2", StallBrowseRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ElVN",
        true,
        0U,
        "ElVN positive i62 overflow");
    failures += ExpectError(
        cg::packet::ParsePacket("ElVN -2lkCB3", StallBrowseRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ElVN",
        true,
        0U,
        "ElVN negative i62 overflow");

    const std::string pollutedCode = "ElVN\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedCode + " a",
            StallBrowseRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "ElVN polluted code");

    const std::string pollutedField = "ElVN a\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedField,
            StallBrowseRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ElVN",
        true,
        1U,
        "ElVN polluted field");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = StallBrowseRequest(10).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        cg::packet::BuildPacket(wrongCode, StallBrowseRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "ElVN Build wrong code");

    cg::packet::Packet wrongDirection = StallBrowseRequest(10).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        cg::packet::BuildPacket(
            wrongDirection,
            StallBrowseRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ElVN",
        false,
        0U,
        "ElVN Build wrong direction");

    cg::packet::Packet wrongName = StallBrowseRequest(10).ToPacket();
    wrongName.fields[0].name = "wrong";
    failures += ExpectError(
        cg::packet::BuildPacket(wrongName, StallBrowseRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ElVN",
        true,
        0U,
        "ElVN Build wrong field name");

    cg::packet::Packet wrongType = StallBrowseRequest(10).ToPacket();
    wrongType.fields[0].type = cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongType, StallBrowseRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ElVN",
        true,
        0U,
        "ElVN Build wrong field type");

    cg::packet::Packet emptyValue = StallBrowseRequest(10).ToPacket();
    emptyValue.fields[0].value.clear();
    failures += ExpectError(
        cg::packet::BuildPacket(emptyValue, StallBrowseRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ElVN",
        true,
        0U,
        "ElVN Build empty decimal");

    cg::packet::Packet invalidValue = StallBrowseRequest(10).ToPacket();
    invalidValue.fields[0].value = "not-an-int";
    failures += ExpectError(
        cg::packet::BuildPacket(invalidValue, StallBrowseRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ElVN",
        true,
        0U,
        "ElVN Build non-decimal");

    cg::packet::Packet positiveOverflow = StallBrowseRequest(10).ToPacket();
    positiveOverflow.fields[0].value = "2147483648";
    failures += ExpectError(
        cg::packet::BuildPacket(
            positiveOverflow,
            StallBrowseRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ElVN",
        true,
        0U,
        "ElVN Build positive overflow");

    cg::packet::Packet negativeOverflow = StallBrowseRequest(10).ToPacket();
    negativeOverflow.fields[0].value = "-2147483649";
    failures += ExpectError(
        cg::packet::BuildPacket(
            negativeOverflow,
            StallBrowseRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "ElVN",
        true,
        0U,
        "ElVN Build negative overflow");
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
                  << " stall browse request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
