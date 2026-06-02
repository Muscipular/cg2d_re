#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <utility>

namespace {

constexpr std::string_view EmptyPlain = "JJQj 0 0| ";
constexpr std::string_view AllPlain = "JJQj 2 1|123l35|500|450|120|Alice| ";
constexpr std::string_view AllPayload = "1|123l35|500|450|120|Alice|";

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
    return failures;
}

int ExpectPartyMemberStatusPacket(
    const cg::packet::Packet& packet,
    int partySlot,
    std::string_view data,
    const char* message)
{
    int failures = 0;
    failures += Expect(
        packet.code == cg::packet::PacketCode(cg::packet::receive::PartyMemberStatus::Code),
        message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "partySlot", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[0].value == std::to_string(partySlot), message);
    failures += Expect(packet.fields[1].name == "data", message);
    failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[1].value == data, message);
    return failures;
}

int ExpectAllMember(
    const cg::packet::receive::PartyMemberStatus& value,
    int partySlot,
    const char* message)
{
    int failures = 0;
    failures += Expect(value.partySlot == partySlot, message);
    failures += Expect(value.kind == cg::packet::receive::PartyMemberStatusKind::All, message);
    failures += Expect(value.member.objectIndex == 123, message);
    failures += Expect(value.member.level == 35, message);
    failures += Expect(value.member.maxHp == 500, message);
    failures += Expect(value.member.hp == 450, message);
    failures += Expect(value.member.forcePoint == 120, message);
    failures += Expect(value.member.name == "Alice", message);
    return failures;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed = cg::packet::ParsePacket(EmptyPlain);

    int failures = Expect(static_cast<bool>(parsed), "parse JJQj outer");
    if (parsed.HasValue()) {
        failures += ExpectPartyMemberStatusPacket(parsed.Value(), 0, "0|", "parse JJQj outer packet");
    }
    return failures;
}

int TestEmptyOrRemove()
{
    const cg::packet::PacketResult<cg::packet::receive::PartyMemberStatus> parsed =
        cg::packet::receive::PartyMemberStatus::Parse(EmptyPlain);

    int failures = Expect(static_cast<bool>(parsed), "parse JJQj empty/remove");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().partySlot == 0, "JJQj empty/remove partySlot");
        failures += Expect(
            parsed.Value().kind == cg::packet::receive::PartyMemberStatusKind::EmptyOrRemove,
            "JJQj empty/remove kind");
        failures += Expect(parsed.Value().BuildData() == "0|", "JJQj empty/remove payload");
    }
    return failures;
}

int TestAllMember()
{
    const cg::packet::PacketResult<cg::packet::receive::PartyMemberStatus> parsed =
        cg::packet::receive::PartyMemberStatus::Parse(AllPlain);

    int failures = Expect(static_cast<bool>(parsed), "parse JJQj all");
    if (parsed.HasValue()) {
        failures += ExpectAllMember(parsed.Value(), 2, "JJQj all value");
        failures += Expect(parsed.Value().BuildData() == AllPayload, "JJQj all payload");
    }
    return failures;
}

int TestRoundtripPreservesObjectLevelAndTrailingDelimiter()
{
    cg::packet::receive::PartyMemberStatusEntry member;
    member.objectIndex = 123;
    member.level = 35;
    member.maxHp = 500;
    member.hp = 450;
    member.forcePoint = 120;
    member.name = "Alice";

    const cg::packet::receive::PartyMemberStatus value(2, std::move(member));
    const cg::packet::Packet packet = value.ToPacket();

    int failures = ExpectPartyMemberStatusPacket(packet, 2, AllPayload, "JJQj ToPacket");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build JJQj packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == AllPlain, "build JJQj preserves objectIndexllevel and trailing delimiter");

    const cg::packet::PacketResult<cg::packet::receive::PartyMemberStatus> parsed =
        cg::packet::receive::PartyMemberStatus::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip JJQj packet");
    if (parsed.HasValue()) {
        failures += ExpectAllMember(parsed.Value(), 2, "roundtrip JJQj value");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::PartyMemberStatus::Parse("JJQj 0"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JJQj",
        "JJQj missing data");
    failures += ExpectError(
        cg::packet::receive::PartyMemberStatus::Parse("JJQj _ 0| "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JJQj",
        "JJQj bad partySlot i62");
    failures += ExpectError(
        cg::packet::receive::PartyMemberStatus::Parse("JJQj 2 1|12335|500|450|120|Alice| "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JJQj",
        "JJQj fieldMask 1 missing l");
    failures += ExpectError(
        cg::packet::receive::PartyMemberStatus::Parse("JJQj 2 1|123l35|500|450|120| "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "JJQj",
        "JJQj fieldMask 1 too few fields");
    failures += ExpectError(
        cg::packet::receive::PartyMemberStatus::Parse("JJQj 2 2| "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "JJQj",
        "JJQj unknown fieldMask");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestEmptyOrRemove();
    failures += TestAllMember();
    failures += TestRoundtripPreservesObjectLevelAndTrailingDelimiter();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
