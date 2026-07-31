#include "packet/PacketBuilder.hpp"
#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::BagPageRequest;

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
    std::string_view page,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode("SWITM"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "page", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == page, message);
    }
    return failures;
}

int TestSpecAndRoundtrip()
{
    const BagPageRequest request(1);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(BagPageRequest::Code == "SWITM", "SWITM code");
    failures += Expect(
        BagPageRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "SWITM direction");
    failures += Expect(
        BagPageRequest::Fields.size() == 1U,
        "SWITM field count");
    failures += Expect(
        BagPageRequest::Fields[0].name == "page",
        "SWITM field name");
    failures += Expect(
        BagPageRequest::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "SWITM field type");
    failures += Expect(
        BagPageRequest::Spec.code == BagPageRequest::Code,
        "SWITM Spec code");
    failures += Expect(
        BagPageRequest::Spec.direction == BagPageRequest::Direction,
        "SWITM Spec direction");
    failures += Expect(
        BagPageRequest::Spec.fieldCount == 1U,
        "SWITM Spec field count");
    failures += Expect(request.page == 1, "SWITM member");
    failures += ExpectPacket(packet, "1", "SWITM ToPacket");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "SWITM Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "SWITM 1 ",
            "SWITM canonical");
    }

    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("SWITM 1", BagPageRequest::Spec);
    failures += Expect(parsed.HasValue(), "SWITM generic parse");
    if (parsed.HasValue()) {
        failures += ExpectPacket(
            parsed.Value(),
            "1",
            "SWITM generic packet");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(parsed.Value(), BagPageRequest::Spec);
        failures += Expect(rebuilt.HasValue(), "SWITM generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(
                rebuilt.Value() == "SWITM 1 ",
                "SWITM generic canonical");
        }
    }
    return failures;
}

int TestSignedValues()
{
    struct Case
    {
        int page;
        std::string_view canonical;
    };

    constexpr Case Cases[] = {
        {0, "SWITM 0 "},
        {1, "SWITM 1 "},
        {4, "SWITM 4 "},
        {5, "SWITM 5 "},
        {-1, "SWITM -1 "},
        {-123, "SWITM -1Z "},
        {std::numeric_limits<int>::min(), "SWITM -2lkCB2 "},
        {std::numeric_limits<int>::max(), "SWITM 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const BagPageRequest request(testCase.page);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.page),
            "SWITM signed packet");
        const cg::packet::PacketResult<std::string> encoded =
            request.Encode();
        failures += Expect(encoded.HasValue(), "SWITM signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.canonical,
                "SWITM signed canonical");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad 1", BagPageRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "SWITM wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("SWITM", BagPageRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "SWITM",
        true,
        0U,
        "SWITM missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("SWITM 1 0", BagPageRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "SWITM",
        true,
        1U,
        "SWITM extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("SWITM _", BagPageRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "SWITM",
        true,
        0U,
        "SWITM invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("SWITM 2lkCB2", BagPageRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "SWITM",
        true,
        0U,
        "SWITM positive i62 overflow");
    failures += ExpectError(
        cg::packet::ParsePacket("SWITM -2lkCB3", BagPageRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "SWITM",
        true,
        0U,
        "SWITM negative i62 overflow");

    const std::string pollutedCode = "SWITM\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedCode + " 1",
            BagPageRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "SWITM polluted code");
    const std::string pollutedField = "SWITM 1\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(pollutedField, BagPageRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "SWITM",
        true,
        1U,
        "SWITM polluted field");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;
    cg::packet::Packet wrongCode = BagPageRequest(1).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        cg::packet::BuildPacket(wrongCode, BagPageRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "SWITM Build wrong code");

    cg::packet::Packet wrongDirection = BagPageRequest(1).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongDirection, BagPageRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "SWITM",
        false,
        0U,
        "SWITM Build wrong direction");

    cg::packet::Packet wrongName = BagPageRequest(1).ToPacket();
    wrongName.fields[0].name = "wrong";
    failures += ExpectError(
        cg::packet::BuildPacket(wrongName, BagPageRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "SWITM",
        true,
        0U,
        "SWITM Build wrong field name");

    cg::packet::Packet wrongType = BagPageRequest(1).ToPacket();
    wrongType.fields[0].type =
        cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        cg::packet::BuildPacket(wrongType, BagPageRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "SWITM",
        true,
        0U,
        "SWITM Build wrong field type");

    cg::packet::Packet emptyValue = BagPageRequest(1).ToPacket();
    emptyValue.fields[0].value.clear();
    failures += ExpectError(
        cg::packet::BuildPacket(emptyValue, BagPageRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "SWITM",
        true,
        0U,
        "SWITM Build empty decimal");

    cg::packet::Packet invalidValue = BagPageRequest(1).ToPacket();
    invalidValue.fields[0].value = "not-an-int";
    failures += ExpectError(
        cg::packet::BuildPacket(invalidValue, BagPageRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "SWITM",
        true,
        0U,
        "SWITM Build non-decimal");

    cg::packet::Packet positiveOverflow = BagPageRequest(1).ToPacket();
    positiveOverflow.fields[0].value = "2147483648";
    failures += ExpectError(
        cg::packet::BuildPacket(positiveOverflow, BagPageRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "SWITM",
        true,
        0U,
        "SWITM Build positive overflow");

    cg::packet::Packet negativeOverflow = BagPageRequest(1).ToPacket();
    negativeOverflow.fields[0].value = "-2147483649";
    failures += ExpectError(
        cg::packet::BuildPacket(negativeOverflow, BagPageRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "SWITM",
        true,
        0U,
        "SWITM Build negative overflow");
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
                  << " bag page request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
