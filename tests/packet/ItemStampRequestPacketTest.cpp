#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::ItemStampRequest;

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

int ExpectField(
    const cg::packet::Packet& packet,
    std::size_t index,
    std::string_view name,
    std::string_view value,
    const char* message)
{
    int failures = Expect(index < packet.fields.size(), message);
    if (index >= packet.fields.size()) {
        return failures;
    }
    failures += Expect(packet.fields[index].name == name, message);
    failures += Expect(
        packet.fields[index].type == cg::packet::PacketFieldType::Int62,
        message);
    failures += Expect(packet.fields[index].value == value, message);
    return failures;
}

int TestSpecAndEncode()
{
    const ItemStampRequest request(10, 20);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(ItemStampRequest::Code == "BG", "BG code");
    failures += Expect(
        ItemStampRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "BG direction");
    failures += Expect(ItemStampRequest::Fields.size() == 2U, "BG fields");
    failures += Expect(
        ItemStampRequest::Fields[0].name == "targetItemSlot",
        "BG targetItemSlot name");
    failures += Expect(
        ItemStampRequest::Fields[1].name == "stampItemSlot",
        "BG stampItemSlot name");
    for (const cg::packet::PacketFieldSpec& field : ItemStampRequest::Fields) {
        failures += Expect(
            field.type == cg::packet::PacketFieldType::Int62,
            "BG field type");
    }
    failures += Expect(
        ItemStampRequest::Spec.code == ItemStampRequest::Code,
        "BG Spec code");
    failures += Expect(
        ItemStampRequest::Spec.direction == ItemStampRequest::Direction,
        "BG Spec direction");
    failures += Expect(
        ItemStampRequest::Spec.fieldCount == 2U,
        "BG Spec field count");
    failures += Expect(request.targetItemSlot == 10, "BG targetItemSlot member");
    failures += Expect(request.stampItemSlot == 20, "BG stampItemSlot member");
    failures += Expect(packet.code == cg::packet::PacketCode("BG"), "BG packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "BG packet direction");
    failures += Expect(packet.fields.size() == 2U, "BG packet fields");
    failures += ExpectField(
        packet,
        0U,
        "targetItemSlot",
        "10",
        "BG targetItemSlot field");
    failures += ExpectField(
        packet,
        1U,
        "stampItemSlot",
        "20",
        "BG stampItemSlot field");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "BG Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "BG a k ", "BG Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("BG a k", ItemStampRequest::Spec);

    int failures = Expect(parsed.HasValue(), "BG generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectField(
        parsed.Value(),
        0U,
        "targetItemSlot",
        "10",
        "BG parsed targetItemSlot");
    failures += ExpectField(
        parsed.Value(),
        1U,
        "stampItemSlot",
        "20",
        "BG parsed stampItemSlot");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), ItemStampRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "BG generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "BG a k ", "BG generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int targetItemSlot;
        int stampItemSlot;
        std::string_view expected;
    };

    // INT_MIN/INT_MAX 仅证明 cg3a codec 保留完整 signed int。
    constexpr Case Cases[] = {
        {0, -1, "BG 0 -1 "},
        {-1, 0, "BG -1 0 "},
        {10, 20, "BG a k "},
        {100, -100, "BG 1C -1C "},
        {
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            "BG -2lkCB2 2lkCB1 ",
        },
        {
            std::numeric_limits<int>::max(),
            std::numeric_limits<int>::min(),
            "BG 2lkCB1 -2lkCB2 ",
        },
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const ItemStampRequest request(
            testCase.targetItemSlot,
            testCase.stampItemSlot);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectField(
            packet,
            0U,
            "targetItemSlot",
            std::to_string(testCase.targetItemSlot),
            "BG signed targetItemSlot");
        failures += ExpectField(
            packet,
            1U,
            "stampItemSlot",
            std::to_string(testCase.stampItemSlot),
            "BG signed stampItemSlot");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "BG signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "BG signed canonical");
        }
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 2U>& tokens)
{
    return "BG " + tokens[0] + ' ' + tokens[1] + ' ';
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a k ", ItemStampRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "BG wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("BG", ItemStampRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "BG",
        true,
        0U,
        "BG missing both fields");
    failures += ExpectError(
        cg::packet::ParsePacket("BG a", ItemStampRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "BG",
        true,
        1U,
        "BG missing stampItemSlot");
    failures += ExpectError(
        cg::packet::ParsePacket("BG a k 0 ", ItemStampRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "BG",
        true,
        2U,
        "BG extra field");

    const std::array<std::string, 2U> validTokens = {{"a", "k"}};
    for (std::size_t index = 0U; index < validTokens.size(); ++index) {
        std::array<std::string, 2U> invalidTokens = validTokens;
        invalidTokens[index] = "_";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(invalidTokens),
                ItemStampRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "BG",
            true,
            index,
            "BG invalid i62");

        std::array<std::string, 2U> positiveOverflowTokens = validTokens;
        positiveOverflowTokens[index] = "2lkCB2";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(positiveOverflowTokens),
                ItemStampRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "BG",
            true,
            index,
            "BG positive i62 overflow");

        std::array<std::string, 2U> negativeOverflowTokens = validTokens;
        negativeOverflowTokens[index] = "-2lkCB3";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(negativeOverflowTokens),
                ItemStampRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "BG",
            true,
            index,
            "BG negative i62 overflow");
    }

    const std::string pollutedCode = "BG\x17" "x";
    const std::string pollutedWire = pollutedCode + " a k";
    failures += ExpectError(
        cg::packet::ParsePacket(pollutedWire, ItemStampRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "BG polluted code");

    const std::string pollutedTarget = "BG a\x17" "x k";
    failures += ExpectError(
        cg::packet::ParsePacket(pollutedTarget, ItemStampRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "BG",
        true,
        2U,
        "BG polluted targetItemSlot");

    const std::string pollutedStamp = "BG a k\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(pollutedStamp, ItemStampRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "BG",
        true,
        2U,
        "BG polluted stampItemSlot");
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
        std::cerr << failures << " item stamp request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
