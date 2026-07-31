#include "packet/PacketBuilder.hpp"
#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::PetItemMoveRequest;

constexpr std::array<std::string_view, 5U> FieldNames = {{
    "petSlotFrom",
    "itemSlotFrom",
    "petSlotTo",
    "itemSlotTo",
    "pile",
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
    const std::array<std::string, 5U>& expected,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode("Cpa"),
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
    constexpr std::array<std::string_view, 5U> ValidTokens = {{
        "0",
        "9",
        "2",
        "1",
        "-1",
    }};
    std::string wire = "Cpa";
    for (std::size_t index = 0U; index < ValidTokens.size(); ++index) {
        wire.push_back(' ');
        wire.append(index == fieldIndex ? token : ValidTokens[index]);
    }
    return wire;
}

int TestSpecAndRoundtrip()
{
    const PetItemMoveRequest request(0, 9, 2, 1, -1);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(PetItemMoveRequest::Code == "Cpa", "Cpa code");
    failures += Expect(
        PetItemMoveRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "Cpa direction");
    failures += Expect(
        PetItemMoveRequest::Fields.size() == FieldNames.size(),
        "Cpa field count");
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(
            PetItemMoveRequest::Fields[index].name == FieldNames[index],
            "Cpa field name order");
        failures += Expect(
            PetItemMoveRequest::Fields[index].type ==
                cg::packet::PacketFieldType::Int62,
            "Cpa field type order");
    }
    failures += Expect(
        PetItemMoveRequest::Spec.code == PetItemMoveRequest::Code,
        "Cpa Spec code");
    failures += Expect(
        PetItemMoveRequest::Spec.direction == PetItemMoveRequest::Direction,
        "Cpa Spec direction");
    failures += Expect(
        PetItemMoveRequest::Spec.fieldCount == FieldNames.size(),
        "Cpa Spec field count");
    failures += Expect(request.petSlotFrom == 0, "Cpa petSlotFrom member");
    failures += Expect(request.itemSlotFrom == 9, "Cpa itemSlotFrom member");
    failures += Expect(request.petSlotTo == 2, "Cpa petSlotTo member");
    failures += Expect(request.itemSlotTo == 1, "Cpa itemSlotTo member");
    failures += Expect(request.pile == -1, "Cpa pile member");
    failures += ExpectPacket(
        packet,
        {{"0", "9", "2", "1", "-1"}},
        "Cpa ToPacket");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "Cpa Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "Cpa 0 9 2 1 -1 ",
            "Cpa canonical");
    }

    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(
            "Cpa 0 9 2 1 -1",
            PetItemMoveRequest::Spec);
    failures += Expect(parsed.HasValue(), "Cpa generic parse");
    if (parsed.HasValue()) {
        failures += ExpectPacket(
            parsed.Value(),
            {{"0", "9", "2", "1", "-1"}},
            "Cpa generic packet");
        const cg::packet::PacketResult<std::string> rebuilt =
            cg::packet::BuildPacket(
                parsed.Value(),
                PetItemMoveRequest::Spec);
        failures += Expect(rebuilt.HasValue(), "Cpa generic build");
        if (rebuilt.HasValue()) {
            failures += Expect(
                rebuilt.Value() == "Cpa 0 9 2 1 -1 ",
                "Cpa generic canonical");
        }
    }
    return failures;
}

int TestPileAndBoundaryValues()
{
    struct PileCase
    {
        int pile;
        std::string_view canonical;
    };
    constexpr PileCase PileCases[] = {
        {-1, "Cpa 0 9 2 1 -1 "},
        {0, "Cpa 0 9 2 1 0 "},
        {1, "Cpa 0 9 2 1 1 "},
        {42, "Cpa 0 9 2 1 G "},
        {std::numeric_limits<int>::min(), "Cpa 0 9 2 1 -2lkCB2 "},
        {std::numeric_limits<int>::max(), "Cpa 0 9 2 1 2lkCB1 "},
    };

    int failures = 0;
    for (const PileCase& testCase : PileCases) {
        const PetItemMoveRequest request(0, 9, 2, 1, testCase.pile);
        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "Cpa pile Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.canonical,
                "Cpa pile wire fidelity");
        }
    }

    const PetItemMoveRequest extremes(
        std::numeric_limits<int>::min(),
        std::numeric_limits<int>::max(),
        -123,
        456,
        -1);
    failures += ExpectPacket(
        extremes.ToPacket(),
        {{
            std::to_string(std::numeric_limits<int>::min()),
            std::to_string(std::numeric_limits<int>::max()),
            "-123",
            "456",
            "-1",
        }},
        "Cpa arbitrary ToPacket");
    const cg::packet::PacketResult<std::string> encoded = extremes.Encode();
    failures += Expect(encoded.HasValue(), "Cpa arbitrary Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "Cpa -2lkCB2 2lkCB1 -1Z 7m -1 ",
            "Cpa arbitrary canonical");
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket(
            "bad 0 9 2 1 -1",
            PetItemMoveRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "Cpa wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "Cpa 0 9 2 1",
            PetItemMoveRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Cpa",
        true,
        4U,
        "Cpa missing field");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "Cpa 0 9 2 1 -1 0",
            PetItemMoveRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Cpa",
        true,
        5U,
        "Cpa extra field");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildWireWithField(index, "_"),
                PetItemMoveRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "Cpa",
            true,
            index,
            "Cpa invalid i62 field");
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildWireWithField(index, "2lkCB2"),
                PetItemMoveRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "Cpa",
            true,
            index,
            "Cpa positive overflow field");
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildWireWithField(index, "-2lkCB3"),
                PetItemMoveRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "Cpa",
            true,
            index,
            "Cpa negative overflow field");
    }

    const std::string pollutedCode = "Cpa\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedCode + " 0 9 2 1 -1",
            PetItemMoveRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "Cpa polluted code");
    const std::string pollutedField = "Cpa 0 9 2 1 -1\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedField,
            PetItemMoveRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Cpa",
        true,
        5U,
        "Cpa polluted field");
    return failures;
}

int TestBuildErrors()
{
    int failures = 0;
    cg::packet::Packet wrongCode =
        PetItemMoveRequest(0, 9, 2, 1, -1).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        cg::packet::BuildPacket(wrongCode, PetItemMoveRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "Cpa Build wrong code");

    cg::packet::Packet wrongDirection =
        PetItemMoveRequest(0, 9, 2, 1, -1).ToPacket();
    wrongDirection.direction = cg::packet::PacketDirection::ServerToClient;
    failures += ExpectError(
        cg::packet::BuildPacket(
            wrongDirection,
            PetItemMoveRequest::Spec),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Cpa",
        false,
        0U,
        "Cpa Build wrong direction");

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        cg::packet::Packet wrongName =
            PetItemMoveRequest(0, 9, 2, 1, -1).ToPacket();
        wrongName.fields[index].name = "wrong";
        failures += ExpectError(
            cg::packet::BuildPacket(wrongName, PetItemMoveRequest::Spec),
            cg::packet::PacketErrorCode::EncodeFailed,
            "Cpa",
            true,
            index,
            "Cpa Build wrong field name");

        cg::packet::Packet wrongType =
            PetItemMoveRequest(0, 9, 2, 1, -1).ToPacket();
        wrongType.fields[index].type =
            cg::packet::PacketFieldType::EscapedString;
        failures += ExpectError(
            cg::packet::BuildPacket(wrongType, PetItemMoveRequest::Spec),
            cg::packet::PacketErrorCode::EncodeFailed,
            "Cpa",
            true,
            index,
            "Cpa Build wrong field type");

        cg::packet::Packet invalidValue =
            PetItemMoveRequest(0, 9, 2, 1, -1).ToPacket();
        invalidValue.fields[index].value = "not-an-int";
        failures += ExpectError(
            cg::packet::BuildPacket(
                invalidValue,
                PetItemMoveRequest::Spec),
            cg::packet::PacketErrorCode::EncodeFailed,
            "Cpa",
            true,
            index,
            "Cpa Build non-decimal");

        cg::packet::Packet overflow =
            PetItemMoveRequest(0, 9, 2, 1, -1).ToPacket();
        overflow.fields[index].value = "2147483648";
        failures += ExpectError(
            cg::packet::BuildPacket(overflow, PetItemMoveRequest::Spec),
            cg::packet::PacketErrorCode::EncodeFailed,
            "Cpa",
            true,
            index,
            "Cpa Build overflow");
    }
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndRoundtrip()
        + TestPileAndBoundaryValues()
        + TestParseErrors()
        + TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures
                  << " pet item move request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
