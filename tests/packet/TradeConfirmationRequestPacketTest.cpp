#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::TradeConfirmationRequest;

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
        packet.code == cg::packet::PacketCode("OU"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "result", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == value, message);
    }
    return failures;
}

int TestSpecAndEncode()
{
    const TradeConfirmationRequest request(1);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(
        TradeConfirmationRequest::Code == "OU",
        "OU code");
    failures += Expect(
        TradeConfirmationRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "OU direction");
    failures += Expect(
        TradeConfirmationRequest::Fields.size() == 1U,
        "OU fields");
    failures += Expect(
        TradeConfirmationRequest::Fields[0].name == "result",
        "OU field name");
    failures += Expect(
        TradeConfirmationRequest::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "OU field type");
    failures += Expect(
        TradeConfirmationRequest::Spec.code ==
            TradeConfirmationRequest::Code,
        "OU Spec code");
    failures += Expect(
        TradeConfirmationRequest::Spec.direction ==
            TradeConfirmationRequest::Direction,
        "OU Spec direction");
    failures += Expect(
        TradeConfirmationRequest::Spec.fieldCount == 1U,
        "OU Spec field count");
    failures += Expect(request.result == 1, "OU member");
    failures += ExpectPacket(packet, "1", "OU packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "OU Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "OU 1 ", "OU canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(
            "OU 1",
            TradeConfirmationRequest::Spec);

    int failures = Expect(parsed.HasValue(), "OU generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(parsed.Value(), "1", "OU generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(
            parsed.Value(),
            TradeConfirmationRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "OU generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "OU 1 ", "OU generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int result;
        std::string_view expected;
    };

    // legacy producer 只发 0/1；其余值仅证明 cg3a codec 保留完整 signed int。
    constexpr Case Cases[] = {
        {0, "OU 0 "},
        {1, "OU 1 "},
        {-1, "OU -1 "},
        {100, "OU 1C "},
        {std::numeric_limits<int>::min(), "OU -2lkCB2 "},
        {std::numeric_limits<int>::max(), "OU 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const TradeConfirmationRequest request(testCase.result);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.result),
            "OU signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "OU signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "OU signed canonical");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad 1 ", TradeConfirmationRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "OU wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("OU", TradeConfirmationRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "OU",
        true,
        0U,
        "OU missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("OU 1 0 ", TradeConfirmationRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "OU",
        true,
        1U,
        "OU extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("OU _ ", TradeConfirmationRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "OU",
        true,
        0U,
        "OU invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("OU 2lkCB2 ", TradeConfirmationRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "OU",
        true,
        0U,
        "OU positive overflow");
    failures += ExpectError(
        cg::packet::ParsePacket("OU -2lkCB3 ", TradeConfirmationRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "OU",
        true,
        0U,
        "OU negative overflow");

    const std::string pollutedCode = "OU\x17" "1";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedCode,
            TradeConfirmationRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "OU polluted code");

    const std::string pollutedField = "OU 1\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedField,
            TradeConfirmationRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "OU",
        true,
        1U,
        "OU polluted field");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndEncode()
        + TestGenericRoundtrip()
        + TestSignedAndBoundaryValues()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures
                  << " trade confirmation request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
