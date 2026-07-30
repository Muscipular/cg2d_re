#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::ItemCharacterPetListRequest;

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
        packet.code == cg::packet::PacketCode("mjCv"),
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
    const ItemCharacterPetListRequest request(10);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(
        ItemCharacterPetListRequest::Code == "mjCv",
        "mjCv code");
    failures += Expect(
        ItemCharacterPetListRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "mjCv direction");
    failures += Expect(
        ItemCharacterPetListRequest::Fields.size() == 1U,
        "mjCv fields");
    failures += Expect(
        ItemCharacterPetListRequest::Fields[0].name == "charSlot",
        "mjCv field name");
    failures += Expect(
        ItemCharacterPetListRequest::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "mjCv field type");
    failures += Expect(
        ItemCharacterPetListRequest::Spec.code ==
            ItemCharacterPetListRequest::Code,
        "mjCv Spec code");
    failures += Expect(
        ItemCharacterPetListRequest::Spec.direction ==
            ItemCharacterPetListRequest::Direction,
        "mjCv Spec direction");
    failures += Expect(
        ItemCharacterPetListRequest::Spec.fieldCount == 1U,
        "mjCv Spec field count");
    failures += Expect(request.charSlot == 10, "mjCv member");
    failures += ExpectPacket(packet, "10", "mjCv packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "mjCv Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "mjCv a ",
            "mjCv Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(
            "mjCv a",
            ItemCharacterPetListRequest::Spec);

    int failures = Expect(parsed.HasValue(), "mjCv generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(parsed.Value(), "10", "mjCv generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(
            parsed.Value(),
            ItemCharacterPetListRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "mjCv generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(
            rebuilt.Value() == "mjCv a ",
            "mjCv generic canonical");
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
        {0, "mjCv 0 "},
        {-1, "mjCv -1 "},
        {9, "mjCv 9 "},
        {10, "mjCv a "},
        {100, "mjCv 1C "},
        {std::numeric_limits<int>::min(), "mjCv -2lkCB2 "},
        {std::numeric_limits<int>::max(), "mjCv 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const ItemCharacterPetListRequest request(testCase.charSlot);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.charSlot),
            "mjCv signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "mjCv signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "mjCv signed canonical");
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
            ItemCharacterPetListRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "mjCv wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "mjCv",
            ItemCharacterPetListRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "mjCv",
        true,
        0U,
        "mjCv missing field");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "mjCv a 0 ",
            ItemCharacterPetListRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "mjCv",
        true,
        1U,
        "mjCv extra field");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "mjCv _ ",
            ItemCharacterPetListRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "mjCv",
        true,
        0U,
        "mjCv invalid i62");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "mjCv 2lkCB2 ",
            ItemCharacterPetListRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "mjCv",
        true,
        0U,
        "mjCv positive i62 overflow");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "mjCv -2lkCB3 ",
            ItemCharacterPetListRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "mjCv",
        true,
        0U,
        "mjCv negative i62 overflow");

    const std::string pollutedCode = "mjCv\x17" "a";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedCode,
            ItemCharacterPetListRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "mjCv polluted code");

    const std::string pollutedField = "mjCv a\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedField,
            ItemCharacterPetListRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "mjCv",
        true,
        1U,
        "mjCv polluted field");
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
                  << " item character pet list request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
