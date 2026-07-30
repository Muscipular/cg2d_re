#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::GuildMemberInviteRequest;

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
    const GuildMemberInviteRequest request(10, 20);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(GuildMemberInviteRequest::Code == "npzi", "npzi code");
    failures += Expect(
        GuildMemberInviteRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "npzi direction");
    failures += Expect(
        GuildMemberInviteRequest::Fields.size() == 2U,
        "npzi fields");
    failures += Expect(
        GuildMemberInviteRequest::Fields[0].name == "x",
        "npzi x name");
    failures += Expect(
        GuildMemberInviteRequest::Fields[1].name == "y",
        "npzi y name");
    for (const cg::packet::PacketFieldSpec& field :
         GuildMemberInviteRequest::Fields) {
        failures += Expect(
            field.type == cg::packet::PacketFieldType::Int62,
            "npzi field type");
    }
    failures += Expect(
        GuildMemberInviteRequest::Spec.code == GuildMemberInviteRequest::Code,
        "npzi Spec code");
    failures += Expect(
        GuildMemberInviteRequest::Spec.direction ==
            GuildMemberInviteRequest::Direction,
        "npzi Spec direction");
    failures += Expect(
        GuildMemberInviteRequest::Spec.fieldCount == 2U,
        "npzi Spec field count");
    failures += Expect(request.x == 10, "npzi x member");
    failures += Expect(request.y == 20, "npzi y member");
    failures += Expect(
        packet.code == cg::packet::PacketCode("npzi"),
        "npzi packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "npzi packet direction");
    failures += Expect(packet.fields.size() == 2U, "npzi packet fields");
    failures += ExpectField(packet, 0U, "x", "10", "npzi x field");
    failures += ExpectField(packet, 1U, "y", "20", "npzi y field");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "npzi Encode");
    if (encoded.HasValue()) {
        failures += Expect(
            encoded.Value() == "npzi a k ",
            "npzi Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("npzi a k", GuildMemberInviteRequest::Spec);

    int failures = Expect(parsed.HasValue(), "npzi generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectField(parsed.Value(), 0U, "x", "10", "npzi parsed x");
    failures += ExpectField(parsed.Value(), 1U, "y", "20", "npzi parsed y");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), GuildMemberInviteRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "npzi generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(
            rebuilt.Value() == "npzi a k ",
            "npzi generic canonical");
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
        {0, -1, "npzi 0 -1 "},
        {-1, 0, "npzi -1 0 "},
        {
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            "npzi -2lkCB2 2lkCB1 ",
        },
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const GuildMemberInviteRequest request(testCase.x, testCase.y);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectField(
            packet,
            0U,
            "x",
            std::to_string(testCase.x),
            "npzi signed x");
        failures += ExpectField(
            packet,
            1U,
            "y",
            std::to_string(testCase.y),
            "npzi signed y");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "npzi signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "npzi signed canonical");
        }
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 2U>& tokens)
{
    return "npzi " + tokens[0] + ' ' + tokens[1] + ' ';
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a k ", GuildMemberInviteRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "npzi wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("npzi", GuildMemberInviteRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "npzi",
        true,
        0U,
        "npzi missing both fields");
    failures += ExpectError(
        cg::packet::ParsePacket("npzi a", GuildMemberInviteRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "npzi",
        true,
        1U,
        "npzi missing y");
    failures += ExpectError(
        cg::packet::ParsePacket("npzi a k 3 ", GuildMemberInviteRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "npzi",
        true,
        2U,
        "npzi extra field");

    const std::array<std::string, 2U> validTokens = {{"a", "k"}};
    for (std::size_t index = 0U; index < validTokens.size(); ++index) {
        std::array<std::string, 2U> invalidTokens = validTokens;
        invalidTokens[index] = "_";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(invalidTokens),
                GuildMemberInviteRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "npzi",
            true,
            index,
            "npzi invalid i62");

        std::array<std::string, 2U> overflowTokens = validTokens;
        overflowTokens[index] = "2lkCB2";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(overflowTokens),
                GuildMemberInviteRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "npzi",
            true,
            index,
            "npzi i62 overflow");
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
        std::cerr << failures
                  << " guild member invite request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
