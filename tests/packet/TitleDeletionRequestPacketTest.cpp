#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::TitleDeletionRequest;

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
        packet.code == cg::packet::PacketCode("tj"),
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
    const TitleDeletionRequest request(10);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(TitleDeletionRequest::Code == "tj", "tj code");
    failures += Expect(
        TitleDeletionRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "tj direction");
    failures += Expect(
        TitleDeletionRequest::Fields.size() == 1U,
        "tj fields");
    failures += Expect(
        TitleDeletionRequest::Fields[0].name == "titleSlot",
        "tj field name");
    failures += Expect(
        TitleDeletionRequest::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "tj field type");
    failures += Expect(
        TitleDeletionRequest::Spec.code == TitleDeletionRequest::Code,
        "tj Spec code");
    failures += Expect(
        TitleDeletionRequest::Spec.direction ==
            TitleDeletionRequest::Direction,
        "tj Spec direction");
    failures += Expect(
        TitleDeletionRequest::Spec.fieldCount == 1U,
        "tj Spec field count");
    failures += Expect(request.titleSlot == 10, "tj member");
    failures += ExpectPacket(packet, "10", "tj packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "tj Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "tj a ", "tj canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("tj a", TitleDeletionRequest::Spec);

    int failures = Expect(parsed.HasValue(), "tj generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(parsed.Value(), "10", "tj generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(
            parsed.Value(),
            TitleDeletionRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "tj generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "tj a ", "tj generic canonical");
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

    // producer 使用非负槽位；负数与 int 边界仅证明 cg3a codec。
    constexpr Case Cases[] = {
        {0, "tj 0 "},
        {9, "tj 9 "},
        {10, "tj a "},
        {-1, "tj -1 "},
        {100, "tj 1C "},
        {std::numeric_limits<int>::min(), "tj -2lkCB2 "},
        {std::numeric_limits<int>::max(), "tj 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const TitleDeletionRequest request(testCase.titleSlot);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.titleSlot),
            "tj signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "tj signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "tj signed canonical");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a ", TitleDeletionRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "tj wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("tj", TitleDeletionRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "tj",
        true,
        0U,
        "tj missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("tj a 0 ", TitleDeletionRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "tj",
        true,
        1U,
        "tj extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("tj _ ", TitleDeletionRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "tj",
        true,
        0U,
        "tj invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("tj 2lkCB2 ", TitleDeletionRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "tj",
        true,
        0U,
        "tj positive overflow");
    failures += ExpectError(
        cg::packet::ParsePacket("tj -2lkCB3 ", TitleDeletionRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "tj",
        true,
        0U,
        "tj negative overflow");

    const std::string pollutedCode = "tj\x17" "a";
    failures += ExpectError(
        cg::packet::ParsePacket(pollutedCode, TitleDeletionRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "tj polluted code");

    const std::string pollutedField = "tj a\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(pollutedField, TitleDeletionRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "tj",
        true,
        1U,
        "tj polluted field");
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
                  << " title deletion request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
