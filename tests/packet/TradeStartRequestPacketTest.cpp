#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::TradeStartRequest;

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
        packet.code == cg::packet::PacketCode("QeuC"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(
            packet.fields[0].name == "tradeListSlot",
            message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == value, message);
    }
    return failures;
}

int TestSpecAndEncode()
{
    const TradeStartRequest request(10);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(
        TradeStartRequest::Code == "QeuC",
        "QeuC code");
    failures += Expect(
        TradeStartRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "QeuC direction");
    failures += Expect(
        TradeStartRequest::Fields.size() == 1U,
        "QeuC fields");
    failures += Expect(
        TradeStartRequest::Fields[0].name == "tradeListSlot",
        "QeuC field name");
    failures += Expect(
        TradeStartRequest::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "QeuC field type");
    failures += Expect(
        TradeStartRequest::Spec.code == TradeStartRequest::Code,
        "QeuC Spec code");
    failures += Expect(
        TradeStartRequest::Spec.direction ==
            TradeStartRequest::Direction,
        "QeuC Spec direction");
    failures += Expect(
        TradeStartRequest::Spec.fieldCount == 1U,
        "QeuC Spec field count");
    failures += Expect(request.tradeListSlot == 10, "QeuC member");
    failures += ExpectPacket(packet, "10", "QeuC packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "QeuC Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "QeuC a ",
            "QeuC Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(
            "QeuC a",
            TradeStartRequest::Spec);

    int failures = Expect(parsed.HasValue(), "QeuC generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(parsed.Value(), "10", "QeuC generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(
            parsed.Value(),
            TradeStartRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "QeuC generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(
            rebuilt.Value() == "QeuC a ",
            "QeuC generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int tradeListSlot;
        std::string_view expected;
    };

    // INT_MIN/INT_MAX 仅证明 cg3a codec 保留完整 signed int。
    constexpr Case Cases[] = {
        {0, "QeuC 0 "},
        {9, "QeuC 9 "},
        {10, "QeuC a "},
        {-1, "QeuC -1 "},
        {100, "QeuC 1C "},
        {std::numeric_limits<int>::min(), "QeuC -2lkCB2 "},
        {std::numeric_limits<int>::max(), "QeuC 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const TradeStartRequest request(testCase.tradeListSlot);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.tradeListSlot),
            "QeuC signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "QeuC signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "QeuC signed canonical");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket(
            "bad a ",
            TradeStartRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "QeuC wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "QeuC",
            TradeStartRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "QeuC",
        true,
        0U,
        "QeuC missing field");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "QeuC a 0 ",
            TradeStartRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "QeuC",
        true,
        1U,
        "QeuC extra field");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "QeuC _ ",
            TradeStartRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "QeuC",
        true,
        0U,
        "QeuC invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "QeuC 2lkCB2 ",
            TradeStartRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "QeuC",
        true,
        0U,
        "QeuC positive i62 overflow");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "QeuC -2lkCB3 ",
            TradeStartRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "QeuC",
        true,
        0U,
        "QeuC negative i62 overflow");

    const std::string pollutedCode = "QeuC\x17" "a";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedCode,
            TradeStartRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "QeuC polluted code");

    const std::string pollutedField = "QeuC a\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedField,
            TradeStartRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "QeuC",
        true,
        1U,
        "QeuC polluted field");
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
                  << " trade start request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
