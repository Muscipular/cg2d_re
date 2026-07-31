#include "packet/PacketBuilder.hpp"
#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::BagPageMoveRequest;

constexpr std::array<std::string_view, 3U> FieldNames = {{
    "fromSlot",
    "toPage",
    "toSlot",
}};

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
    const std::array<std::string, 3U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode("SWITMP"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == FieldNames.size(), message);
    if (packet.fields.size() != FieldNames.size()) {
        return failures;
    }
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(
            packet.fields[index].name == FieldNames[index],
            message);
        failures += Expect(
            packet.fields[index].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(
            packet.fields[index].value == expected[index],
            message);
    }
    return failures;
}

std::string BuildWireWithField(std::size_t fieldIndex, std::string_view token)
{
    constexpr std::array<std::string_view, 3U> ValidTokens = {{
        "a",
        "1",
        "-1",
    }};
    std::string wire = "SWITMP";
    for (std::size_t index = 0U; index < ValidTokens.size(); ++index) {
        wire.push_back(' ');
        wire.append(index == fieldIndex ? token : ValidTokens[index]);
    }
    return wire;
}

int TestSpecAndRoundtrip()
{
    const BagPageMoveRequest request(10, 1, -1);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(
        BagPageMoveRequest::Code == "SWITMP",
        "SWITMP code");
    failures += Expect(
        BagPageMoveRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "SWITMP direction");
    failures += Expect(
        BagPageMoveRequest::Fields.size() == FieldNames.size(),
        "SWITMP field count");
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(
            BagPageMoveRequest::Fields[index].name == FieldNames[index],
            "SWITMP field name order");
        failures += Expect(
            BagPageMoveRequest::Fields[index].type ==
                cg::packet::PacketFieldType::Int62,
            "SWITMP field type order");
    }
    failures += Expect(
        BagPageMoveRequest::Spec.code == BagPageMoveRequest::Code,
        "SWITMP Spec code");
    failures += Expect(
        BagPageMoveRequest::Spec.direction == BagPageMoveRequest::Direction,
        "SWITMP Spec direction");
    failures += Expect(
        BagPageMoveRequest::Spec.fieldCount == FieldNames.size(),
        "SWITMP Spec field count");
    failures += Expect(request.fromSlot == 10, "SWITMP fromSlot member");
    failures += Expect(request.toPage == 1, "SWITMP toPage member");
    failures += Expect(request.toSlot == -1, "SWITMP toSlot member");
    failures += ExpectPacket(
        packet,
        {{"10", "1", "-1"}},
        "SWITMP ToPacket");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "SWITMP Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "SWITMP a 1 -1 ",
            "SWITMP canonical");
    }

    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(
            "SWITMP a 1 -1",
            BagPageMoveRequest::Spec);
    failures += Expect(parsed.HasValue(), "SWITMP generic parse");
    if (parsed.HasValue()) {
        failures += ExpectPacket(
            parsed.Value(),
            {{"10", "1", "-1"}},
            "SWITMP generic packet");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(
                parsed.Value(),
                BagPageMoveRequest::Spec);
        failures += Expect(rebuilt.HasValue(), "SWITMP generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(
                rebuilt.Value() == "SWITMP a 1 -1 ",
                "SWITMP generic canonical");
        }
    }
    return failures;
}

int TestSignedValues()
{
    struct Case
    {
        int fromSlot;
        int toPage;
        int toSlot;
        std::string_view canonical;
    };

    constexpr Case Cases[] = {
        {10, 1, -1, "SWITMP a 1 -1 "},
        {0, 0, 0, "SWITMP 0 0 0 "},
        {9, 4, 1, "SWITMP 9 4 1 "},
        {10, 10, 10, "SWITMP a a a "},
        {-1, -1, -1, "SWITMP -1 -1 -1 "},
        {
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            -123,
            "SWITMP -2lkCB2 2lkCB1 -1Z ",
        },
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const BagPageMoveRequest request(
            testCase.fromSlot,
            testCase.toPage,
            testCase.toSlot);
        failures += ExpectPacket(
            request.ToPacket(),
            {{
                std::to_string(testCase.fromSlot),
                std::to_string(testCase.toPage),
                std::to_string(testCase.toSlot),
            }},
            "SWITMP signed packet");
        const cg::packet::PacketResult<std::string> encoded =
            request.Encode();
        failures += Expect(encoded.HasValue(), "SWITMP signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.canonical,
                "SWITMP signed canonical");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket(
            "bad a 1 -1",
            BagPageMoveRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "SWITMP wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "SWITMP a 1",
            BagPageMoveRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "SWITMP",
        true,
        2U,
        "SWITMP missing field");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "SWITMP a 1 -1 0",
            BagPageMoveRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "SWITMP",
        true,
        3U,
        "SWITMP extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildWireWithField(index, "_"),
                BagPageMoveRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "SWITMP",
            true,
            index,
            "SWITMP invalid i62 field");
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildWireWithField(index, "2lkCB2"),
                BagPageMoveRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "SWITMP",
            true,
            index,
            "SWITMP positive overflow field");
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildWireWithField(index, "-2lkCB3"),
                BagPageMoveRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "SWITMP",
            true,
            index,
            "SWITMP negative overflow field");
    }

    const std::string pollutedCode = "SWITMP\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedCode + " a 1 -1",
            BagPageMoveRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "SWITMP polluted code");
    const std::string pollutedField = "SWITMP a 1 -1\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedField,
            BagPageMoveRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "SWITMP",
        true,
        3U,
        "SWITMP polluted field");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;
    cg::packet::Packet wrongCode =
        BagPageMoveRequest(10, 1, -1).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        cg::packet::BuildPacket(wrongCode, BagPageMoveRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "SWITMP Build wrong code");

    cg::packet::Packet wrongDirection =
        BagPageMoveRequest(10, 1, -1).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        cg::packet::BuildPacket(
            wrongDirection,
            BagPageMoveRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "SWITMP",
        false,
        0U,
        "SWITMP Build wrong direction");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        cg::packet::Packet wrongName =
            BagPageMoveRequest(10, 1, -1).ToPacket();
        wrongName.fields[index].name = "wrong";
        failures += ExpectError(
            cg::packet::BuildPacket(
                wrongName,
                BagPageMoveRequest::Spec),
            cg::packet::PacketErrorCode::EncodeFailed,
            "SWITMP",
            true,
            index,
            "SWITMP Build wrong field name");

        cg::packet::Packet wrongType =
            BagPageMoveRequest(10, 1, -1).ToPacket();
        wrongType.fields[index].type =
            cg::packet::PacketFieldType::EscapedString;
        failures += ExpectError(
            cg::packet::BuildPacket(
                wrongType,
                BagPageMoveRequest::Spec),
            cg::packet::PacketErrorCode::EncodeFailed,
            "SWITMP",
            true,
            index,
            "SWITMP Build wrong field type");

        cg::packet::Packet emptyValue =
            BagPageMoveRequest(10, 1, -1).ToPacket();
        emptyValue.fields[index].value.clear();
        failures += ExpectError(
            cg::packet::BuildPacket(
                emptyValue,
                BagPageMoveRequest::Spec),
            cg::packet::PacketErrorCode::EncodeFailed,
            "SWITMP",
            true,
            index,
            "SWITMP Build empty decimal");

        cg::packet::Packet invalidValue =
            BagPageMoveRequest(10, 1, -1).ToPacket();
        invalidValue.fields[index].value = "not-an-int";
        failures += ExpectError(
            cg::packet::BuildPacket(
                invalidValue,
                BagPageMoveRequest::Spec),
            cg::packet::PacketErrorCode::EncodeFailed,
            "SWITMP",
            true,
            index,
            "SWITMP Build non-decimal");

        cg::packet::Packet overflow =
            BagPageMoveRequest(10, 1, -1).ToPacket();
        overflow.fields[index].value = "2147483648";
        failures += ExpectError(
            cg::packet::BuildPacket(
                overflow,
                BagPageMoveRequest::Spec),
            cg::packet::PacketErrorCode::EncodeFailed,
            "SWITMP",
            true,
            index,
            "SWITMP Build overflow");
    }
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
                  << " bag page move request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
