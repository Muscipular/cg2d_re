#include "packet/Send.hpp"

#include <array>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <string>
#include <string_view>

namespace {

using cg::packet::send::BattleCommandPayload;
using cg::packet::send::BattleCommandRequest;

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

int TestSpecAndEncode()
{
    const BattleCommandRequest request(BattleCommandPayload::PetNoAction());
    const cg::packet::Packet packet = request.ToPacket();

    int failures = 0;
    failures += Expect(BattleCommandRequest::Code == "Rg", "Rg code");
    failures += Expect(
        BattleCommandRequest::Direction == cg::packet::PacketDirection::ClientToServer,
        "Rg direction");
    failures += Expect(BattleCommandRequest::Fields.size() == 1U, "Rg fields");
    failures += Expect(BattleCommandRequest::Fields[0].name == "command", "Rg command name");
    failures += Expect(
        BattleCommandRequest::Fields[0].type == cg::packet::PacketFieldType::SubPayload,
        "Rg command Spec type");
    failures += Expect(BattleCommandRequest::Spec.code == BattleCommandRequest::Code, "Rg Spec code");
    failures += Expect(
        BattleCommandRequest::Spec.direction == BattleCommandRequest::Direction,
        "Rg Spec direction");
    failures += Expect(BattleCommandRequest::Spec.fieldCount == 1U, "Rg Spec field count");
    failures += Expect(packet.code == cg::packet::PacketCode("Rg"), "Rg packet code");
    failures += Expect(
        packet.direction == cg::packet::PacketDirection::ClientToServer,
        "Rg packet direction");
    failures += Expect(packet.fields.size() == 1U, "Rg packet fields");
    if (packet.fields.size() == 1U) {
        failures += Expect(packet.fields[0].name == "command", "Rg packet command name");
        failures += Expect(
            packet.fields[0].type == cg::packet::PacketFieldType::EscapedString,
            "Rg packet command actual type");
        failures += Expect(packet.fields[0].value == "W|FF", "Rg packet command value");
    }

    const cg::packet::PacketResult<std::string> encoded = request.Encode();
    failures += Expect(encoded.HasValue(), "Rg Encode");
    if (encoded.HasValue()) {
        failures += Expect(encoded.Value() == "Rg W|FF ", "Rg Encode value");
    }
    return failures;
}

int TestAllFactories()
{
    struct CommandCase
    {
        BattleCommandPayload payload;
        std::string_view expected;
    };

    const std::array<CommandCase, 13U> cases = {{
        {BattleCommandPayload::PlayerSkill(0x1AU, 0x2BU, 0xFFFFFFFFU), "S|1A|2B|FFFFFFFF"},
        {BattleCommandPayload::PetSkill(0xCDU, 0xEFU), "W|CD|EF"},
        {BattleCommandPayload::PetNoAction(), "W|FF"},
        {BattleCommandPayload::Attack(0xABCU), "H|ABC"},
        {BattleCommandPayload::Equipment(0x12U), "Q|12|"},
        {BattleCommandPayload::PetChange(0x23U), "M|23"},
        {BattleCommandPayload::UseItem(0x34U, 0x45U), "I|34|45"},
        {BattleCommandPayload::Position(), "P"},
        {BattleCommandPayload::NoAction(), "N"},
        {BattleCommandPayload::Guard(), "G"},
        {BattleCommandPayload::Escape(), "E"},
        {BattleCommandPayload::WatchStop(), "U"},
        {BattleCommandPayload::Rebirth(0x56U), "R|56"},
    }};

    int failures = 0;
    for (const CommandCase& commandCase : cases) {
        failures += Expect(
            commandCase.payload.Encode() == commandCase.expected,
            "Rg factory canonical");
    }
    failures += Expect(
        BattleCommandPayload::Attack(0U).Encode() == "H|0",
        "Rg zero uppercase hex operand");
    return failures;
}

int TestGenericRoundtripAndFixturePayload()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket("Rg W|FF", BattleCommandRequest::Spec);

    int failures = Expect(parsed.HasValue(), "Rg generic parse");
    if (!parsed.HasValue()) {
        return failures;
    }
    failures += Expect(parsed.Value().fields.size() == 1U, "Rg parsed field count");
    if (parsed.Value().fields.size() == 1U) {
        const std::string& payload = parsed.Value().fields[0].value;
        const std::array<unsigned char, 4U> expected = {0x57U, 0x7CU, 0x46U, 0x46U};
        failures += Expect(payload.size() == expected.size(), "Rg fixture payload size");
        if (payload.size() == expected.size()) {
            for (std::size_t index = 0U; index < expected.size(); ++index) {
                failures += Expect(
                    static_cast<unsigned char>(payload[index]) == expected[index],
                    "Rg fixture payload byte");
            }
        }
    }

    const cg::packet::PacketResult<std::string> rebuilt =
        cg::packet::BuildPacket(parsed.Value(), BattleCommandRequest::Spec);
    failures += Expect(rebuilt.HasValue(), "Rg generic build");
    if (rebuilt.HasValue()) {
        failures += Expect(rebuilt.Value() == "Rg W|FF ", "Rg generic canonical");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::ParsePacket("bad W|FF ", BattleCommandRequest::Spec),
        cg::packet::PacketErrorCode::InvalidCode,
        "bad",
        false,
        0U,
        "Rg wrong code");
    failures += ExpectError(
        cg::packet::ParsePacket("Rg", BattleCommandRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Rg",
        true,
        0U,
        "Rg missing field");
    failures += ExpectError(
        cg::packet::ParsePacket("Rg W|FF extra ", BattleCommandRequest::Spec),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Rg",
        true,
        1U,
        "Rg extra field");

    std::string invalidEs = "Rg ";
    invalidEs.push_back(static_cast<char>(0x80U));
    invalidEs.push_back(' ');
    failures += ExpectError(
        cg::packet::ParsePacket(invalidEs, BattleCommandRequest::Spec),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Rg",
        true,
        0U,
        "Rg invalid es");
    return failures;
}

} // namespace

int main()
{
    const int failures =
        TestSpecAndEncode()
        + TestAllFactories()
        + TestGenericRoundtripAndFixturePayload()
        + TestErrors();
    if (failures != 0) {
        std::cerr << failures << " battle command request packet test(s) failed\n";
        return 1;
    }
    return 0;
}
