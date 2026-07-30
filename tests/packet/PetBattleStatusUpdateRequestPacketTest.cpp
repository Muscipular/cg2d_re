#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::PetBattleStatusUpdateRequest;

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
    const std::array<std::string_view, 5U>& values,
    const char* message)
{
    constexpr std::array<std::string_view, 5U> FieldNames = {{
        "petStatus0",
        "petStatus1",
        "petStatus2",
        "petStatus3",
        "petStatus4",
    }};

    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode(PetBattleStatusUpdateRequest::Code),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == FieldNames.size(), message);
    if (packet.fields.size() != FieldNames.size()) {
        return failures;
    }

    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(packet.fields[index].name == FieldNames[index], message);
        failures += Expect(
            packet.fields[index].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[index].value == values[index], message);
    }
    return failures;
}

int TestSpecAndEncode()
{
    constexpr std::array<std::string_view, 5U> FieldNames = {{
        "petStatus0",
        "petStatus1",
        "petStatus2",
        "petStatus3",
        "petStatus4",
    }};
    const PetBattleStatusUpdateRequest request(
        std::array<int, 5U> {{0, 1, 2, 3, 16}});

    int failures = 0;
    failures += Expect(
        PetBattleStatusUpdateRequest::Code == "LKQy",
        "LKQy code");
    failures += Expect(
        PetBattleStatusUpdateRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "LKQy direction");
    failures += Expect(
        PetBattleStatusUpdateRequest::Fields.size() == FieldNames.size(),
        "LKQy field count");
    for (std::size_t index = 0U; index < FieldNames.size(); ++index) {
        failures += Expect(
            PetBattleStatusUpdateRequest::Fields[index].name ==
                FieldNames[index],
            "LKQy field name");
        failures += Expect(
            PetBattleStatusUpdateRequest::Fields[index].type ==
                cg::packet::PacketFieldType::Int62,
            "LKQy field type");
    }
    failures += Expect(
        PetBattleStatusUpdateRequest::Spec.code ==
            PetBattleStatusUpdateRequest::Code,
        "LKQy Spec code");
    failures += Expect(
        PetBattleStatusUpdateRequest::Spec.direction ==
            PetBattleStatusUpdateRequest::Direction,
        "LKQy Spec direction");
    failures += Expect(
        PetBattleStatusUpdateRequest::Spec.fieldCount == 5U,
        "LKQy Spec field count");
    failures += Expect(
        request.petStatuses == std::array<int, 5U> {{0, 1, 2, 3, 16}},
        "LKQy members");
    failures += ExpectPacket(
        request.ToPacket(),
        {{"0", "1", "2", "3", "16"}},
        "LKQy packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "LKQy Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "LKQy 0 1 2 3 g ",
            "LKQy Encode canonical");
    }
    return failures;
}

int TestArbitraryStatusValues()
{
    constexpr struct
    {
        std::array<int, 5U> values;
        std::string_view expected;
    } Cases[] = {
        {{{3, 3, 3, 3, 3}}, "LKQy 3 3 3 3 3 "},
        {{{16, 17, 18, 19, 20}}, "LKQy g h i j k "},
        {{{-1, -2, -3, -16, -17}}, "LKQy -1 -2 -3 -g -h "},
        {{
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            0,
            -1,
            238328,
        }, "LKQy -2lkCB2 2lkCB1 0 -1 1000 "},
    };

    int failures = 0;
    for (const auto& testCase : Cases) {
        const PetBattleStatusUpdateRequest request(testCase.values);
        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "LKQy arbitrary Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "LKQy arbitrary canonical");
        }
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(
            "LKQy 0 1 2 3 g",
            PetBattleStatusUpdateRequest::Spec);

    int failures = Expect(parsed.HasValue(), "LKQy generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(
        parsed.Value(),
        {{"0", "1", "2", "3", "16"}},
        "LKQy generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(
            parsed.Value(),
            PetBattleStatusUpdateRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "LKQy generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(
            rebuilt.Value() == "LKQy 0 1 2 3 g ",
            "LKQy generic canonical");
    }
    return failures;
}

int TestArityErrors()
{
    constexpr struct
    {
        std::string_view wire;
        std::size_t fieldIndex;
        const char* message;
    } MissingCases[] = {
        {"LKQy", 0U, "LKQy missing all"},
        {"LKQy 0", 1U, "LKQy missing fields 1-4"},
        {"LKQy 0 1", 2U, "LKQy missing fields 2-4"},
        {"LKQy 0 1 2", 3U, "LKQy missing fields 3-4"},
        {"LKQy 0 1 2 3", 4U, "LKQy missing field 4"},
    };

    int failures = ExpectError(
        cg::packet::ParsePacket(
            "bad 0 1 2 3 g",
            PetBattleStatusUpdateRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "LKQy wrong code");
    for (const auto& testCase : MissingCases) {
        failures += ExpectError(
            cg::packet::ParsePacket(
                testCase.wire,
                PetBattleStatusUpdateRequest::Spec),
            cg::packet::PacketErrorCode::FieldCountMismatch,
            "LKQy",
            true,
            testCase.fieldIndex,
            testCase.message);
    }
    failures += ExpectError(
        cg::packet::ParsePacket(
            "LKQy 0 1 2 3 g 5",
            PetBattleStatusUpdateRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "LKQy",
        true,
        5U,
        "LKQy extra field");
    return failures;
}

int TestFieldErrors()
{
    constexpr struct
    {
        std::string_view token;
        const char* label;
    } TokenCases[] = {
        {"_", "invalid"},
        {"2lkCB2", "positive overflow"},
        {"-2lkCB3", "negative overflow"},
    };

    int failures = 0;
    for (const auto& tokenCase : TokenCases) {
        for (std::size_t fieldIndex = 0U; fieldIndex < 5U; ++fieldIndex) {
            std::array<std::string, 5U> fields = {{"0", "1", "2", "3", "g"}};
            fields[fieldIndex] = tokenCase.token;
            const std::string wire =
                "LKQy " + fields[0] + " " + fields[1] + " " + fields[2]
                + " " + fields[3] + " " + fields[4];
            failures += ExpectError(
                cg::packet::ParsePacket(
                    wire,
                    PetBattleStatusUpdateRequest::Spec),
                cg::packet::PacketErrorCode::DecodeFailed,
                "LKQy",
                true,
                fieldIndex,
                tokenCase.label);
        }
    }

    const std::string pollutedCode = "LKQy\x17" "0";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedCode + " 1 2 3 g",
            PetBattleStatusUpdateRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "LKQy polluted code");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndEncode()
        + TestArbitraryStatusValues()
        + TestGenericRoundtrip()
        + TestArityErrors()
        + TestFieldErrors();
    if (failures != 0) {
        std::cerr << failures
                  << " pet battle status update request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
