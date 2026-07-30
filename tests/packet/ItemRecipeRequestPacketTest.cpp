#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::ItemRecipeRequest;

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
    failures += Expect(packet.code == cg::packet::PacketCode("fS"), message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "haveskillindex", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == value, message);
    }
    return failures;
}

int TestSpecAndEncode()
{
    const ItemRecipeRequest request(10);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(ItemRecipeRequest::Code == "fS", "fS code");
    failures += Expect(
        ItemRecipeRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "fS direction");
    failures += Expect(ItemRecipeRequest::Fields.size() == 1U, "fS fields");
    failures += Expect(
        ItemRecipeRequest::Fields[0].name == "haveskillindex",
        "fS field name");
    failures += Expect(
        ItemRecipeRequest::Fields[0].type == cg::packet::PacketFieldType::Int62,
        "fS field type");
    failures += Expect(
        ItemRecipeRequest::Spec.code == ItemRecipeRequest::Code,
        "fS Spec code");
    failures += Expect(
        ItemRecipeRequest::Spec.direction == ItemRecipeRequest::Direction,
        "fS Spec direction");
    failures += Expect(ItemRecipeRequest::Spec.fieldCount == 1U, "fS Spec field count");
    failures += Expect(request.haveSkillIndex == 10, "fS member");
    failures += ExpectPacket(packet, "10", "fS packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "fS Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "fS a ", "fS Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("fS a", ItemRecipeRequest::Spec);

    int failures = Expect(parsed.HasValue(), "fS generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(parsed.Value(), "10", "fS generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), ItemRecipeRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "fS generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "fS a ", "fS generic canonical");
    }
    return failures;
}

int TestRawSkillIndex()
{
    const ItemRecipeRequest request(110);
    int failures = Expect(request.haveSkillIndex == 110, "fS raw 110 member");
    failures += ExpectPacket(request.ToPacket(), "110", "fS raw 110 packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "fS raw 110 Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "fS 1M ", "fS raw 110 canonical");
    }
    return failures;
}

int TestSignedAndExtremeValues()
{
    struct Case
    {
        int haveSkillIndex;
        std::string_view expected;
    };

    constexpr Case Cases[] = {
        {-1, "fS -1 "},
        {std::numeric_limits<int>::min(), "fS -2lkCB2 "},
        {std::numeric_limits<int>::max(), "fS 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const ItemRecipeRequest request(testCase.haveSkillIndex);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.haveSkillIndex),
            "fS signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "fS signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(encoded.Value() == testCase.expected, "fS signed canonical");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a ", ItemRecipeRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "fS wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("fS", ItemRecipeRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "fS",
        true,
        0U,
        "fS missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("fS a 1 ", ItemRecipeRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "fS",
        true,
        1U,
        "fS extra field");
    failures += ExpectError(
        cg::packet::ParsePacket("fS _ ", ItemRecipeRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "fS",
        true,
        0U,
        "fS invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket("fS 2lkCB2 ", ItemRecipeRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "fS",
        true,
        0U,
        "fS i62 overflow");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndEncode()
        + TestGenericRoundtrip()
        + TestRawSkillIndex()
        + TestSignedAndExtremeValues()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " item recipe request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
