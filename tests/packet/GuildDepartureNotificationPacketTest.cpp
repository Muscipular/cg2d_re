#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::GuildDepartureNotification;

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
        packet.code == cg::packet::PacketCode("frQp"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ServerToClient,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "guildId", message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == value, message);
    }
    return failures;
}

int TestSpecAndSuccess()
{
    const GuildDepartureNotification notification(10);
    const cg::packet::Packet packet = notification.ToPacket();

    int failures = 0;
    failures += Expect(
        GuildDepartureNotification::Code == "frQp",
        "frQp code");
    failures += Expect(
        GuildDepartureNotification::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "frQp direction");
    failures += Expect(
        GuildDepartureNotification::Fields.size() == 1U,
        "frQp fields");
    failures += Expect(
        GuildDepartureNotification::Fields[0].name == "guildId",
        "frQp field name");
    failures += Expect(
        GuildDepartureNotification::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "frQp field type");
    failures += Expect(
        GuildDepartureNotification::Spec.code ==
            GuildDepartureNotification::Code,
        "frQp Spec code");
    failures += Expect(
        GuildDepartureNotification::Spec.direction ==
            GuildDepartureNotification::Direction,
        "frQp Spec direction");
    failures += Expect(
        GuildDepartureNotification::Spec.fieldCount == 1U,
        "frQp Spec field count");
    failures += Expect(notification.guildId == 10, "frQp member");
    failures += ExpectPacket(packet, "10", "frQp packet");

    const cg::packet::PacketResult<GuildDepartureNotification> converted =
        GuildDepartureNotification::FromPacket(packet);
    failures += Expect(converted.HasValue(), "frQp FromPacket valid");
    if (converted.HasValue()) {
        failures += Expect(
            converted.Value().guildId == 10,
            "frQp FromPacket member");
    }

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(packet, GuildDepartureNotification::Spec);
    failures += Expect(built.HasValue(), "frQp build");
    if (built.HasValue()) {
        failures += Expect(
            built.Value() == "frQp a ",
            "frQp canonical");
    }
    return failures;
}

int TestTypedAndGenericRoundtrip()
{
    const cg::packet::PacketResult<GuildDepartureNotification> typed =
        GuildDepartureNotification::Parse("frQp a");

    int failures = Expect(typed.HasValue(), "frQp typed parse");
    if (!typed.HasValue()) {
        return failures;
    }
    failures += Expect(typed.Value().guildId == 10, "frQp typed value");

    const cg::packet::PacketResult<std::string> typedBuilt =
        cg::packet::BuildPacket(
            typed.Value().ToPacket(),
            GuildDepartureNotification::Spec);
    failures += Expect(typedBuilt.HasValue(), "frQp typed build");
    if (typedBuilt.HasValue()) {
        failures += Expect(
            typedBuilt.Value() == "frQp a ",
            "frQp typed canonical");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket(
            "frQp a",
            GuildDepartureNotification::Spec);
    failures += Expect(generic.HasValue(), "frQp generic parse");
    if (!generic.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(
        generic.Value(),
        "10",
        "frQp generic packet");

    const cg::packet::PacketResult<std::string> genericBuilt =
        cg::packet::BuildPacket(
            generic.Value(),
            GuildDepartureNotification::Spec);
    failures += Expect(genericBuilt.HasValue(), "frQp generic build");
    if (genericBuilt.HasValue()) {
        failures += Expect(
            genericBuilt.Value() == "frQp a ",
            "frQp generic canonical");
    }
    return failures;
}

int TestSignedValues()
{
    struct Case
    {
        int value;
        std::string_view canonical;
    };

    constexpr Case Cases[] = {
        {0, "frQp 0 "},
        {10, "frQp a "},
        {-1, "frQp -1 "},
        {std::numeric_limits<int>::min(), "frQp -2lkCB2 "},
        {std::numeric_limits<int>::max(), "frQp 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const GuildDepartureNotification notification(testCase.value);
        failures += ExpectPacket(
            notification.ToPacket(),
            std::to_string(testCase.value),
            "frQp signed packet");

        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(
                notification.ToPacket(),
                GuildDepartureNotification::Spec);
        failures += Expect(built.HasValue(), "frQp signed build");
        if (built.HasValue()) {
            failures += Expect(
                built.Value() == testCase.canonical,
                "frQp signed canonical");
        }

        const cg::packet::PacketResult<GuildDepartureNotification> parsed =
            GuildDepartureNotification::Parse(testCase.canonical);
        failures += Expect(parsed.HasValue(), "frQp signed parse");
        if (parsed.HasValue()) {
            failures += Expect(
                parsed.Value().guildId == testCase.value,
                "frQp signed value");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        GuildDepartureNotification::Parse("bad a "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "frQp wrong code");
    failures += ExpectError(
        GuildDepartureNotification::Parse("frQp"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "frQp",
        true,
        0U,
        "frQp missing field");
    failures += ExpectError(
        GuildDepartureNotification::Parse("frQp a 0 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "frQp",
        true,
        1U,
        "frQp extra field");
    failures += ExpectError(
        GuildDepartureNotification::Parse("frQp _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "frQp",
        true,
        0U,
        "frQp invalid i62");
    failures += ExpectError(
        GuildDepartureNotification::Parse("frQp 2lkCB2 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "frQp",
        true,
        0U,
        "frQp positive overflow");
    failures += ExpectError(
        GuildDepartureNotification::Parse("frQp -2lkCB3 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "frQp",
        true,
        0U,
        "frQp negative overflow");

    const std::string pollutedCode = "frQp\x17" "x";
    failures += ExpectError(
        GuildDepartureNotification::Parse(pollutedCode + " a"),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "frQp polluted code");
    const std::string pollutedField = "frQp a\x17" "x";
    failures += ExpectError(
        GuildDepartureNotification::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "frQp",
        true,
        1U,
        "frQp polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode =
        GuildDepartureNotification(10).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        GuildDepartureNotification::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "frQp FromPacket wrong code");

    cg::packet::Packet wrongDirection =
        GuildDepartureNotification(10).ToPacket();
    wrongDirection.direction =
        cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        GuildDepartureNotification::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "frQp",
        false,
        0U,
        "frQp FromPacket wrong direction");

    cg::packet::Packet missingField(
        cg::packet::PacketCode("frQp"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        GuildDepartureNotification::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "frQp",
        true,
        0U,
        "frQp FromPacket missing field");

    cg::packet::Packet extraField =
        GuildDepartureNotification(10).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "0",
    });
    failures += ExpectError(
        GuildDepartureNotification::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "frQp",
        true,
        1U,
        "frQp FromPacket extra field");

    cg::packet::Packet wrongName =
        GuildDepartureNotification(10).ToPacket();
    wrongName.fields[0].name = "wrong";
    failures += ExpectError(
        GuildDepartureNotification::FromPacket(wrongName),
        cg::packet::PacketErrorCode::DecodeFailed,
        "frQp",
        true,
        0U,
        "frQp FromPacket wrong name");

    cg::packet::Packet wrongType =
        GuildDepartureNotification(10).ToPacket();
    wrongType.fields[0].type =
        cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        GuildDepartureNotification::FromPacket(wrongType),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "frQp",
        true,
        0U,
        "frQp FromPacket wrong type");

    cg::packet::Packet invalidValue =
        GuildDepartureNotification(10).ToPacket();
    invalidValue.fields[0].value = "not-an-int";
    failures += ExpectError(
        GuildDepartureNotification::FromPacket(invalidValue),
        cg::packet::PacketErrorCode::DecodeFailed,
        "frQp",
        true,
        0U,
        "frQp FromPacket invalid decimal");

    cg::packet::Packet overflowValue =
        GuildDepartureNotification(10).ToPacket();
    overflowValue.fields[0].value = "2147483648";
    failures += ExpectError(
        GuildDepartureNotification::FromPacket(overflowValue),
        cg::packet::PacketErrorCode::DecodeFailed,
        "frQp",
        true,
        0U,
        "frQp FromPacket decimal overflow");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndSuccess()
        + TestTypedAndGenericRoundtrip()
        + TestSignedValues()
        + TestParseErrors()
        + TestFromPacketErrors();
    if (failures != 0) {
        std::cerr << failures
                  << " guild departure notification packet test(s) failed\n";
        return 1;
    }
    return 0;
}
