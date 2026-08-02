#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <limits>
#include <string>
#include <string_view>
#include <utility>

namespace {

using PartyMemberStatus = cg::packet::receive::PartyMemberStatus;
using PartyMemberStatusEntry = cg::packet::receive::PartyMemberStatusEntry;
using PartyMemberStatusKind = cg::packet::receive::PartyMemberStatusKind;

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, PartyMemberStatus::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, PartyMemberStatus::Spec);
}

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
    int failures = Expect(!static_cast<bool>(result), message);
    failures += Expect(result.HasError(), message);
    if (!result.HasError()) {
        return failures;
    }

    const cg::packet::PacketError& error = result.Error();
    failures += Expect(error.code == code, message);
    failures += Expect(error.packetCode.has_value(), message);
    if (error.packetCode.has_value()) {
        failures += Expect(error.packetCode->View() == packetCode, message);
    }

    if (hasFieldIndex) {
        failures += Expect(error.fieldIndex.has_value(), message);
        if (error.fieldIndex.has_value()) {
            failures += Expect(*error.fieldIndex == fieldIndex, message);
        }
    } else {
        failures += Expect(!error.fieldIndex.has_value(), message);
    }
    return failures;
}

std::string Bytes(std::initializer_list<unsigned int> values)
{
    std::string output;
    output.reserve(values.size());
    for (const unsigned int value : values) {
        output.push_back(static_cast<char>(value));
    }
    return output;
}

std::string BuildPlain(std::string_view partySlotToken, std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return "JJQj  ";
    }

    std::string plain = "JJQj ";
    plain.append(partySlotToken);
    plain.push_back(' ');
    plain.append(encoded.Value());
    plain.push_back(' ');
    return plain;
}

std::string FullPayload(std::string_view name)
{
    std::string payload = "1|123|35|500|450|120|";
    payload.append(name);
    payload.push_back('|');
    return payload;
}

std::string ServerRawLegacyName()
{
    return Bytes({
        0x81U, 0x7CU, 0x81U, 0x5CU,
        0x41U, 0x5CU, 0x7AU, 0x42U, 0x5CU, 0x63U,
        0x43U, 0x5CU, 0x79U, 0x44U, 0x5CU, 0x6EU, 0x45U,
    });
}

std::string ConsumerSemanticName()
{
    return Bytes({
        0x81U, 0x7CU, 0x81U, 0x5CU,
        0x41U, 0x7CU, 0x42U, 0x2CU, 0x43U, 0x5CU, 0x44U, 0x0AU, 0x45U,
    });
}

int ExpectPartyMemberStatusPacket(
    const cg::packet::Packet& packet,
    int partySlot,
    std::string_view data,
    cg::packet::PacketFieldType dataType,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(PartyMemberStatus::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "partySlot", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[0].value == std::to_string(partySlot), message);
    failures += Expect(packet.fields[1].name == "data", message);
    failures += Expect(packet.fields[1].type == dataType, message);
    failures += Expect(packet.fields[1].value == data, message);
    return failures;
}

int ExpectAllMember(
    const PartyMemberStatus& value,
    int partySlot,
    int objectIndex,
    int level,
    int maxHp,
    int hp,
    int forcePoint,
    std::string_view name,
    const char* message)
{
    int failures = 0;
    failures += Expect(value.partySlot == partySlot, message);
    failures += Expect(value.kind == PartyMemberStatusKind::All, message);
    failures += Expect(value.member.objectIndex == objectIndex, message);
    failures += Expect(value.member.level == level, message);
    failures += Expect(value.member.maxHp == maxHp, message);
    failures += Expect(value.member.hp == hp, message);
    failures += Expect(value.member.forcePoint == forcePoint, message);
    failures += Expect(value.member.name == name, message);
    return failures;
}

int TestSpecAndOuterParse()
{
    const std::string payload = FullPayload("Alice");
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlain("2", payload));

    int failures = Expect(
        PartyMemberStatus::Fields[1U].type == cg::packet::PacketFieldType::SubPayload,
        "JJQj data Spec SubPayload");
    failures += Expect(static_cast<bool>(parsed), "parse JJQj outer");
    if (parsed.HasValue()) {
        failures += ExpectPartyMemberStatusPacket(
            parsed.Value(),
            2,
            payload,
            cg::packet::PacketFieldType::EscapedString,
            "parse JJQj outer runtime type");
    }
    return failures;
}

int TestEmptyOrRemove()
{
    const cg::packet::PacketResult<PartyMemberStatus> parsed =
        PartyMemberStatus::Parse(BuildPlain("0", "0|"));

    int failures = Expect(static_cast<bool>(parsed), "parse JJQj empty/remove");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().partySlot == 0, "JJQj empty/remove partySlot");
    failures += Expect(parsed.Value().kind == PartyMemberStatusKind::EmptyOrRemove, "JJQj empty/remove kind");
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(data.HasValue(), "JJQj empty/remove BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == "0|", "JJQj empty/remove payload");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(packet.HasValue(), "JJQj empty/remove ToPacket");
    if (packet.HasValue()) {
        failures += ExpectPartyMemberStatusPacket(
            packet.Value(),
            0,
            "0|",
            cg::packet::PacketFieldType::SubPayload,
            "JJQj empty/remove ToPacket fields");
    }
    return failures;
}

int TestIndependentFieldsRoundtrip()
{
    const std::string payload = FullPayload("Alice");
    const cg::packet::PacketResult<PartyMemberStatus> parsed =
        PartyMemberStatus::Parse(BuildPlain("2", payload));

    int failures = Expect(static_cast<bool>(parsed), "parse JJQj independent fields");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectAllMember(parsed.Value(), 2, 123, 35, 500, 450, 120, "Alice", "JJQj all value");

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(data.HasValue(), "JJQj independent BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "JJQj independent payload");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(packet.HasValue(), "JJQj independent ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }
    failures += ExpectPartyMemberStatusPacket(
        packet.Value(),
        2,
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "JJQj independent ToPacket fields");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(built.HasValue(), "build JJQj independent packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlain("2", payload), "JJQj independent canonical wire");
    }
    return failures;
}

int TestConsumerLegacyGbkBoundaries()
{
    const std::string payload = FullPayload(ServerRawLegacyName());
    const cg::packet::PacketResult<PartyMemberStatus> parsed =
        PartyMemberStatus::Parse(BuildPlain("2", payload));

    int failures = Expect(static_cast<bool>(parsed), "parse JJQj GBK/legacy name");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += ExpectAllMember(
        parsed.Value(),
        2,
        123,
        35,
        500,
        450,
        120,
        ConsumerSemanticName(),
        "JJQj consumer semantic name");

    failures += ExpectError(
        parsed.Value().BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "JJQj",
        true,
        6U,
        "JJQj Server raw builder rejects decoded literal pipe");
    failures += ExpectError(
        parsed.Value().ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "JJQj",
        true,
        6U,
        "JJQj ToPacket preserves Server raw asymmetry");
    return failures;
}

int TestServerRawBuildBoundaries()
{
    std::string rawName = Bytes({0x81U, 0x7CU, 0x81U, 0x5CU});
    rawName.append("Raw\\");
    const std::string payload = FullPayload(rawName);
    const PartyMemberStatus value(2, PartyMemberStatusEntry {123, 35, 500, 450, 120, rawName});

    const cg::packet::PacketResult<std::string> data = value.BuildData();
    int failures = Expect(data.HasValue(), "JJQj Server raw BuildData accepts GBK trails/raw slash");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "JJQj Server raw BuildData bytes");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = value.ToPacket();
    failures += Expect(packet.HasValue(), "JJQj Server raw ToPacket");
    if (packet.HasValue()) {
        failures += ExpectPartyMemberStatusPacket(
            packet.Value(),
            2,
            payload,
            cg::packet::PacketFieldType::SubPayload,
            "JJQj Server raw ToPacket fields");
        failures += Expect(BuildPacket(packet.Value()).HasValue(), "JJQj Server raw BuildPacket");
    }

    failures += ExpectError(
        PartyMemberStatus::Parse(BuildPlain("2", payload)),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JJQj",
        true,
        6U,
        "JJQj consumer rejects raw trailing slash");
    return failures;
}

int TestEmptyNameAndSignedDecimals()
{
    const std::string emptyNamePayload = "1|123|35|500|450|120||";
    const cg::packet::PacketResult<PartyMemberStatus> emptyName =
        PartyMemberStatus::Parse(BuildPlain("2", emptyNamePayload));

    int failures = Expect(emptyName.HasValue(), "parse JJQj empty name");
    if (emptyName.HasValue()) {
        failures += Expect(emptyName.Value().member.name.empty(), "JJQj empty semantic name");
        const cg::packet::PacketResult<std::string> data = emptyName.Value().BuildData();
        failures += Expect(data.HasValue(), "JJQj empty name BuildData");
        if (data.HasValue()) {
            failures += Expect(data.Value() == emptyNamePayload, "JJQj empty name payload");
        }
    }

    const std::string signedPayload =
        "1|-2147483648|2147483647|-1|0|-2147483648|N|";
    const cg::packet::PacketResult<PartyMemberStatus> signedValue =
        PartyMemberStatus::Parse(BuildPlain("-a", signedPayload));
    failures += Expect(signedValue.HasValue(), "parse JJQj signed raw decimals");
    if (signedValue.HasValue()) {
        failures += ExpectAllMember(
            signedValue.Value(),
            -10,
            std::numeric_limits<int>::min(),
            std::numeric_limits<int>::max(),
            -1,
            0,
            std::numeric_limits<int>::min(),
            "N",
            "JJQj signed raw values");
        const cg::packet::PacketResult<std::string> data = signedValue.Value().BuildData();
        failures += Expect(data.HasValue(), "JJQj signed BuildData");
        if (data.HasValue()) {
            failures += Expect(data.Value() == signedPayload, "JJQj signed canonical payload");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        PartyMemberStatus::Parse("Rspn 0 0| "),
        cg::packet::PacketErrorCode::InvalidCode,
        "Rspn",
        false,
        0U,
        "JJQj wrong code");
    failures += ExpectError(
        PartyMemberStatus::Parse("JJQj 0"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JJQj",
        true,
        1U,
        "JJQj missing data");
    failures += ExpectError(
        PartyMemberStatus::Parse("JJQj _ 0| "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JJQj",
        true,
        0U,
        "JJQj bad partySlot i62");
    failures += ExpectError(
        PartyMemberStatus::Parse(BuildPlain("2", "1|123l35|500|450|120|Alice|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JJQj",
        true,
        1U,
        "JJQj rejects legacy objectIndexllevel");
    failures += ExpectError(
        PartyMemberStatus::Parse(BuildPlain("2", "1|123|35|500|450|120|Alice")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JJQj",
        true,
        1U,
        "JJQj requires trailing delimiter");
    failures += ExpectError(
        PartyMemberStatus::Parse(BuildPlain("2", "0||")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JJQj",
        true,
        1U,
        "JJQj fieldMask 0 exact payload");
    failures += ExpectError(
        PartyMemberStatus::Parse(BuildPlain("2", "2|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JJQj",
        true,
        1U,
        "JJQj unknown fieldMask");
    failures += ExpectError(
        PartyMemberStatus::Parse(BuildPlain("2", "1|123|35|500|450|120|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JJQj",
        true,
        1U,
        "JJQj missing name field");
    failures += ExpectError(
        PartyMemberStatus::Parse(BuildPlain("2", "1|123|35|500|450|120|Alice|extra|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JJQj",
        true,
        1U,
        "JJQj extra payload field");
    failures += ExpectError(
        PartyMemberStatus::Parse(BuildPlain("2", "1|bad|35|500|450|120|Alice|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JJQj",
        true,
        1U,
        "JJQj bad objectIndex decimal");
    failures += ExpectError(
        PartyMemberStatus::Parse(BuildPlain("2", "1|123|2147483648|500|450|120|Alice|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JJQj",
        true,
        2U,
        "JJQj level overflow");

    std::string truncatedPayload = "1|123|35|500|450|120|";
    truncatedPayload.push_back(static_cast<char>(0x81));
    truncatedPayload.push_back('|');
    failures += ExpectError(
        PartyMemberStatus::Parse(BuildPlain("2", truncatedPayload)),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JJQj",
        true,
        1U,
        "JJQj rejects truncated GBK name on parse");

    failures += ExpectError(
        PartyMemberStatus(2, PartyMemberStatusEntry {123, 35, 500, 450, 120, Bytes({0x81U})})
            .BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "JJQj",
        true,
        6U,
        "JJQj rejects truncated GBK name on build");
    failures += ExpectError(
        PartyMemberStatus(2, PartyMemberStatusEntry {123, 35, 500, 450, 120, "A|B"})
            .BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "JJQj",
        true,
        6U,
        "JJQj Server raw builder rejects ASCII pipe");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestSpecAndOuterParse();
    failures += TestEmptyOrRemove();
    failures += TestIndependentFieldsRoundtrip();
    failures += TestConsumerLegacyGbkBoundaries();
    failures += TestServerRawBuildBoundaries();
    failures += TestEmptyNameAndSignedDecimals();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
