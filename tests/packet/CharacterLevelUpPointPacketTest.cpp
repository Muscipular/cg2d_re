#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::CharacterLevelUpPoint;

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
        packet.code == cg::packet::PacketCode("IHw"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "sqeno", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == value, message);
    }
    return failures;
}

int TestSpecAndEncode()
{
    const CharacterLevelUpPoint request(4);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(CharacterLevelUpPoint::Code == "IHw", "IHw code");
    failures += Expect(
        CharacterLevelUpPoint::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "IHw direction");
    failures += Expect(
        CharacterLevelUpPoint::Fields.size() == 1U,
        "IHw fields");
    failures += Expect(
        CharacterLevelUpPoint::Fields[0].name == "sqeno",
        "IHw field name");
    failures += Expect(
        CharacterLevelUpPoint::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "IHw field type");
    failures += Expect(
        CharacterLevelUpPoint::Spec.code == CharacterLevelUpPoint::Code,
        "IHw Spec code");
    failures += Expect(
        CharacterLevelUpPoint::Spec.direction ==
            CharacterLevelUpPoint::Direction,
        "IHw Spec direction");
    failures += Expect(
        CharacterLevelUpPoint::Spec.fieldCount == 1U,
        "IHw Spec field count");
    failures += Expect(request.sqeno == 4, "IHw member");
    failures += ExpectPacket(packet, "4", "IHw packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "IHw Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "IHw 4 ", "IHw canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("IHw 4", CharacterLevelUpPoint::Spec);

    int failures = Expect(parsed.HasValue(), "IHw generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(parsed.Value(), "4", "IHw generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(
            parsed.Value(),
            CharacterLevelUpPoint::Spec);
    failures += Expect(rebuilt.HasValue(), "IHw generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(
            rebuilt.Value() == "IHw 4 ",
            "IHw generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int sqeno;
        std::string_view expected;
    };

    // Server 业务层仅接受 0..4；5、负数与 int 边界仅证明 cg3a codec。
    constexpr Case Cases[] = {
        {0, "IHw 0 "},
        {4, "IHw 4 "},
        {5, "IHw 5 "},
        {-1, "IHw -1 "},
        {10, "IHw a "},
        {std::numeric_limits<int>::min(), "IHw -2lkCB2 "},
        {std::numeric_limits<int>::max(), "IHw 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const CharacterLevelUpPoint request(testCase.sqeno);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.sqeno),
            "IHw signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "IHw signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "IHw signed canonical");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad 4 ", CharacterLevelUpPoint::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "IHw wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("IHw", CharacterLevelUpPoint::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IHw",
        true,
        0U,
        "IHw missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("IHw 4 0 ", CharacterLevelUpPoint::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IHw",
        true,
        1U,
        "IHw extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("IHw _ ", CharacterLevelUpPoint::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IHw",
        true,
        0U,
        "IHw invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("IHw 2lkCB2 ", CharacterLevelUpPoint::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IHw",
        true,
        0U,
        "IHw positive overflow");
    failures += ExpectError(
        cg::packet::ParsePacket("IHw -2lkCB3 ", CharacterLevelUpPoint::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "IHw",
        true,
        0U,
        "IHw negative overflow");

    const std::string pollutedCode = "IHw\x17" "4";
    failures += ExpectError(
        cg::packet::ParsePacket(pollutedCode, CharacterLevelUpPoint::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "IHw polluted code");

    const std::string pollutedField = "IHw 4\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(pollutedField, CharacterLevelUpPoint::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "IHw",
        true,
        1U,
        "IHw polluted field");
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
                  << " character level up point packet test(s) failed\n";
        return 1;
    }
    return 0;
}
