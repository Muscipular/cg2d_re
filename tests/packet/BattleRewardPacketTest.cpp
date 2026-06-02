#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::BattleReward::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::BattleReward::Spec);
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

std::string JoinComma(const std::vector<std::string>& parts)
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

std::vector<std::string> BuildSampleParts()
{
    std::vector<std::string> parts = {
        "-1|1|a",
        "-2|0|-1",
        "3|0|10",
        "0|0|0",
        "100\\zName\\cRaw||200\\zOther\\nRaw",
    };

    parts.reserve(cg::packet::receive::BattleReward::ActorCount +
        1U + cg::packet::receive::BattleReward::SkillCount);
    parts.push_back("1|a");
    parts.push_back("0|-1");
    parts.push_back("1|10");
    for (std::size_t index = 3U; index < cg::packet::receive::BattleReward::SkillCount; ++index) {
        parts.push_back(std::to_string(index % 2U) + "|0");
    }
    return parts;
}

std::string BuildPlain(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedPayload = cg::packet::EncodeEscapedString(payload);
    if (!encodedPayload.HasValue()) {
        return "Gn  ";
    }

    std::string plain(cg::packet::receive::BattleReward::Code);
    plain.push_back(' ');
    plain.append(encodedPayload.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectBattleRewardPacket(
    const cg::packet::Packet& packet,
    std::string_view data,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::BattleReward::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[0].value == data, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = JoinComma(BuildSampleParts());
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlain(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse Gn outer");
    if (parsed.HasValue()) {
        failures += ExpectBattleRewardPacket(parsed.Value(), payload, "parse Gn outer packet");
    }
    return failures;
}

int TestFullRoundtrip()
{
    const std::string payload = JoinComma(BuildSampleParts());
    const cg::packet::PacketResult<cg::packet::receive::BattleReward> parsed =
        cg::packet::receive::BattleReward::Parse(BuildPlain(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse Gn full");
    if (!parsed.HasValue()) {
        return failures;
    }

    const cg::packet::receive::BattleReward& reward = parsed.Value();
    failures += Expect(reward.actors[0].targetIndex == -1, "Gn actor0 target");
    failures += Expect(reward.actors[0].levelUpFlag == 1, "Gn actor0 levelUp");
    failures += Expect(reward.actors[0].expDelta == 10, "Gn actor0 exp i62 a");
    failures += Expect(reward.actors[1].targetIndex == -2, "Gn actor1 target");
    failures += Expect(reward.actors[1].expDelta == -1, "Gn actor1 exp -1");
    failures += Expect(reward.actors[2].targetIndex == 3, "Gn actor2 pet index");
    failures += Expect(reward.actors[2].expDelta == 62, "Gn actor2 exp 10");

    failures += Expect(reward.items[0].hasItem, "Gn item0 exists");
    failures += Expect(reward.items[0].imageNumber == 100, "Gn item0 image");
    failures += Expect(reward.items[0].hasNameRaw, "Gn item0 has raw name");
    failures += Expect(reward.items[0].nameRaw == "Name\\cRaw", "Gn item0 raw name");
    failures += Expect(!reward.items[1].hasItem, "Gn item1 empty");
    failures += Expect(reward.items[2].hasItem, "Gn item2 exists");
    failures += Expect(reward.items[2].nameRaw == "Other\\nRaw", "Gn item2 raw name");

    failures += Expect(reward.skills[0].levelUpFlag == 1, "Gn skill0 levelUp");
    failures += Expect(reward.skills[0].expDelta == 10, "Gn skill0 exp a");
    failures += Expect(reward.skills[1].expDelta == -1, "Gn skill1 exp -1");
    failures += Expect(reward.skills[2].expDelta == 62, "Gn skill2 exp 10");
    failures += Expect(reward.BuildData() == payload, "Gn BuildData exact");
    failures += Expect(!reward.BuildData().empty() && reward.BuildData().back() != ',', "Gn BuildData no trailing comma");
    failures += ExpectBattleRewardPacket(reward.ToPacket(), payload, "Gn ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(reward.ToPacket());
    failures += Expect(static_cast<bool>(built), "build Gn packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == BuildPlain(payload), "build Gn plain");
    const cg::packet::PacketResult<cg::packet::receive::BattleReward> roundtrip =
        cg::packet::receive::BattleReward::Parse(built.Value());
    failures += Expect(static_cast<bool>(roundtrip), "roundtrip Gn");
    if (roundtrip.HasValue()) {
        failures += Expect(roundtrip.Value().items[0].nameRaw == "Name\\cRaw", "roundtrip Gn raw item name");
    }
    return failures;
}

int TestEmptyItems()
{
    std::vector<std::string> parts = BuildSampleParts();
    parts[4] = "||";
    const std::string payload = JoinComma(parts);
    const cg::packet::PacketResult<cg::packet::receive::BattleReward> parsed =
        cg::packet::receive::BattleReward::Parse(BuildPlain(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse Gn empty items");
    if (parsed.HasValue()) {
        failures += Expect(!parsed.Value().items[0].hasItem, "Gn empty item0");
        failures += Expect(!parsed.Value().items[1].hasItem, "Gn empty item1");
        failures += Expect(!parsed.Value().items[2].hasItem, "Gn empty item2");
        failures += Expect(parsed.Value().BuildData() == payload, "Gn empty items BuildData");
    }
    return failures;
}

int TestRawOuterEscapes()
{
    const std::string payload = JoinComma(BuildSampleParts());
    const std::string plain = std::string(cg::packet::receive::BattleReward::Code) + " " + payload + " ";
    const cg::packet::PacketResult<cg::packet::receive::BattleReward> parsed =
        cg::packet::receive::BattleReward::Parse(plain);

    int failures = Expect(static_cast<bool>(parsed), "parse Gn raw outer escapes");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().items[0].nameRaw == "Name\\cRaw", "Gn raw outer keeps c");
        failures += Expect(parsed.Value().items[2].nameRaw == "Other\\nRaw", "Gn raw outer keeps n");
    }
    return failures;
}

int TestPayloadErrors()
{
    int failures = 0;
    std::vector<std::string> parts = BuildSampleParts();
    parts.pop_back();
    failures += ExpectError(
        cg::packet::receive::BattleReward::Parse(BuildPlain(JoinComma(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Gn",
        "Gn top-level 19 parts");

    parts = BuildSampleParts();
    parts.push_back("0|0");
    failures += ExpectError(
        cg::packet::receive::BattleReward::Parse(BuildPlain(JoinComma(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Gn",
        "Gn top-level 21 parts");

    parts = BuildSampleParts();
    parts[0] = "-1|1";
    failures += ExpectError(
        cg::packet::receive::BattleReward::Parse(BuildPlain(JoinComma(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Gn",
        "Gn actor segment count");

    parts = BuildSampleParts();
    parts[5] = "1";
    failures += ExpectError(
        cg::packet::receive::BattleReward::Parse(BuildPlain(JoinComma(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Gn",
        "Gn skill segment count");

    parts = BuildSampleParts();
    parts[0] = "x|1|a";
    failures += ExpectError(
        cg::packet::receive::BattleReward::Parse(BuildPlain(JoinComma(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Gn",
        "Gn invalid actor decimal");

    parts = BuildSampleParts();
    parts[0] = "-1|1|_";
    failures += ExpectError(
        cg::packet::receive::BattleReward::Parse(BuildPlain(JoinComma(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Gn",
        "Gn invalid actor i62");

    parts = BuildSampleParts();
    parts[4] = "x\\zName||";
    failures += ExpectError(
        cg::packet::receive::BattleReward::Parse(BuildPlain(JoinComma(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Gn",
        "Gn invalid item image");

    parts = BuildSampleParts();
    parts[5] = "x|a";
    failures += ExpectError(
        cg::packet::receive::BattleReward::Parse(BuildPlain(JoinComma(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Gn",
        "Gn invalid skill decimal");

    parts = BuildSampleParts();
    parts[5] = "1|_";
    failures += ExpectError(
        cg::packet::receive::BattleReward::Parse(BuildPlain(JoinComma(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Gn",
        "Gn invalid skill i62");
    return failures;
}

int TestOuterErrors()
{
    int failures = 0;
    const std::string payload = JoinComma(BuildSampleParts());
    failures += ExpectError(
        cg::packet::receive::BattleReward::Parse("LYIm BU "),
        cg::packet::PacketErrorCode::InvalidCode,
        "LYIm",
        "Gn wrong code");
    failures += ExpectError(
        cg::packet::receive::BattleReward::Parse("Gn"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Gn",
        "Gn missing outer data");
    failures += ExpectError(
        cg::packet::receive::BattleReward::Parse("Gn data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Gn",
        "Gn extra outer data");

    cg::packet::Packet manual(cg::packet::PacketCode(cg::packet::receive::BattleReward::Code),
        cg::packet::PacketDirection::ClientToServer);
    manual.fields.push_back(cg::packet::PacketField {
        "data",
        cg::packet::PacketFieldType::EscapedString,
        payload,
    });
    failures += ExpectError(
        cg::packet::receive::BattleReward::FromPacket(manual),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Gn",
        "Gn wrong direction");

    manual.direction = cg::packet::PacketDirection::ServerToClient;
    manual.fields[0].name = "payload";
    failures += ExpectError(
        cg::packet::receive::BattleReward::FromPacket(manual),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Gn",
        "Gn wrong field name");

    manual.fields[0].name = "data";
    manual.fields[0].type = cg::packet::PacketFieldType::Int62;
    failures += ExpectError(
        cg::packet::receive::BattleReward::FromPacket(manual),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "Gn",
        "Gn wrong field type");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestFullRoundtrip();
    failures += TestEmptyItems();
    failures += TestRawOuterEscapes();
    failures += TestPayloadErrors();
    failures += TestOuterErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
