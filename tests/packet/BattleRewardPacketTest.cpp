#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

using BattleReward = cg::packet::receive::BattleReward;

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, BattleReward::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, BattleReward::Spec);
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

template <typename T>
int ExpectErrorAt(
    const cg::packet::PacketResult<T>& result,
    cg::packet::PacketErrorCode code,
    std::size_t fieldIndex,
    const char* message)
{
    int failures = ExpectError(result, code, BattleReward::Code, message);
    if (result.HasError()) {
        failures += Expect(result.Error().fieldIndex.has_value(), message);
        if (result.Error().fieldIndex.has_value()) {
            failures += Expect(result.Error().fieldIndex.value() == fieldIndex, message);
        }
    }
    return failures;
}

std::string Bytes(std::initializer_list<unsigned int> values)
{
    std::string bytes;
    bytes.reserve(values.size());
    for (const unsigned int value : values) {
        bytes.push_back(static_cast<char>(value));
    }
    return bytes;
}

std::string SemanticItemName()
{
    return Bytes({
        0xC3U,
        0xFBU,
        0x96U,
        0x7CU,
        0x41U,
        0x7CU,
        0x42U,
        0x2CU,
        0x43U,
        0x5CU,
        0x44U,
        0x0AU,
        0x45U,
    });
}

std::string EncodedItemGroup()
{
    std::string itemGroup = "7300\\z";
    itemGroup.append(Bytes({0xC3U, 0xFBU, 0x96U, 0x7CU}));
    itemGroup.append("A\\yzB\\ycC\\yyD\\ynE|||");
    return itemGroup;
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
        "-2|1|10",
        "",
        "",
        "",
        EncodedItemGroup(),
        "1|10",
    };

    for (std::size_t index = 1U; index + 1U < BattleReward::SkillCount; ++index) {
        parts.emplace_back();
    }
    parts.emplace_back("0|z");
    parts.emplace_back();
    return parts;
}

std::string BuildPlain(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedPayload =
        cg::packet::EncodeEscapedString(payload);
    if (!encodedPayload.HasValue()) {
        return "Gn  ";
    }

    std::string plain(BattleReward::Code);
    plain.push_back(' ');
    plain.append(encodedPayload.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectBattleRewardPacket(
    const cg::packet::Packet& packet,
    cg::packet::PacketFieldType fieldType,
    std::string_view data,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(BattleReward::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == fieldType, message);
    failures += Expect(packet.fields[0].value == data, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = JoinComma(BuildSampleParts());
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlain(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse Gn outer");
    if (parsed.HasValue()) {
        failures += ExpectBattleRewardPacket(
            parsed.Value(),
            cg::packet::PacketFieldType::EscapedString,
            payload,
            "parse Gn outer packet");
    }
    return failures;
}

int TestFullRoundtrip()
{
    const std::string payload = JoinComma(BuildSampleParts());
    const cg::packet::PacketResult<BattleReward> parsed = BattleReward::Parse(BuildPlain(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse Gn full");
    if (!parsed.HasValue()) {
        return failures;
    }

    const BattleReward& reward = parsed.Value();
    failures += Expect(reward.actors[0].hasData, "Gn actor0 exists");
    failures += Expect(reward.actors[0].targetIndex == -2, "Gn actor0 target");
    failures += Expect(reward.actors[0].levelUpFlag == 1, "Gn actor0 levelUp");
    failures += Expect(reward.actors[0].expDelta == 62, "Gn actor0 exp i62");
    for (std::size_t index = 1U; index < reward.actors.size(); ++index) {
        failures += Expect(!reward.actors[index].hasData, "Gn empty actor");
    }

    failures += Expect(reward.items[0].hasItem, "Gn item0 exists");
    failures += Expect(reward.items[0].imageNumber == 7300, "Gn item0 image");
    failures += Expect(reward.items[0].hasNameRaw, "Gn item0 has name");
    failures += Expect(reward.items[0].nameRaw == SemanticItemName(), "Gn item0 semantic GBK name");
    failures += Expect(!reward.items[1].hasItem, "Gn item1 empty");
    failures += Expect(!reward.items[2].hasItem, "Gn item2 empty");

    failures += Expect(reward.skills[0].hasData, "Gn skill0 exists");
    failures += Expect(reward.skills[0].levelUpFlag == 1, "Gn skill0 levelUp");
    failures += Expect(reward.skills[0].expDelta == 62, "Gn skill0 exp i62");
    for (std::size_t index = 1U; index + 1U < reward.skills.size(); ++index) {
        failures += Expect(!reward.skills[index].hasData, "Gn empty skill");
    }
    failures += Expect(reward.skills.back().hasData, "Gn last skill exists");
    failures += Expect(reward.skills.back().levelUpFlag == 0, "Gn last skill levelUp");
    failures += Expect(reward.skills.back().expDelta == 35, "Gn last skill exp i62");

    const cg::packet::PacketResult<std::string> data = reward.BuildData();
    failures += Expect(static_cast<bool>(data), "Gn BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "Gn BuildData exact");
        failures += Expect(!data.Value().empty() && data.Value().back() == ',', "Gn BuildData trailing comma");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = reward.ToPacket();
    failures += Expect(static_cast<bool>(packet), "Gn ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }
    failures += ExpectBattleRewardPacket(
        packet.Value(),
        cg::packet::PacketFieldType::SubPayload,
        payload,
        "Gn ToPacket value");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build Gn packet");
    if (!built.HasValue()) {
        return failures;
    }

    failures += Expect(built.Value() == BuildPlain(payload), "build Gn plain");
    const cg::packet::PacketResult<BattleReward> roundtrip = BattleReward::Parse(built.Value());
    failures += Expect(static_cast<bool>(roundtrip), "roundtrip Gn");
    if (roundtrip.HasValue()) {
        failures += Expect(
            roundtrip.Value().items[0].nameRaw == SemanticItemName(),
            "roundtrip Gn semantic item name");
    }
    return failures;
}

int TestEmptySlots()
{
    std::vector<std::string> parts(BattleReward::ActorCount + 1U + BattleReward::SkillCount + 1U);
    parts[BattleReward::ActorCount] = "|||";
    const std::string payload = JoinComma(parts);
    const cg::packet::PacketResult<BattleReward> parsed = BattleReward::Parse(BuildPlain(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse Gn empty slots");
    if (!parsed.HasValue()) {
        return failures;
    }

    for (const cg::packet::receive::BattleRewardActor& actor : parsed.Value().actors) {
        failures += Expect(!actor.hasData, "Gn empty actor slot");
    }
    for (const cg::packet::receive::BattleRewardItem& item : parsed.Value().items) {
        failures += Expect(!item.hasItem, "Gn empty item slot");
    }
    for (const cg::packet::receive::BattleRewardSkill& skill : parsed.Value().skills) {
        failures += Expect(!skill.hasData, "Gn empty skill slot");
    }

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "Gn empty slots BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "Gn empty slots canonical");
    }
    return failures;
}

int TestPayloadErrors()
{
    int failures = 0;
    std::vector<std::string> parts = BuildSampleParts();
    parts.pop_back();
    failures += ExpectErrorAt(
        BattleReward::Parse(BuildPlain(JoinComma(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        0U,
        "Gn missing top trailing segment");

    parts = BuildSampleParts();
    parts.emplace_back();
    failures += ExpectErrorAt(
        BattleReward::Parse(BuildPlain(JoinComma(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        0U,
        "Gn extra top segment");

    parts = BuildSampleParts();
    parts[0] = "-2|1";
    failures += ExpectError(
        BattleReward::Parse(BuildPlain(JoinComma(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        BattleReward::Code,
        "Gn actor segment count");

    parts = BuildSampleParts();
    parts[4].pop_back();
    failures += ExpectError(
        BattleReward::Parse(BuildPlain(JoinComma(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        BattleReward::Code,
        "Gn itemGroup missing trailing segment");

    parts = BuildSampleParts();
    parts[4] = "7300|||";
    failures += ExpectError(
        BattleReward::Parse(BuildPlain(JoinComma(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        BattleReward::Code,
        "Gn item missing name segment");

    parts = BuildSampleParts();
    parts[0] = "x|1|10";
    failures += ExpectError(
        BattleReward::Parse(BuildPlain(JoinComma(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        BattleReward::Code,
        "Gn invalid actor decimal");

    parts = BuildSampleParts();
    parts[0] = "-2|1|_";
    failures += ExpectError(
        BattleReward::Parse(BuildPlain(JoinComma(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        BattleReward::Code,
        "Gn invalid actor i62");

    parts = BuildSampleParts();
    parts[4] = "x\\zName|||";
    failures += ExpectError(
        BattleReward::Parse(BuildPlain(JoinComma(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        BattleReward::Code,
        "Gn invalid item image");

    parts = BuildSampleParts();
    parts[5] = "x|10";
    failures += ExpectError(
        BattleReward::Parse(BuildPlain(JoinComma(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        BattleReward::Code,
        "Gn invalid skill decimal");

    parts = BuildSampleParts();
    parts[5] = "1|_";
    failures += ExpectError(
        BattleReward::Parse(BuildPlain(JoinComma(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        BattleReward::Code,
        "Gn invalid skill i62");
    return failures;
}

int TestBuildErrors()
{
    BattleReward reward;
    reward.items[0].hasItem = true;
    reward.items[0].imageNumber = 7300;
    reward.items[0].hasNameRaw = true;
    reward.items[0].nameRaw = Bytes({0x81U});

    int failures = ExpectErrorAt(
        reward.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        0U,
        "Gn truncated GBK BuildData");
    failures += ExpectErrorAt(
        reward.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        0U,
        "Gn truncated GBK ToPacket");
    return failures;
}

int TestOuterErrors()
{
    int failures = 0;
    const std::string payload = JoinComma(BuildSampleParts());
    failures += ExpectError(
        BattleReward::Parse("LYIm BU "),
        cg::packet::PacketErrorCode::InvalidCode,
        "LYIm",
        "Gn wrong code");
    failures += ExpectError(
        BattleReward::Parse("Gn"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        BattleReward::Code,
        "Gn missing outer data");
    failures += ExpectError(
        BattleReward::Parse("Gn data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        BattleReward::Code,
        "Gn extra outer data");

    cg::packet::Packet manual(
        cg::packet::PacketCode(BattleReward::Code),
        cg::packet::PacketDirection::ClientToServer);
    manual.fields.push_back(cg::packet::PacketField {
        "data",
        cg::packet::PacketFieldType::EscapedString,
        payload,
    });
    failures += ExpectError(
        BattleReward::FromPacket(manual),
        cg::packet::PacketErrorCode::DecodeFailed,
        BattleReward::Code,
        "Gn wrong direction");

    manual.direction = cg::packet::PacketDirection::ServerToClient;
    manual.fields[0].name = "payload";
    failures += ExpectError(
        BattleReward::FromPacket(manual),
        cg::packet::PacketErrorCode::DecodeFailed,
        BattleReward::Code,
        "Gn wrong field name");

    manual.fields[0].name = "data";
    manual.fields[0].type = cg::packet::PacketFieldType::Int62;
    failures += ExpectError(
        BattleReward::FromPacket(manual),
        cg::packet::PacketErrorCode::InvalidFieldType,
        BattleReward::Code,
        "Gn wrong field type");

    manual.fields[0].type = cg::packet::PacketFieldType::EscapedString;
    manual.fields[0].value = Bytes({0x81U});
    failures += ExpectErrorAt(
        BattleReward::FromPacket(manual),
        cg::packet::PacketErrorCode::DecodeFailed,
        0U,
        "Gn truncated GBK payload");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestFullRoundtrip();
    failures += TestEmptySlots();
    failures += TestPayloadErrors();
    failures += TestBuildErrors();
    failures += TestOuterErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
