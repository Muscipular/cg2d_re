#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::ActionSkillIconRequest;

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
    const ActionSkillIconRequest request(10, 11, 12);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(ActionSkillIconRequest::Code == "klF", "klF code");
    failures += Expect(
        ActionSkillIconRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "klF direction");
    failures += Expect(
        ActionSkillIconRequest::Fields.size() == 3U,
        "klF fields");
    constexpr std::array<std::string_view, 3U> names = {{
        "x",
        "y",
        "icon",
    }};
    for (std::size_t index = 0U; index < names.size(); ++index) {
        failures += Expect(
            ActionSkillIconRequest::Fields[index].name == names[index],
            "klF field name");
        failures += Expect(
            ActionSkillIconRequest::Fields[index].type ==
                cg::packet::PacketFieldType::Int62,
            "klF field type");
    }
    failures += Expect(
        ActionSkillIconRequest::Spec.code == ActionSkillIconRequest::Code,
        "klF Spec code");
    failures += Expect(
        ActionSkillIconRequest::Spec.direction ==
            ActionSkillIconRequest::Direction,
        "klF Spec direction");
    failures += Expect(
        ActionSkillIconRequest::Spec.fieldCount == 3U,
        "klF Spec field count");
    failures += Expect(request.x == 10, "klF x member");
    failures += Expect(request.y == 11, "klF y member");
    failures += Expect(request.icon == 12, "klF icon member");
    failures += Expect(
        packet.code == cg::packet::PacketCode("klF"),
        "klF packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "klF packet direction");
    failures += Expect(packet.fields.size() == 3U, "klF packet fields");
    failures += ExpectField(packet, 0U, "x", "10", "klF x field");
    failures += ExpectField(packet, 1U, "y", "11", "klF y field");
    failures += ExpectField(packet, 2U, "icon", "12", "klF icon field");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "klF Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "klF a b c ",
            "klF Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(
            "klF a b c",
            ActionSkillIconRequest::Spec);

    int failures = Expect(parsed.HasValue(), "klF generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectField(parsed.Value(), 0U, "x", "10", "klF parsed x");
    failures += ExpectField(parsed.Value(), 1U, "y", "11", "klF parsed y");
    failures += ExpectField(
        parsed.Value(),
        2U,
        "icon",
        "12",
        "klF parsed icon");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(
            parsed.Value(),
            ActionSkillIconRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "klF generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(
            rebuilt.Value() == "klF a b c ",
            "klF generic canonical");
    }
    return failures;
}

int TestSignedAndBusinessOutsideValues()
{
    struct Case
    {
        int x;
        int y;
        int icon;
        std::string_view expected;
    };

    // 0..16 与 0 清除均为 producer/业务语义；17、负值与任意坐标证明 packet 层不收窄。
    // INT_MIN/INT_MAX 仅证明 cg3a codec 保留完整 signed int。
    constexpr Case Cases[] = {
        {0, 0, 0, "klF 0 0 0 "},
        {10, 11, 16, "klF a b g "},
        {10, 11, 17, "klF a b h "},
        {-1, -1, -1, "klF -1 -1 -1 "},
        {100, 100, 100, "klF 1C 1C 1C "},
        {
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            17,
            "klF -2lkCB2 2lkCB1 h ",
        },
        {
            std::numeric_limits<int>::max(),
            std::numeric_limits<int>::min(),
            -100,
            "klF 2lkCB1 -2lkCB2 -1C ",
        },
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const ActionSkillIconRequest request(
            testCase.x,
            testCase.y,
            testCase.icon);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectField(
            packet,
            0U,
            "x",
            std::to_string(testCase.x),
            "klF signed x");
        failures += ExpectField(
            packet,
            1U,
            "y",
            std::to_string(testCase.y),
            "klF signed y");
        failures += ExpectField(
            packet,
            2U,
            "icon",
            std::to_string(testCase.icon),
            "klF signed icon");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "klF signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "klF signed canonical");
        }
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 3U>& tokens)
{
    return "klF " + tokens[0] + ' ' + tokens[1] + ' ' + tokens[2] + ' ';
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket(
            "bad a b c ",
            ActionSkillIconRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "klF wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("klF", ActionSkillIconRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "klF",
        true,
        0U,
        "klF missing all fields");
    failures += ExpectError(
        cg::packet::ParsePacket("klF a", ActionSkillIconRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "klF",
        true,
        1U,
        "klF missing y and icon");
    failures += ExpectError(
        cg::packet::ParsePacket("klF a b", ActionSkillIconRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "klF",
        true,
        2U,
        "klF missing icon");
    failures += ExpectError(
        cg::packet::ParsePacket("klF a b c 0 ", ActionSkillIconRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "klF",
        true,
        3U,
        "klF extra field");

    const std::array<std::string, 3U> validTokens = {{"a", "b", "c"}};
    for (std::size_t index = 0U; index < validTokens.size(); ++index) {
        std::array<std::string, 3U> invalidTokens = validTokens;
        invalidTokens[index] = "_";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(invalidTokens),
                ActionSkillIconRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "klF",
            true,
            index,
            "klF invalid i62");

        std::array<std::string, 3U> positiveOverflowTokens = validTokens;
        positiveOverflowTokens[index] = "2lkCB2";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(positiveOverflowTokens),
                ActionSkillIconRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "klF",
            true,
            index,
            "klF positive i62 overflow");

        std::array<std::string, 3U> negativeOverflowTokens = validTokens;
        negativeOverflowTokens[index] = "-2lkCB3";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(negativeOverflowTokens),
                ActionSkillIconRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "klF",
            true,
            index,
            "klF negative i62 overflow");
    }

    const std::string pollutedCode = "klF\x17" "x";
    const std::string pollutedWire = pollutedCode + " a b c";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedWire,
            ActionSkillIconRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "klF polluted code");

    const std::string pollutedField = "klF a b\x17" "x c";
    failures += ExpectError(
        cg::packet::ParsePacket(
            pollutedField,
            ActionSkillIconRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "klF",
        true,
        3U,
        "klF polluted field");
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
            << " action skill icon request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
