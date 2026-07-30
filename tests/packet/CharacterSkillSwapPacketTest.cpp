#include "packet/Send.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::CharacterSkillSwap;

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

int ExpectPacket(
    const cg::packet::Packet& packet,
    std::string_view from,
    std::string_view to,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode("Noeb"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() == 2U) {
        failures += ExpectField(packet, 0U, "from", from, message);
        failures += ExpectField(packet, 1U, "to", to, message);
    }
    return failures;
}

int TestSpecAndEncode()
{
    const CharacterSkillSwap request(0, -1);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(CharacterSkillSwap::Code == "Noeb", "Noeb code");
    failures += Expect(
        CharacterSkillSwap::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "Noeb direction");
    failures += Expect(
        CharacterSkillSwap::Fields.size() == 2U,
        "Noeb fields");
    failures += Expect(
        CharacterSkillSwap::Fields[0].name == "from",
        "Noeb from name");
    failures += Expect(
        CharacterSkillSwap::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "Noeb from type");
    failures += Expect(
        CharacterSkillSwap::Fields[1].name == "to",
        "Noeb to name");
    failures += Expect(
        CharacterSkillSwap::Fields[1].type ==
            cg::packet::PacketFieldType::Int62,
        "Noeb to type");
    failures += Expect(
        CharacterSkillSwap::Spec.code == CharacterSkillSwap::Code,
        "Noeb Spec code");
    failures += Expect(
        CharacterSkillSwap::Spec.direction ==
            CharacterSkillSwap::Direction,
        "Noeb Spec direction");
    failures += Expect(
        CharacterSkillSwap::Spec.fieldCount == 2U,
        "Noeb Spec field count");
    failures += Expect(request.from == 0, "Noeb from member");
    failures += Expect(request.to == -1, "Noeb to member");
    failures += ExpectPacket(packet, "0", "-1", "Noeb packet");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "Noeb Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "Noeb 0 -1 ",
            "Noeb canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(
            "Noeb 0 -1",
            CharacterSkillSwap::Spec);

    int failures = Expect(parsed.HasValue(), "Noeb generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(
        parsed.Value(),
        "0",
        "-1",
        "Noeb generic packet");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(
            parsed.Value(),
            CharacterSkillSwap::Spec);
    failures += Expect(rebuilt.HasValue(), "Noeb generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(
            rebuilt.Value() == "Noeb 0 -1 ",
            "Noeb generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int from;
        int to;
        std::string_view expected;
    };

    // to=-1 是 Server 的真实自动整理语义；其他负数与 int 边界仅证明 cg3a codec。
    constexpr Case Cases[] = {
        {0, -1, "Noeb 0 -1 "},
        {1, 2, "Noeb 1 2 "},
        {10, 61, "Noeb a Z "},
        {-1, 10, "Noeb -1 a "},
        {
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            "Noeb -2lkCB2 2lkCB1 ",
        },
        {
            std::numeric_limits<int>::max(),
            std::numeric_limits<int>::min(),
            "Noeb 2lkCB1 -2lkCB2 ",
        },
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const CharacterSkillSwap request(testCase.from, testCase.to);
        failures += ExpectPacket(
            request.ToPacket(),
            std::to_string(testCase.from),
            std::to_string(testCase.to),
            "Noeb signed packet");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "Noeb signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "Noeb signed canonical");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket(
            "bad 0 -1 ",
            CharacterSkillSwap::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "Noeb wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "Noeb 0",
            CharacterSkillSwap::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Noeb",
        true,
        1U,
        "Noeb missing to");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "Noeb 0 -1 1 ",
            CharacterSkillSwap::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Noeb",
        true,
        2U,
        "Noeb extra field");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "Noeb _ -1 ",
            CharacterSkillSwap::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Noeb",
        true,
        0U,
        "Noeb invalid from");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "Noeb 0 _ ",
            CharacterSkillSwap::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Noeb",
        true,
        1U,
        "Noeb invalid to");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "Noeb 2lkCB2 -1 ",
            CharacterSkillSwap::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Noeb",
        true,
        0U,
        "Noeb from positive overflow");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "Noeb -2lkCB3 -1 ",
            CharacterSkillSwap::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Noeb",
        true,
        0U,
        "Noeb from negative overflow");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "Noeb 0 2lkCB2 ",
            CharacterSkillSwap::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Noeb",
        true,
        1U,
        "Noeb to positive overflow");
    failures += ExpectError(
        cg::packet::ParsePacket(
            "Noeb 0 -2lkCB3 ",
            CharacterSkillSwap::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Noeb",
        true,
        1U,
        "Noeb to negative overflow");

    const std::string pollutedCode = "Noeb\x17" "x";
    const std::string pollutedWire = pollutedCode + " 0 -1";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedWire,
            CharacterSkillSwap::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "Noeb polluted code");

    const std::string pollutedField = "Noeb 0 -1\x17" "x";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedField,
            CharacterSkillSwap::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Noeb",
        true,
        2U,
        "Noeb polluted field");
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
                  << " character skill swap packet test(s) failed\n";
        return 1;
    }
    return 0;
}
