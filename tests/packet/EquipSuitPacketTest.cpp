#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

const cg::packet::PacketSpec& PacketSpec() noexcept
{
    return cg::packet::receive::EquipSuit::Spec;
}

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, PacketSpec());
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, PacketSpec());
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

std::string BuildPlain(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedPayload = cg::packet::EncodeEscapedString(payload);
    if (!encodedPayload.HasValue()) {
        std::string plain(cg::packet::receive::EquipSuit::Code);
        plain.append("  ");
        return plain;
    }

    std::string plain(cg::packet::receive::EquipSuit::Code);
    plain.push_back(' ');
    plain.append(encodedPayload.Value());
    plain.push_back(' ');
    return plain;
}

std::string JoinParts(const std::vector<std::string>& parts)
{
    std::string payload;
    for (std::size_t index = 0; index < parts.size(); ++index) {
        if (index != 0U) {
            payload.push_back(',');
        }
        payload.append(parts[index]);
    }
    return payload;
}

std::vector<std::string> BuildEquipSuitParts()
{
    return {
        "Name\\cComma",
        " ",
        "1",
        "10",
        "-20",
        "0",
        "40",
        "-5",
        "6",
        "7",
        "-8",
        "9",
        "0",
        "11",
        "-12",
        "13",
        "0",
        "-15",
        "16",
        "17",
        "-18",
        "19",
        "20",
        "-21",
        "22",
        "23",
        "-24",
    };
}

int ExpectEquipSuitPacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::EquipSuit::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[0].value == value, message);
    return failures;
}

int ExpectSampleValue(const cg::packet::receive::EquipSuit& equipSuit, const char* message)
{
    int failures = 0;
    failures += Expect(equipSuit.active, message);
    failures += Expect(equipSuit.nameRaw == "Name\\cComma", message);
    failures += Expect(equipSuit.skillNameRaw == " ", message);
    failures += Expect(equipSuit.specialTitleFlag == 1, message);
    failures += Expect(equipSuit.attackPercent == 10, message);
    failures += Expect(equipSuit.attackValue == -20, message);
    failures += Expect(equipSuit.defensePercent == 0, message);
    failures += Expect(equipSuit.defenseValue == 40, message);
    failures += Expect(equipSuit.agilityPercent == -5, message);
    failures += Expect(equipSuit.agilityValue == 6, message);
    failures += Expect(equipSuit.spiritPercent == 7, message);
    failures += Expect(equipSuit.spiritValue == -8, message);
    failures += Expect(equipSuit.recoveryPercent == 9, message);
    failures += Expect(equipSuit.recoveryValue == 0, message);
    failures += Expect(equipSuit.hpPercent == 11, message);
    failures += Expect(equipSuit.hpValue == -12, message);
    failures += Expect(equipSuit.fpPercent == 13, message);
    failures += Expect(equipSuit.fpValue == 0, message);
    failures += Expect(equipSuit.poisonResist == -15, message);
    failures += Expect(equipSuit.sleepResist == 16, message);
    failures += Expect(equipSuit.stoneResist == 17, message);
    failures += Expect(equipSuit.drunkResist == -18, message);
    failures += Expect(equipSuit.confusionResist == 19, message);
    failures += Expect(equipSuit.amnesiaResist == 20, message);
    failures += Expect(equipSuit.critical == -21, message);
    failures += Expect(equipSuit.counter == 22, message);
    failures += Expect(equipSuit.hitRate == 23, message);
    failures += Expect(equipSuit.avoid == -24, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = JoinParts(BuildEquipSuitParts());
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlain(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse TgX outer");
    if (parsed.HasValue()) {
        failures += ExpectEquipSuitPacket(parsed.Value(), payload, "parse TgX outer packet");
    }
    return failures;
}

int TestFullRoundtrip()
{
    const std::string payload = JoinParts(BuildEquipSuitParts());
    const cg::packet::PacketResult<cg::packet::receive::EquipSuit> parsed =
        cg::packet::receive::EquipSuit::Parse(BuildPlain(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse TgX full");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectSampleValue(parsed.Value(), "TgX sample fields");
    failures += Expect(parsed.Value().BuildData() == payload, "TgX BuildData");
    failures += Expect(!parsed.Value().BuildData().empty(), "TgX BuildData non-empty");
    failures += Expect(parsed.Value().BuildData().back() != ',', "TgX BuildData no trailing comma");
    failures += ExpectEquipSuitPacket(parsed.Value().ToPacket(), payload, "TgX ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build TgX packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlain(payload), "roundtrip TgX packet");
    }
    return failures;
}

int TestEmptyPayload()
{
    const cg::packet::PacketResult<cg::packet::receive::EquipSuit> parsed =
        cg::packet::receive::EquipSuit::Parse(BuildPlain(""));

    int failures = Expect(static_cast<bool>(parsed), "parse TgX empty data");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(!parsed.Value().active, "TgX empty inactive");
    failures += Expect(parsed.Value().BuildData().empty(), "TgX empty BuildData");
    failures += ExpectEquipSuitPacket(parsed.Value().ToPacket(), "", "TgX empty ToPacket");
    return failures;
}

int TestLegacyDoubleByteComma()
{
    std::vector<std::string> parts = BuildEquipSuitParts();
    std::string legacyName;
    legacyName.push_back(static_cast<char>(0x81));
    legacyName.push_back(',');
    legacyName.push_back('A');
    parts[0] = legacyName;

    const std::string payload = JoinParts(parts);
    const cg::packet::PacketResult<cg::packet::receive::EquipSuit> parsed =
        cg::packet::receive::EquipSuit::Parse(BuildPlain(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse TgX legacy comma");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().nameRaw == legacyName, "TgX legacy comma name");
        failures += Expect(parsed.Value().BuildData() == payload, "TgX legacy comma BuildData");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;

    std::vector<std::string> parts = BuildEquipSuitParts();
    parts.pop_back();
    failures += ExpectError(
        cg::packet::receive::EquipSuit::Parse(BuildPlain(JoinParts(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TgX",
        "TgX missing payload field");

    parts = BuildEquipSuitParts();
    parts.push_back("extra");
    failures += ExpectError(
        cg::packet::receive::EquipSuit::Parse(BuildPlain(JoinParts(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TgX",
        "TgX extra payload field");

    parts = BuildEquipSuitParts();
    parts[2] = "2";
    failures += ExpectError(
        cg::packet::receive::EquipSuit::Parse(BuildPlain(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TgX",
        "TgX invalid flag");

    parts = BuildEquipSuitParts();
    parts[3].clear();
    failures += ExpectError(
        cg::packet::receive::EquipSuit::Parse(BuildPlain(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TgX",
        "TgX empty numeric");

    parts = BuildEquipSuitParts();
    parts[4] = "x";
    failures += ExpectError(
        cg::packet::receive::EquipSuit::Parse(BuildPlain(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TgX",
        "TgX invalid numeric");

    parts = BuildEquipSuitParts();
    parts[5] = "2147483648";
    failures += ExpectError(
        cg::packet::receive::EquipSuit::Parse(BuildPlain(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TgX",
        "TgX numeric overflow");

    failures += ExpectError(
        cg::packet::receive::EquipSuit::Parse("rU data "),
        cg::packet::PacketErrorCode::InvalidCode,
        "rU",
        "TgX wrong code");

    failures += ExpectError(
        cg::packet::receive::EquipSuit::Parse("TgX"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TgX",
        "TgX missing outer data");

    failures += ExpectError(
        cg::packet::receive::EquipSuit::Parse("TgX data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "TgX",
        "TgX extra outer data");

    const std::string payload = JoinParts(BuildEquipSuitParts());
    cg::packet::Packet manual(cg::packet::PacketCode(cg::packet::receive::EquipSuit::Code),
        cg::packet::PacketDirection::ClientToServer);
    manual.fields.push_back(cg::packet::PacketField {
        "data",
        cg::packet::PacketFieldType::EscapedString,
        payload,
    });
    failures += ExpectError(
        cg::packet::receive::EquipSuit::FromPacket(manual),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TgX",
        "TgX wrong direction");

    manual.direction = cg::packet::PacketDirection::ServerToClient;
    manual.fields[0].name = "payload";
    failures += ExpectError(
        cg::packet::receive::EquipSuit::FromPacket(manual),
        cg::packet::PacketErrorCode::DecodeFailed,
        "TgX",
        "TgX wrong field name");

    manual.fields[0].name = "data";
    manual.fields[0].type = cg::packet::PacketFieldType::Int62;
    failures += ExpectError(
        cg::packet::receive::EquipSuit::FromPacket(manual),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "TgX",
        "TgX wrong field type");

    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestFullRoundtrip();
    failures += TestEmptyPayload();
    failures += TestLegacyDoubleByteComma();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
