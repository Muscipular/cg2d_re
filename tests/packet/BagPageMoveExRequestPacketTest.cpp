#include "packet/PacketBuilder.hpp"
#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::BagPageMoveExRequest;

constexpr std::array<std::string_view, 4U> FieldNames = {{
    "fromPage",
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
    const std::array<std::string, 4U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode("SWITMP2"),
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
    constexpr std::array<std::string_view, 4U> ValidTokens = {{
        "0",
        "8",
        "1",
        "-1",
    }};
    std::string wire = "SWITMP2";
    for (std::size_t index = 0U; index < ValidTokens.size(); ++index) {
        wire.push_back(' ');
        wire.append(index == fieldIndex ? token : ValidTokens[index]);
    }
    return wire;
}

int TestSpecAndRoundtrip()
{
    const BagPageMoveExRequest request(0, 8, 1, -1);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(
        BagPageMoveExRequest::Code == "SWITMP2",
        "SWITMP2 code");
    failures += Expect(
        BagPageMoveExRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "SWITMP2 direction");
    failures += Expect(
        BagPageMoveExRequest::Fields.size() == FieldNames.size(),
        "SWITMP2 field count");
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(
            BagPageMoveExRequest::Fields[index].name == FieldNames[index],
            "SWITMP2 field name order");
        failures += Expect(
            BagPageMoveExRequest::Fields[index].type ==
                cg::packet::PacketFieldType::Int62,
            "SWITMP2 field type order");
    }
    failures += Expect(
        BagPageMoveExRequest::Spec.code == BagPageMoveExRequest::Code,
        "SWITMP2 Spec code");
    failures += Expect(
        BagPageMoveExRequest::Spec.direction ==
            BagPageMoveExRequest::Direction,
        "SWITMP2 Spec direction");
    failures += Expect(
        BagPageMoveExRequest::Spec.fieldCount == FieldNames.size(),
        "SWITMP2 Spec field count");
    failures += Expect(request.fromPage == 0, "SWITMP2 fromPage member");
    failures += Expect(request.fromSlot == 8, "SWITMP2 fromSlot member");
    failures += Expect(request.toPage == 1, "SWITMP2 toPage member");
    failures += Expect(request.toSlot == -1, "SWITMP2 toSlot member");
    failures += ExpectPacket(
        packet,
        {{"0", "8", "1", "-1"}},
        "SWITMP2 ToPacket");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "SWITMP2 Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "SWITMP2 0 8 1 -1 ",
            "SWITMP2 canonical");
    }

    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(
            "SWITMP2 0 8 1 -1",
            BagPageMoveExRequest::Spec);
    failures += Expect(parsed.HasValue(), "SWITMP2 generic parse");
    if (parsed.HasValue()) {
        failures += ExpectPacket(
            parsed.Value(),
            {{"0", "8", "1", "-1"}},
            "SWITMP2 generic packet");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(
                parsed.Value(),
                BagPageMoveExRequest::Spec);
        failures += Expect(rebuilt.HasValue(), "SWITMP2 generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(
                rebuilt.Value() == "SWITMP2 0 8 1 -1 ",
                "SWITMP2 generic canonical");
        }
    }
    return failures;
}

int TestSignedValues()
{
    struct Case
    {
        int fromPage;
        int fromSlot;
        int toPage;
        int toSlot;
        std::string_view canonical;
    };

    constexpr Case Cases[] = {
        {0, 8, 1, -1, "SWITMP2 0 8 1 -1 "},
        {0, 0, 0, 0, "SWITMP2 0 0 0 0 "},
        {4, 9, 4, 1, "SWITMP2 4 9 4 1 "},
        {10, 10, 10, 10, "SWITMP2 a a a a "},
        {-1, -1, -1, -1, "SWITMP2 -1 -1 -1 -1 "},
        {
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            -123,
            456,
            "SWITMP2 -2lkCB2 2lkCB1 -1Z 7m ",
        },
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const BagPageMoveExRequest request(
            testCase.fromPage,
            testCase.fromSlot,
            testCase.toPage,
            testCase.toSlot);
        failures += ExpectPacket(
            request.ToPacket(),
            {{
                std::to_string(testCase.fromPage),
                std::to_string(testCase.fromSlot),
                std::to_string(testCase.toPage),
                std::to_string(testCase.toSlot),
            }},
            "SWITMP2 signed packet");
        const cg::packet::PacketResult<std::string> encoded =
            request.Encode();
        failures += Expect(encoded.HasValue(), "SWITMP2 signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.canonical,
                "SWITMP2 signed canonical");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket(
            "bad 0 8 1 -1",
            BagPageMoveExRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "SWITMP2 wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "SWITMP2 0 8 1",
            BagPageMoveExRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "SWITMP2",
        true,
        3U,
        "SWITMP2 missing field");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "SWITMP2 0 8 1 -1 0",
            BagPageMoveExRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "SWITMP2",
        true,
        4U,
        "SWITMP2 extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildWireWithField(index, "_"),
                BagPageMoveExRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "SWITMP2",
            true,
            index,
            "SWITMP2 invalid i62 field");
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildWireWithField(index, "2lkCB2"),
                BagPageMoveExRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "SWITMP2",
            true,
            index,
            "SWITMP2 positive overflow field");
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildWireWithField(index, "-2lkCB3"),
                BagPageMoveExRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "SWITMP2",
            true,
            index,
            "SWITMP2 negative overflow field");
    }

    const std::string pollutedCode = "SWITMP2\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedCode + " 0 8 1 -1",
            BagPageMoveExRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "SWITMP2 polluted code");
    const std::string pollutedField = "SWITMP2 0 8 1 -1\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedField,
            BagPageMoveExRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "SWITMP2",
        true,
        4U,
        "SWITMP2 polluted field");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;
    cg::packet::Packet wrongCode =
        BagPageMoveExRequest(0, 8, 1, -1).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        cg::packet::BuildPacket(wrongCode, BagPageMoveExRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "SWITMP2 Build wrong code");

    cg::packet::Packet wrongDirection =
        BagPageMoveExRequest(0, 8, 1, -1).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        cg::packet::BuildPacket(
            wrongDirection,
            BagPageMoveExRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "SWITMP2",
        false,
        0U,
        "SWITMP2 Build wrong direction");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        cg::packet::Packet wrongName =
            BagPageMoveExRequest(0, 8, 1, -1).ToPacket();
        wrongName.fields[index].name = "wrong";
        failures += ExpectError(
            cg::packet::BuildPacket(
                wrongName,
                BagPageMoveExRequest::Spec),
            cg::packet::PacketErrorCode::EncodeFailed,
            "SWITMP2",
            true,
            index,
            "SWITMP2 Build wrong field name");

        cg::packet::Packet wrongType =
            BagPageMoveExRequest(0, 8, 1, -1).ToPacket();
        wrongType.fields[index].type =
            cg::packet::PacketFieldType::EscapedString;
        failures += ExpectError(
            cg::packet::BuildPacket(
                wrongType,
                BagPageMoveExRequest::Spec),
            cg::packet::PacketErrorCode::EncodeFailed,
            "SWITMP2",
            true,
            index,
            "SWITMP2 Build wrong field type");

        cg::packet::Packet invalidValue =
            BagPageMoveExRequest(0, 8, 1, -1).ToPacket();
        invalidValue.fields[index].value = "not-an-int";
        failures += ExpectError(
            cg::packet::BuildPacket(
                invalidValue,
                BagPageMoveExRequest::Spec),
            cg::packet::PacketErrorCode::EncodeFailed,
            "SWITMP2",
            true,
            index,
            "SWITMP2 Build non-decimal");

        cg::packet::Packet overflow =
            BagPageMoveExRequest(0, 8, 1, -1).ToPacket();
        overflow.fields[index].value = "2147483648";
        failures += ExpectError(
            cg::packet::BuildPacket(
                overflow,
                BagPageMoveExRequest::Spec),
            cg::packet::PacketErrorCode::EncodeFailed,
            "SWITMP2",
            true,
            index,
            "SWITMP2 Build overflow");
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
                  << " bag page move ex request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
