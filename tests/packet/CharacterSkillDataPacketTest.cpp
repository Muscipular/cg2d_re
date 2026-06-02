#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

const cg::packet::PacketSpec& PacketSpec() noexcept
{
    return cg::packet::receive::CharacterSkillData::Spec;
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

std::string BuildPlain(std::string_view code, std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedPayload = cg::packet::EncodeEscapedString(payload);
    if (!encodedPayload.HasValue()) {
        std::string plain(code);
        plain.append("  ");
        return plain;
    }

    std::string plain(code);
    plain.push_back(' ');
    plain.append(encodedPayload.Value());
    plain.push_back(' ');
    return plain;
}

std::string BuildPlainCne(std::string_view payload)
{
    return BuildPlain(cg::packet::receive::CharacterSkillData::Code, payload);
}

int ExpectCharacterSkillDataPacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::CharacterSkillData::Code), message);
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

int ExpectRecord(
    const cg::packet::receive::CharacterSkillRecord& record,
    int skillSlot,
    int skillId,
    std::string_view name,
    int level,
    int exp,
    int maxLevel,
    int nextExp,
    int requiredSlots,
    int operation,
    int consumptionRate,
    int seqNo,
    const char* message)
{
    int failures = 0;
    failures += Expect(!record.emptySlot, message);
    failures += Expect(record.skillSlot == skillSlot, message);
    failures += Expect(record.skillId == skillId, message);
    failures += Expect(record.name == name, message);
    failures += Expect(record.level == level, message);
    failures += Expect(record.exp == exp, message);
    failures += Expect(record.maxLevel == maxLevel, message);
    failures += Expect(record.nextExp == nextExp, message);
    failures += Expect(record.requiredSlots == requiredSlots, message);
    failures += Expect(record.operation == operation, message);
    failures += Expect(record.consumptionRate == consumptionRate, message);
    failures += Expect(record.seqNo == seqNo, message);
    return failures;
}

int ExpectEmptyRecord(
    const cg::packet::receive::CharacterSkillRecord& record,
    int skillSlot,
    const char* message)
{
    int failures = 0;
    failures += Expect(record.emptySlot, message);
    failures += Expect(record.skillSlot == skillSlot, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = "0|100|Fire|1|25|10|100|1|2|3|4|";
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlainCne(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse cNE outer");
    if (parsed.HasValue()) {
        failures += ExpectCharacterSkillDataPacket(parsed.Value(), payload, "parse cNE outer packet");
    }
    return failures;
}

int TestSingleRecordRoundtrip()
{
    const std::string payload = "0|100|Fire|1|25|10|100|1|2|3|4|";
    const cg::packet::PacketResult<cg::packet::receive::CharacterSkillData> parsed =
        cg::packet::receive::CharacterSkillData::Parse(BuildPlainCne(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse cNE single record");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "cNE single record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(parsed.Value().records[0], 0, 100, "Fire", 1, 25, 10, 100, 1, 2, 3, 4, "cNE record");
    }
    failures += Expect(parsed.Value().BuildData() == payload, "cNE single BuildData");

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build cNE single packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainCne(payload), "roundtrip cNE single packet");
    }
    return failures;
}

int TestMultiRecordRoundtrip()
{
    std::vector<cg::packet::receive::CharacterSkillRecord> records;
    records.push_back(cg::packet::receive::CharacterSkillRecord {0, 100, "Fire", 1, 25, 10, 100, 1, 2, 3, 4, false});
    records.push_back(cg::packet::receive::CharacterSkillRecord {1, 101, "Heal", 2, 50, 10, 200, 2, 3, 4, 5, false});
    const cg::packet::receive::CharacterSkillData skillData(std::move(records));
    const std::string payload = "0|100|Fire|1|25|10|100|1|2|3|4|1|101|Heal|2|50|10|200|2|3|4|5|";

    int failures = ExpectCharacterSkillDataPacket(skillData.ToPacket(), payload, "cNE multi ToPacket");
    const cg::packet::PacketResult<std::string> built = BuildPacket(skillData.ToPacket());
    failures += Expect(static_cast<bool>(built), "build cNE multi packet");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::receive::CharacterSkillData> parsed =
        cg::packet::receive::CharacterSkillData::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip cNE multi packet");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.size() == 2U, "roundtrip cNE multi count");
        failures += Expect(parsed.Value().BuildData() == payload, "roundtrip cNE multi payload");
    }
    return failures;
}

int TestEmptySlot()
{
    const std::string payload = "0|||||||||||";
    const cg::packet::PacketResult<cg::packet::receive::CharacterSkillData> parsed =
        cg::packet::receive::CharacterSkillData::Parse(BuildPlainCne(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse cNE empty slot");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "cNE empty slot count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectEmptyRecord(parsed.Value().records[0], 0, "cNE empty slot record");
    }
    failures += Expect(parsed.Value().BuildData() == payload, "cNE empty slot BuildData");
    return failures;
}

int TestEmptyPayload()
{
    const cg::packet::PacketResult<cg::packet::receive::CharacterSkillData> parsed =
        cg::packet::receive::CharacterSkillData::Parse(BuildPlainCne(""));

    int failures = Expect(static_cast<bool>(parsed), "parse cNE empty payload");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.empty(), "cNE empty payload records");
        failures += Expect(parsed.Value().BuildData().empty(), "cNE empty payload BuildData");
    }
    return failures;
}

int TestRawNamePreserved()
{
    const std::string payload = "0|100|A\\zB\\cC\\yD\\nE|1|25|10|100|1|2|3|4|";
    const cg::packet::PacketResult<cg::packet::receive::CharacterSkillData> parsed =
        cg::packet::receive::CharacterSkillData::Parse(BuildPlainCne(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse cNE raw name");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "cNE raw name count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(
            parsed.Value().records[0],
            0,
            100,
            "A\\zB\\cC\\yD\\nE",
            1,
            25,
            10,
            100,
            1,
            2,
            3,
            4,
            "cNE raw name record");
    }
    failures += Expect(parsed.Value().BuildData() == payload, "cNE raw name BuildData");
    return failures;
}

int TestNoTrailingDelimiter()
{
    const std::string payload = "0|100|Fire|1|25|10|100|1|2|3|4";
    const cg::packet::PacketResult<cg::packet::receive::CharacterSkillData> parsed =
        cg::packet::receive::CharacterSkillData::Parse(BuildPlainCne(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse cNE no trailing delimiter");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.size() == 1U, "cNE no trailing count");
        failures += Expect(parsed.Value().BuildData() == "0|100|Fire|1|25|10|100|1|2|3|4|", "cNE build adds trailing");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::CharacterSkillData::Parse(BuildPlainCne("0|100|Fire|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "cNE",
        "cNE payload field count");
    failures += ExpectError(
        cg::packet::receive::CharacterSkillData::Parse(BuildPlainCne("0||Fire|1|25|10|100|1|2|3|4|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "cNE",
        "cNE empty decimal");
    failures += ExpectError(
        cg::packet::receive::CharacterSkillData::Parse(BuildPlainCne("0|x|Fire|1|25|10|100|1|2|3|4|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "cNE",
        "cNE bad decimal");
    failures += ExpectError(
        cg::packet::receive::CharacterSkillData::Parse(BuildPlainCne("0|2147483648|Fire|1|25|10|100|1|2|3|4|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "cNE",
        "cNE decimal overflow");
    failures += ExpectError(
        cg::packet::receive::CharacterSkillData::Parse("cNE 0|100|Fire|1|25|10|100|1|2|3|4| extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "cNE",
        "cNE outer field count");
    failures += ExpectError(
        cg::packet::receive::CharacterSkillData::Parse(BuildPlain("Sc", "0|100|Fire|1|25|10|100|1|2|3|4|")),
        cg::packet::PacketErrorCode::InvalidCode,
        "Sc",
        "cNE wrong code");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestSingleRecordRoundtrip();
    failures += TestMultiRecordRoundtrip();
    failures += TestEmptySlot();
    failures += TestEmptyPayload();
    failures += TestRawNamePreserved();
    failures += TestNoTrailingDelimiter();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
