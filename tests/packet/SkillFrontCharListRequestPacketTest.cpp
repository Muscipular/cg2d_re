#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::SkillFrontCharListRequest;

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
        packet.code == cg::packet::PacketCode("iET"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(
            packet.fields[0].name == "haveskillindex",
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
    const SkillFrontCharListRequest request(10);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(
        SkillFrontCharListRequest::Code == "iET",
        "iET code");
    failures += Expect(
        SkillFrontCharListRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "iET direction");
    failures += Expect(
        SkillFrontCharListRequest::Fields.size() == 1U,
        "iET fields");
    failures += Expect(
        SkillFrontCharListRequest::Fields[0].name == "haveskillindex",
        "iET field name");
    failures += Expect(
        SkillFrontCharListRequest::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "iET field type");
    failures += Expect(
        SkillFrontCharListRequest::Spec.code ==
            SkillFrontCharListRequest::Code,
        "iET Spec code");
    failures += Expect(
        SkillFrontCharListRequest::Spec.direction ==
            SkillFrontCharListRequest::Direction,
        "iET Spec direction");
    failures += Expect(
        SkillFrontCharListRequest::Spec.fieldCount == 1U,
        "iET Spec field count");
    failures += Expect(request.haveSkillIndex == 10, "iET member");
    failures += ExpectPacket(packet, "10", "iET packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "iET Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "iET a ",
            "iET Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(
            "iET a",
            SkillFrontCharListRequest::Spec);

    int failures = Expect(parsed.HasValue(), "iET generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(parsed.Value(), "10", "iET generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(
            parsed.Value(),
            SkillFrontCharListRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "iET generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(
            rebuilt.Value() == "iET a ",
            "iET generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int haveSkillIndex;
        std::string_view expected;
    };

    constexpr Case Cases[] = {
        {0, "iET 0 "},
        {-1, "iET -1 "},
        {100, "iET 1C "},
        {std::numeric_limits<int>::min(), "iET -2lkCB2 "},
        {std::numeric_limits<int>::max(), "iET 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const SkillFrontCharListRequest request(testCase.haveSkillIndex);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.haveSkillIndex),
            "iET signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "iET signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "iET signed canonical");
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
            SkillFrontCharListRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "iET wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "iET",
            SkillFrontCharListRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "iET",
        true,
        0U,
        "iET missing field");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "iET a 0 ",
            SkillFrontCharListRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "iET",
        true,
        1U,
        "iET extra field");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "iET _ ",
            SkillFrontCharListRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "iET",
        true,
        0U,
        "iET invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "iET 2lkCB2 ",
            SkillFrontCharListRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "iET",
        true,
        0U,
        "iET positive i62 overflow");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "iET -2lkCB3 ",
            SkillFrontCharListRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "iET",
        true,
        0U,
        "iET negative i62 overflow");

    const std::string pollutedCode = "iET\x17" "a";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedCode,
            SkillFrontCharListRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "iET polluted code");

    const std::string pollutedField = "iET a\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedField,
            SkillFrontCharListRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "iET",
        true,
        1U,
        "iET polluted field");
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
                  << " skill front char list request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
