#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>

namespace {

using cg::packet::receive::GuildMemberRemovalNotification;

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
        packet.code == cg::packet::PacketCode("oRut"),
        message);
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ServerToClient,
        message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() == 1U) {
        failures += Expect(
            packet.fields[0].name == "registSeqNumber",
            message);
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::Int62,
            message);
        failures += Expect(packet.fields[0].value == value, message);
    }
    return failures;
}

int TestSpecAndSuccess()
{
    const GuildMemberRemovalNotification notification(10);
    const cg::packet::Packet packet = notification.ToPacket();

    int failures = 0;
    failures += Expect(
        GuildMemberRemovalNotification::Code == "oRut",
        "oRut code");
    failures += Expect(
        GuildMemberRemovalNotification::Direction ==
            cg::packet::PacketDirection::ServerToClient,
        "oRut direction");
    failures += Expect(
        GuildMemberRemovalNotification::Fields.size() == 1U,
        "oRut fields");
    failures += Expect(
        GuildMemberRemovalNotification::Fields[0].name ==
            "registSeqNumber",
        "oRut field name");
    failures += Expect(
        GuildMemberRemovalNotification::Fields[0].type ==
            cg::packet::PacketFieldType::Int62,
        "oRut field type");
    failures += Expect(
        GuildMemberRemovalNotification::Spec.code ==
            GuildMemberRemovalNotification::Code,
        "oRut Spec code");
    failures += Expect(
        GuildMemberRemovalNotification::Spec.direction ==
            GuildMemberRemovalNotification::Direction,
        "oRut Spec direction");
    failures += Expect(
        GuildMemberRemovalNotification::Spec.fieldCount == 1U,
        "oRut Spec field count");
    failures += Expect(
        notification.registSeqNumber == 10,
        "oRut member");
    failures += ExpectPacket(packet, "10", "oRut packet");

    const cg::packet::PacketResult<GuildMemberRemovalNotification> converted =
        GuildMemberRemovalNotification::FromPacket(packet);
    failures += Expect(converted.HasValue(), "oRut FromPacket valid");
    if (converted.HasValue()) {
        failures += Expect(
            converted.Value().registSeqNumber == 10,
            "oRut FromPacket member");
    }

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(
            packet,
            GuildMemberRemovalNotification::Spec);
    failures += Expect(built.HasValue(), "oRut build");
    if (built.HasValue()) {
        failures += Expect(
            built.Value() == "oRut a ",
            "oRut canonical");
    }
    return failures;
}

int TestTypedAndGenericRoundtrip()
{
    const cg::packet::PacketResult<GuildMemberRemovalNotification> typed =
        GuildMemberRemovalNotification::Parse("oRut a");

    int failures = Expect(typed.HasValue(), "oRut typed parse");
    if (!typed.HasValue()) {
        return failures;
    }
    failures += Expect(
        typed.Value().registSeqNumber == 10,
        "oRut typed value");

    const cg::packet::PacketResult<std::string> typedBuilt =
        cg::packet::BuildPacket(
            typed.Value().ToPacket(),
            GuildMemberRemovalNotification::Spec);
    failures += Expect(typedBuilt.HasValue(), "oRut typed build");
    if (typedBuilt.HasValue()) {
        failures += Expect(
            typedBuilt.Value() == "oRut a ",
            "oRut typed canonical");
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic =
        cg::packet::ParsePacket(
            "oRut a",
            GuildMemberRemovalNotification::Spec);
    failures += Expect(generic.HasValue(), "oRut generic parse");
    if (!generic.HasValue()) {
        return failures;
    }
    failures += ExpectPacket(
        generic.Value(),
        "10",
        "oRut generic packet");

    const cg::packet::PacketResult<std::string> genericBuilt =
        cg::packet::BuildPacket(
            generic.Value(),
            GuildMemberRemovalNotification::Spec);
    failures += Expect(genericBuilt.HasValue(), "oRut generic build");
    if (genericBuilt.HasValue()) {
        failures += Expect(
            genericBuilt.Value() == "oRut a ",
            "oRut generic canonical");
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
        {0, "oRut 0 "},
        {10, "oRut a "},
        {-1, "oRut -1 "},
        {std::numeric_limits<int>::min(), "oRut -2lkCB2 "},
        {std::numeric_limits<int>::max(), "oRut 2lkCB1 "},
    };

    int failures = 0;
    for (const Case& testCase : Cases) {
        const GuildMemberRemovalNotification notification(testCase.value);
        failures += ExpectPacket(
            notification.ToPacket(),
            std::to_string(testCase.value),
            "oRut signed packet");

        const cg::packet::PacketResult<std::string> built =
            cg::packet::BuildPacket(
                notification.ToPacket(),
                GuildMemberRemovalNotification::Spec);
        failures += Expect(built.HasValue(), "oRut signed build");
        if (built.HasValue()) {
            failures += Expect(
                built.Value() == testCase.canonical,
                "oRut signed canonical");
        }

        const cg::packet::PacketResult<GuildMemberRemovalNotification> parsed =
            GuildMemberRemovalNotification::Parse(testCase.canonical);
        failures += Expect(parsed.HasValue(), "oRut signed parse");
        if (parsed.HasValue()) {
            failures += Expect(
                parsed.Value().registSeqNumber == testCase.value,
                "oRut signed value");
        }
    }
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        GuildMemberRemovalNotification::Parse("bad a "),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "oRut wrong code");
    failures += ExpectError(
        GuildMemberRemovalNotification::Parse("oRut"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "oRut",
        true,
        0U,
        "oRut missing field");
    failures += ExpectError(
        GuildMemberRemovalNotification::Parse("oRut a 0 "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "oRut",
        true,
        1U,
        "oRut extra field");
    failures += ExpectError(
        GuildMemberRemovalNotification::Parse("oRut _ "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "oRut",
        true,
        0U,
        "oRut invalid i62");
    failures += ExpectError(
        GuildMemberRemovalNotification::Parse("oRut 2lkCB2 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "oRut",
        true,
        0U,
        "oRut positive overflow");
    failures += ExpectError(
        GuildMemberRemovalNotification::Parse("oRut -2lkCB3 "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "oRut",
        true,
        0U,
        "oRut negative overflow");

    const std::string pollutedCode = "oRut\x17" "x";
    failures += ExpectError(
        GuildMemberRemovalNotification::Parse(pollutedCode + " a"),
        cg::packet::PacketErrorCode::InvalidCode,
        pollutedCode,
        false,
        0U,
        "oRut polluted code");
    const std::string pollutedField = "oRut a\x17" "x";
    failures += ExpectError(
        GuildMemberRemovalNotification::Parse(pollutedField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "oRut",
        true,
        1U,
        "oRut polluted field");
    return failures;
}

int TestFromPacketErrors()
{
    int failures = 0;

    cg::packet::Packet wrongCode =
        GuildMemberRemovalNotification(10).ToPacket();
    wrongCode.code = cg::packet::PacketCode("bad");
    failures += ExpectError(
        GuildMemberRemovalNotification::FromPacket(wrongCode),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "oRut FromPacket wrong code");

    cg::packet::Packet wrongDirection =
        GuildMemberRemovalNotification(10).ToPacket();
    wrongDirection.direction =
        cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        GuildMemberRemovalNotification::FromPacket(wrongDirection),
        cg::packet::PacketErrorCode::DecodeFailed,
        "oRut",
        false,
        0U,
        "oRut FromPacket wrong direction");

    cg::packet::Packet missingField(
        cg::packet::PacketCode("oRut"),
        cg::packet::PacketDirection::ServerToClient);
    failures += ExpectError(
        GuildMemberRemovalNotification::FromPacket(missingField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "oRut",
        true,
        0U,
        "oRut FromPacket missing field");

    cg::packet::Packet extraField =
        GuildMemberRemovalNotification(10).ToPacket();
    extraField.fields.push_back(cg::packet::PacketField {
        "extra",
        cg::packet::PacketFieldType::Int62,
        "0",
    });
    failures += ExpectError(
        GuildMemberRemovalNotification::FromPacket(extraField),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "oRut",
        true,
        1U,
        "oRut FromPacket extra field");

    cg::packet::Packet wrongName =
        GuildMemberRemovalNotification(10).ToPacket();
    wrongName.fields[0].name = "wrong";
    failures += ExpectError(
        GuildMemberRemovalNotification::FromPacket(wrongName),
        cg::packet::PacketErrorCode::DecodeFailed,
        "oRut",
        true,
        0U,
        "oRut FromPacket wrong name");

    cg::packet::Packet wrongType =
        GuildMemberRemovalNotification(10).ToPacket();
    wrongType.fields[0].type =
        cg::packet::PacketFieldType::EscapedString;
    failures += ExpectError(
        GuildMemberRemovalNotification::FromPacket(wrongType),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "oRut",
        true,
        0U,
        "oRut FromPacket wrong type");

    cg::packet::Packet invalidValue =
        GuildMemberRemovalNotification(10).ToPacket();
    invalidValue.fields[0].value = "not-an-int";
    failures += ExpectError(
        GuildMemberRemovalNotification::FromPacket(invalidValue),
        cg::packet::PacketErrorCode::DecodeFailed,
        "oRut",
        true,
        0U,
        "oRut FromPacket invalid decimal");

    cg::packet::Packet overflowValue =
        GuildMemberRemovalNotification(10).ToPacket();
    overflowValue.fields[0].value = "2147483648";
    failures += ExpectError(
        GuildMemberRemovalNotification::FromPacket(overflowValue),
        cg::packet::PacketErrorCode::DecodeFailed,
        "oRut",
        true,
        0U,
        "oRut FromPacket decimal overflow");
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
                  << " guild member removal notification packet test(s) failed\n";
        return 1;
    }
    return 0;
}
