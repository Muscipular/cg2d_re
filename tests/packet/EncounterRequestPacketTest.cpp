#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::EncounterRequest;

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
    const EncounterRequest request(10, 20);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(EncounterRequest::Code == "KeOX", "KeOX code");
    failures += Expect(
        EncounterRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "KeOX direction");
    failures += Expect(EncounterRequest::Fields.size() == 2U, "KeOX fields");
    failures += Expect(EncounterRequest::Fields[0].name == "x", "KeOX x name");
    failures += Expect(EncounterRequest::Fields[1].name == "y", "KeOX y name");
    for (const cg::packet::PacketFieldSpec& field : EncounterRequest::Fields) {
        failures += Expect(
            field.type == cg::packet::PacketFieldType::Int62,
            "KeOX field type");
    }
    failures += Expect(EncounterRequest::Spec.code == EncounterRequest::Code, "KeOX Spec code");
    failures += Expect(
        EncounterRequest::Spec.direction == EncounterRequest::Direction,
        "KeOX Spec direction");
    failures += Expect(EncounterRequest::Spec.fieldCount == 2U, "KeOX Spec field count");
    failures += Expect(request.x == 10, "KeOX x member");
    failures += Expect(request.y == 20, "KeOX y member");
    failures += Expect(packet.code == cg::packet::PacketCode("KeOX"), "KeOX packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "KeOX packet direction");
    failures += Expect(packet.fields.size() == 2U, "KeOX packet fields");
    failures += ExpectField(packet, 0U, "x", "10", "KeOX x field");
    failures += ExpectField(packet, 1U, "y", "20", "KeOX y field");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "KeOX Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "KeOX a k ",
            "KeOX Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("KeOX a k", EncounterRequest::Spec);

    int failures = Expect(parsed.HasValue(), "KeOX generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectField(parsed.Value(), 0U, "x", "10", "KeOX parsed x");
    failures += ExpectField(parsed.Value(), 1U, "y", "20", "KeOX parsed y");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), EncounterRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "KeOX generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(
            rebuilt.Value() == "KeOX a k ",
            "KeOX generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int x;
        int y;
        std::string_view expected;
    };

    constexpr Case Cases[] = {
        {0, -1, "KeOX 0 -1 "},
        {
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            "KeOX -2lkCB2 2lkCB1 ",
        },
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const EncounterRequest request(testCase.x, testCase.y);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectField(
            packet,
            0U,
            "x",
            std::to_string(testCase.x),
            "KeOX signed x");
        failures += ExpectField(
            packet,
            1U,
            "y",
            std::to_string(testCase.y),
            "KeOX signed y");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "KeOX signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "KeOX signed canonical");
        }
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 2U>& tokens)
{
    return "KeOX " + tokens[0] + ' ' + tokens[1] + ' ';
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a k ", EncounterRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "KeOX wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("KeOX", EncounterRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "KeOX",
        true,
        0U,
        "KeOX missing both fields");
    failures += ExpectError(
        cg::packet::ParsePacket("KeOX a", EncounterRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "KeOX",
        true,
        1U,
        "KeOX missing y");
    failures += ExpectError(
        cg::packet::ParsePacket("KeOX a k 3 ", EncounterRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "KeOX",
        true,
        2U,
        "KeOX extra field");

    const std::array<std::string, 2U> validTokens = {{"a", "k"}};
    for (std::size_t index = 0U; index < validTokens.size(); ++index) {
        std::array<std::string, 2U> invalidTokens = validTokens;
        invalidTokens[index] = "_";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(invalidTokens), EncounterRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "KeOX",
            true,
            index,
            "KeOX invalid i62");

        std::array<std::string, 2U> overflowTokens = validTokens;
        overflowTokens[index] = "2lkCB2";
        failures += ExpectError(
            cg::packet::ParsePacket(BuildPlain(overflowTokens), EncounterRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "KeOX",
            true,
            index,
            "KeOX i62 overflow");
    }
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
        std::cerr << failures << " encounter request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
