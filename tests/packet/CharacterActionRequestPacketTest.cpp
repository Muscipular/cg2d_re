#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::CharacterActionRequest;

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
    const CharacterActionRequest request(10, 11, 12);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(CharacterActionRequest::Code == "MIS", "MIS code");
    failures += Expect(
        CharacterActionRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "MIS direction");
    failures += Expect(
        CharacterActionRequest::Fields.size() == 3U,
        "MIS fields");
    constexpr std::array<std::string_view, 3U> names = {{
        "x",
        "y",
        "action",
    }};
    for (std::size_t index = 0U; index < names.size(); ++index) {
        failures += Expect(
            CharacterActionRequest::Fields[index].name == names[index],
            "MIS field name");
        failures += Expect(
            CharacterActionRequest::Fields[index].type ==
                cg::packet::PacketFieldType::Int62,
            "MIS field type");
    }
    failures += Expect(
        CharacterActionRequest::Spec.code == CharacterActionRequest::Code,
        "MIS Spec code");
    failures += Expect(
        CharacterActionRequest::Spec.direction ==
            CharacterActionRequest::Direction,
        "MIS Spec direction");
    failures += Expect(
        CharacterActionRequest::Spec.fieldCount == 3U,
        "MIS Spec field count");
    failures += Expect(request.x == 10, "MIS x member");
    failures += Expect(request.y == 11, "MIS y member");
    failures += Expect(request.action == 12, "MIS action member");
    failures += Expect(
        packet.code == cg::packet::PacketCode("MIS"),
        "MIS packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "MIS packet direction");
    failures += Expect(packet.fields.size() == 3U, "MIS packet fields");
    failures += ExpectField(packet, 0U, "x", "10", "MIS x field");
    failures += ExpectField(packet, 1U, "y", "11", "MIS y field");
    failures += ExpectField(packet, 2U, "action", "12", "MIS action field");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "MIS Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "MIS a b c ",
            "MIS Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(
            "MIS a b c",
            CharacterActionRequest::Spec);

    int failures = Expect(parsed.HasValue(), "MIS generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectField(parsed.Value(), 0U, "x", "10", "MIS parsed x");
    failures += ExpectField(parsed.Value(), 1U, "y", "11", "MIS parsed y");
    failures += ExpectField(
        parsed.Value(),
        2U,
        "action",
        "12",
        "MIS parsed action");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(
            parsed.Value(),
            CharacterActionRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "MIS generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(
            rebuilt.Value() == "MIS a b c ",
            "MIS generic canonical");
    }
    return failures;
}

int TestSignedAndBusinessOutsideValues()
{
    struct Case
    {
        int x;
        int y;
        int action;
        std::string_view expected;
    };

    // 业务外值证明 packet 层不下沉坐标、XYD 或动作范围；
    // INT_MIN/INT_MAX 仅证明 cg3a codec 保留完整 signed int。
    constexpr Case Cases[] = {
        {0, 1, 40, "MIS 0 1 E "},
        {40, 70, 100, "MIS E 18 1C "},
        {-1, -40, -70, "MIS -1 -E -18 "},
        {
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            100,
            "MIS -2lkCB2 2lkCB1 1C ",
        },
        {
            std::numeric_limits<int>::max(),
            std::numeric_limits<int>::min(),
            -100,
            "MIS 2lkCB1 -2lkCB2 -1C ",
        },
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const CharacterActionRequest request(
            testCase.x,
            testCase.y,
            testCase.action);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectField(
            packet,
            0U,
            "x",
            std::to_string(testCase.x),
            "MIS signed x");
        failures += ExpectField(
            packet,
            1U,
            "y",
            std::to_string(testCase.y),
            "MIS signed y");
        failures += ExpectField(
            packet,
            2U,
            "action",
            std::to_string(testCase.action),
            "MIS signed action");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "MIS signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "MIS signed canonical");
        }
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 3U>& tokens)
{
    return "MIS " + tokens[0] + ' ' + tokens[1] + ' ' + tokens[2] + ' ';
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket(
            "bad a b c ",
            CharacterActionRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "MIS wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("MIS", CharacterActionRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "MIS",
        true,
        0U,
        "MIS missing all fields");
    failures += ExpectError(
        cg::packet::ParsePacket("MIS a", CharacterActionRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "MIS",
        true,
        1U,
        "MIS missing y and action");
    failures += ExpectError(
        cg::packet::ParsePacket("MIS a b", CharacterActionRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "MIS",
        true,
        2U,
        "MIS missing action");
    failures += ExpectError(
        cg::packet::ParsePacket("MIS a b c 0 ", CharacterActionRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "MIS",
        true,
        3U,
        "MIS extra field");

    const std::array<std::string, 3U> validTokens = {{"a", "b", "c"}};
    for (std::size_t index = 0U; index < validTokens.size(); ++index) {
        std::array<std::string, 3U> invalidTokens = validTokens;
        invalidTokens[index] = "_";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(invalidTokens),
                CharacterActionRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "MIS",
            true,
            index,
            "MIS invalid i62");

        std::array<std::string, 3U> positiveOverflowTokens = validTokens;
        positiveOverflowTokens[index] = "2lkCB2";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(positiveOverflowTokens),
                CharacterActionRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "MIS",
            true,
            index,
            "MIS positive i62 overflow");

        std::array<std::string, 3U> negativeOverflowTokens = validTokens;
        negativeOverflowTokens[index] = "-2lkCB3";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(negativeOverflowTokens),
                CharacterActionRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "MIS",
            true,
            index,
            "MIS negative i62 overflow");
    }

    const std::string pollutedCode = "MIS\x17" "x";
    const std::string pollutedWire = pollutedCode + " a b c";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedWire,
            CharacterActionRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "MIS polluted code");

    const std::string pollutedField = "MIS a b\x17" "x c";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedField,
            CharacterActionRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "MIS",
        true,
        3U,
        "MIS polluted field");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndEncode()
        + TestGenericRoundtrip()
        + TestSignedAndBusinessOutsideValues()
        + TestErrors();
    if (failures != 0) {
        std::cerr
            << failures
            << " character action request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
