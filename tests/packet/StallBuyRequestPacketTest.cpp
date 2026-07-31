#include "packet/PacketBuilder.hpp"
#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::StallBuyRequest;

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
    std::string_view position,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode("wAo"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "position", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == position, message);
    }
    return failures;
}

int TestSpecAndRoundtrip()
{
    const StallBuyRequest request(8);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(StallBuyRequest::Code == "wAo", "wAo code");
    failures += Expect(
        StallBuyRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "wAo direction");
    failures += Expect(
        StallBuyRequest::Fields.size() == 1U,
        "wAo field count");
    failures += Expect(
        StallBuyRequest::Fields[0].name == "position",
        "wAo field name");
    failures += Expect(
        StallBuyRequest::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "wAo field type");
    failures += Expect(
        StallBuyRequest::Spec.code == StallBuyRequest::Code,
        "wAo Spec code");
    failures += Expect(
        StallBuyRequest::Spec.direction == StallBuyRequest::Direction,
        "wAo Spec direction");
    failures += Expect(
        StallBuyRequest::Spec.fieldCount == 1U,
        "wAo Spec field count");
    failures += Expect(request.position == 8, "wAo member");
    failures += ExpectPacket(packet, "8", "wAo ToPacket");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "wAo Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "wAo 8 ", "wAo canonical");
    }

    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("wAo 8", StallBuyRequest::Spec);
    failures += Expect(parsed.HasValue(), "wAo generic parse");
    if (parsed.HasValue()) {
        failures += ExpectPacket(
            parsed.Value(),
            "8",
            "wAo generic packet");

        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(parsed.Value(), StallBuyRequest::Spec);
        failures += Expect(rebuilt.HasValue(), "wAo generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(
                rebuilt.Value() == "wAo 8 ",
                "wAo generic canonical");
        }
    }
    return failures;
}

int TestSignedValues()
{
    struct Case
    {
        int position;
        std::string_view canonical;
    };

    constexpr Case Cases[] = {
        {0, "wAo 0 "},
        {4, "wAo 4 "},
        {5, "wAo 5 "},
        {7, "wAo 7 "},
        {8, "wAo 8 "},
        {27, "wAo r "},
        {28, "wAo s "},
        {-1, "wAo -1 "},
        {-123, "wAo -1Z "},
        {std::numeric_limits<int>::min(), "wAo -2lkCB2 "},
        {std::numeric_limits<int>::max(), "wAo 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const StallBuyRequest request(testCase.position);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.position),
            "wAo signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "wAo signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.canonical,
                "wAo signed canonical");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad 8", StallBuyRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "wAo wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("wAo", StallBuyRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "wAo",
        true,
        0U,
        "wAo missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("wAo 8 1", StallBuyRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "wAo",
        true,
        1U,
        "wAo extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("wAo _", StallBuyRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "wAo",
        true,
        0U,
        "wAo invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("wAo 2lkCB2", StallBuyRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "wAo",
        true,
        0U,
        "wAo positive i62 overflow");
    failures += ExpectError(
        cg::packet::ParsePacket("wAo -2lkCB3", StallBuyRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "wAo",
        true,
        0U,
        "wAo negative i62 overflow");

    const std::string pollutedCode = "wAo\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedCode + " 8",
            StallBuyRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "wAo polluted code");

    const std::string pollutedField = "wAo 8\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(pollutedField, StallBuyRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "wAo",
        true,
        1U,
        "wAo polluted field");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode = StallBuyRequest(8).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        cg::packet::BuildPacket(wrongCode, StallBuyRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "wAo Build wrong code");

    cg::packet::Packet wrongDirection = StallBuyRequest(8).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongDirection, StallBuyRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "wAo",
        false,
        0U,
        "wAo Build wrong direction");

    cg::packet::Packet wrongName = StallBuyRequest(8).ToPacket();
    wrongName.fields[0].name = "wrong";
    failures += ExpectError(
        cg::packet::BuildPacket(wrongName, StallBuyRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "wAo",
        true,
        0U,
        "wAo Build wrong field name");

    cg::packet::Packet wrongType = StallBuyRequest(8).ToPacket();
    wrongType.fields[0].type = cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongType, StallBuyRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "wAo",
        true,
        0U,
        "wAo Build wrong field type");

    cg::packet::Packet emptyValue = StallBuyRequest(8).ToPacket();
    emptyValue.fields[0].value.clear();
    failures += ExpectError(
        cg::packet::BuildPacket(emptyValue, StallBuyRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "wAo",
        true,
        0U,
        "wAo Build empty decimal");

    cg::packet::Packet invalidValue = StallBuyRequest(8).ToPacket();
    invalidValue.fields[0].value = "not-an-int";
    failures += ExpectError(
        cg::packet::BuildPacket(invalidValue, StallBuyRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "wAo",
        true,
        0U,
        "wAo Build non-decimal");

    cg::packet::Packet positiveOverflow = StallBuyRequest(8).ToPacket();
    positiveOverflow.fields[0].value = "2147483648";
    failures += ExpectError(
        cg::packet::BuildPacket(positiveOverflow, StallBuyRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "wAo",
        true,
        0U,
        "wAo Build positive overflow");

    cg::packet::Packet negativeOverflow = StallBuyRequest(8).ToPacket();
    negativeOverflow.fields[0].value = "-2147483649";
    failures += ExpectError(
        cg::packet::BuildPacket(negativeOverflow, StallBuyRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "wAo",
        true,
        0U,
        "wAo Build negative overflow");
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
                  << " stall buy request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
