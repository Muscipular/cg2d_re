#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::GuildMemberTitleChangeRequest;

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
    const GuildMemberTitleChangeRequest request(10, 20);
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(GuildMemberTitleChangeRequest::Code == "uHb", "uHb code");
    failures += Expect(
        GuildMemberTitleChangeRequest::Direction ==
            cg::packet::PacketDirection::ClientToServer,
        "uHb direction");
    failures += Expect(
        GuildMemberTitleChangeRequest::Fields.size() == 2U,
        "uHb fields");
    failures += Expect(
        GuildMemberTitleChangeRequest::Fields[0].name == "index",
        "uHb index name");
    failures += Expect(
        GuildMemberTitleChangeRequest::Fields[1].name == "title",
        "uHb title name");
    for (const cg::packet::PacketFieldSpec& field :
         GuildMemberTitleChangeRequest::Fields) {
        failures += Expect(
            field.type == cg::packet::PacketFieldType::Int62,
            "uHb field type");
    }
    failures += Expect(
        GuildMemberTitleChangeRequest::Spec.code ==
            GuildMemberTitleChangeRequest::Code,
        "uHb Spec code");
    failures += Expect(
        GuildMemberTitleChangeRequest::Spec.direction ==
            GuildMemberTitleChangeRequest::Direction,
        "uHb Spec direction");
    failures += Expect(
        GuildMemberTitleChangeRequest::Spec.fieldCount == 2U,
        "uHb Spec field count");
    failures += Expect(request.index == 10, "uHb index member");
    failures += Expect(request.titleId == 20, "uHb titleId member");
    failures += Expect(packet.code == cg::packet::PacketCode("uHb"), "uHb packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "uHb packet direction");
    failures += Expect(packet.fields.size() == 2U, "uHb packet fields");
    failures += ExpectField(packet, 0U, "index", "10", "uHb index field");
    failures += ExpectField(packet, 1U, "title", "20", "uHb title field");

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "uHb Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "uHb a k ", "uHb Encode canonical");
    }
    return failures;
}

int TestGenericRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("uHb a k", GuildMemberTitleChangeRequest::Spec);

    int failures = Expect(parsed.HasValue(), "uHb generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectField(parsed.Value(), 0U, "index", "10", "uHb parsed index");
    failures += ExpectField(parsed.Value(), 1U, "title", "20", "uHb parsed title");

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), GuildMemberTitleChangeRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "uHb generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "uHb a k ", "uHb generic canonical");
    }
    return failures;
}

int TestSignedAndBoundaryValues()
{
    struct Case
    {
        int index;
        int titleId;
        std::string_view expected;
    };

    constexpr Case Cases[] = {
        {0, -1, "uHb 0 -1 "},
        {10, 31, "uHb a v "},
        {10, 32, "uHb a w "},
        {
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            "uHb -2lkCB2 2lkCB1 ",
        },
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const GuildMemberTitleChangeRequest request(testCase.index, testCase.titleId);
        const cg::packet::Packet packet = request.ToPacket();
        failures += ExpectField(
            packet,
            0U,
            "index",
            std::to_string(testCase.index),
            "uHb signed index");
        failures += ExpectField(
            packet,
            1U,
            "title",
            std::to_string(testCase.titleId),
            "uHb signed title");

        const cg::packet::PacketResult<std::string> encoded = request.Encode();
        failures += Expect(encoded.HasValue(), "uHb signed Encode");
        if (encoded.HasValue()) {
            failures += Expect(
                encoded.Value() == testCase.expected,
                "uHb signed canonical");
        }
    }
    return failures;
}

std::string BuildPlain(const std::array<std::string, 2U>& tokens)
{
    return "uHb " + tokens[0] + ' ' + tokens[1] + ' ';
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad a k ", GuildMemberTitleChangeRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "uHb wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("uHb", GuildMemberTitleChangeRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "uHb",
        true,
        0U,
        "uHb missing both fields");
    failures += ExpectError(
        cg::packet::ParsePacket("uHb a", GuildMemberTitleChangeRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "uHb",
        true,
        1U,
        "uHb missing title");
    failures += ExpectError(
        cg::packet::ParsePacket("uHb a k 3 ", GuildMemberTitleChangeRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "uHb",
        true,
        2U,
        "uHb extra field");

    const std::array<std::string, 2U> validTokens = {{"a", "k"}};
    for (std::size_t index = 0U; index < validTokens.size(); ++index) {
        std::array<std::string, 2U> invalidTokens = validTokens;
        invalidTokens[index] = "_";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(invalidTokens),
                GuildMemberTitleChangeRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "uHb",
            true,
            index,
            "uHb invalid i62");

        std::array<std::string, 2U> overflowTokens = validTokens;
        overflowTokens[index] = "2lkCB2";
        failures += ExpectError(
            cg::packet::ParsePacket(
                BuildPlain(overflowTokens),
                GuildMemberTitleChangeRequest::Spec),
            cg::packet::PacketErrorCode::DecodeFailed,
            "uHb",
            true,
            index,
            "uHb i62 overflow");
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
                  << " guild member title change request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
