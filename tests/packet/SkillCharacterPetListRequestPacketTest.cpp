#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::SkillCharacterPetListRequest;

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
        packet.code == cg::packet::PacketCode("McVa"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "charSlot", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == value, message);
    }
    return failures;
}

int TestSpecAndEncode()
{
    const SkillCharacterPetListRequest request(10);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(
        SkillCharacterPetListRequest::Code == "McVa",
        "McVa code");
    failures += Expect(
        SkillCharacterPetListRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "McVa direction");
    failures += Expect(
        SkillCharacterPetListRequest::Fields.size() == 1U,
        "McVa fields");
    failures += Expect(
        SkillCharacterPetListRequest::Fields[0].name == "charSlot",
        "McVa field name");
    failures += Expect(
        SkillCharacterPetListRequest::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "McVa field type");
    failures += Expect(
        SkillCharacterPetListRequest::Spec.code ==
            SkillCharacterPetListRequest::Code,
        "McVa Spec code");
    failures += Expect(
        SkillCharacterPetListRequest::Spec.direction ==
            SkillCharacterPetListRequest::Direction,
        "McVa Spec direction");
    failures += Expect(
        SkillCharacterPetListRequest::Spec.fieldCount == 1U,
        "McVa Spec field count");
    failures += Expect(request.charSlot == 10, "McVa member");
    failures += ExpectPacket(packet, "10", "McVa packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "McVa Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "McVa a ",
            "McVa Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(
            "McVa a",
            SkillCharacterPetListRequest::Spec);

    int failures = Expect(parsed.HasValue(), "McVa generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(parsed.Value(), "10", "McVa generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(
            parsed.Value(),
            SkillCharacterPetListRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "McVa generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(
            rebuilt.Value() == "McVa a ",
            "McVa generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int charSlot;
        std::string_view expected;
    };

    // INT_MIN/INT_MAX 仅证明 cg3a codec 保留完整 signed int。
    constexpr Case Cases[] = {
        {0, "McVa 0 "},
        {-1, "McVa -1 "},
        {9, "McVa 9 "},
        {10, "McVa a "},
        {100, "McVa 1C "},
        {std::numeric_limits<int>::min(), "McVa -2lkCB2 "},
        {std::numeric_limits<int>::max(), "McVa 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const SkillCharacterPetListRequest request(testCase.charSlot);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.charSlot),
            "McVa signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "McVa signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "McVa signed canonical");
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
            SkillCharacterPetListRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "McVa wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "McVa",
            SkillCharacterPetListRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "McVa",
        true,
        0U,
        "McVa missing field");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "McVa a 0 ",
            SkillCharacterPetListRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "McVa",
        true,
        1U,
        "McVa extra field");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "McVa _ ",
            SkillCharacterPetListRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "McVa",
        true,
        0U,
        "McVa invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "McVa 2lkCB2 ",
            SkillCharacterPetListRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "McVa",
        true,
        0U,
        "McVa positive i62 overflow");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "McVa -2lkCB3 ",
            SkillCharacterPetListRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "McVa",
        true,
        0U,
        "McVa negative i62 overflow");

    const std::string pollutedCode = "McVa\x17" "a";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedCode,
            SkillCharacterPetListRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "McVa polluted code");

    const std::string pollutedField = "McVa a\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedField,
            SkillCharacterPetListRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "McVa",
        true,
        1U,
        "McVa polluted field");
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
                  << " skill character pet list request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
