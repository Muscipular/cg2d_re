#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::TitleSelectionRequest;

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
        packet.code == cg::packet::PacketCode("zGp"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "titleSlot", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == value, message);
    }
    return failures;
}

int TestSpecAndEncode()
{
    const TitleSelectionRequest request(-1);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(TitleSelectionRequest::Code == "zGp", "zGp code");
    failures += Expect(
        TitleSelectionRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "zGp direction");
    failures += Expect(
        TitleSelectionRequest::Fields.size() == 1U,
        "zGp fields");
    failures += Expect(
        TitleSelectionRequest::Fields[0].name == "titleSlot",
        "zGp field name");
    failures += Expect(
        TitleSelectionRequest::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "zGp field type");
    failures += Expect(
        TitleSelectionRequest::Spec.code == TitleSelectionRequest::Code,
        "zGp Spec code");
    failures += Expect(
        TitleSelectionRequest::Spec.direction ==
            TitleSelectionRequest::Direction,
        "zGp Spec direction");
    failures += Expect(
        TitleSelectionRequest::Spec.fieldCount == 1U,
        "zGp Spec field count");
    failures += Expect(request.titleSlot == -1, "zGp member");
    failures += ExpectPacket(packet, "-1", "zGp packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "zGp Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "zGp -1 ", "zGp canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(
            "zGp -1",
            TitleSelectionRequest::Spec);

    int failures = Expect(parsed.HasValue(), "zGp generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(parsed.Value(), "-1", "zGp generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(
            parsed.Value(),
            TitleSelectionRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "zGp generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(
            rebuilt.Value() == "zGp -1 ",
            "zGp generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int titleSlot;
        std::string_view expected;
    };

    // -1/非负值来自 legacy producer；int 边界仅证明 cg3a codec。
    constexpr Case Cases[] = {
        {-1, "zGp -1 "},
        {0, "zGp 0 "},
        {9, "zGp 9 "},
        {10, "zGp a "},
        {100, "zGp 1C "},
        {std::numeric_limits<int>::min(), "zGp -2lkCB2 "},
        {std::numeric_limits<int>::max(), "zGp 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const TitleSelectionRequest request(testCase.titleSlot);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.titleSlot),
            "zGp signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "zGp signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "zGp signed canonical");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad -1 ", TitleSelectionRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "zGp wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("zGp", TitleSelectionRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "zGp",
        true,
        0U,
        "zGp missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("zGp -1 0 ", TitleSelectionRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "zGp",
        true,
        1U,
        "zGp extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("zGp _ ", TitleSelectionRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "zGp",
        true,
        0U,
        "zGp invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("zGp 2lkCB2 ", TitleSelectionRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "zGp",
        true,
        0U,
        "zGp positive overflow");
    failures += ExpectError(
        cg::packet::ParsePacket("zGp -2lkCB3 ", TitleSelectionRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "zGp",
        true,
        0U,
        "zGp negative overflow");

    const std::string pollutedCode = "zGp\x17" "-1";
    failures += ExpectError(
        cg::packet::ParsePacket(pollutedCode, TitleSelectionRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "zGp polluted code");

    const std::string pollutedField = "zGp -1\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(pollutedField, TitleSelectionRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "zGp",
        true,
        1U,
        "zGp polluted field");
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
                  << " title selection request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
