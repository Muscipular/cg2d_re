#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::CharacterSkillData::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::CharacterSkillData::Spec);
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
    int failures = ExpectError(result, code, "cNE", message);
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
    cg::packet::PacketFieldType dataFieldType,
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
    failures += Expect(packet.fields[0].type == dataFieldType, message);
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
        failures += ExpectCharacterSkillDataPacket(
            parsed.Value(),
            payload,
            cg::packet::PacketFieldType::EscapedString,
            "parse cNE outer packet");
    }
    failures += Expect(
        cg::packet::receive::CharacterSkillData::Fields[0].type == cg::packet::PacketFieldType::SubPayload,
        "cNE data spec is SubPayload");
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
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "cNE single BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "cNE single BuildData value");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "cNE single ToPacket");
    if (packet.HasValue()) {
        failures += ExpectCharacterSkillDataPacket(
            packet.Value(),
            payload,
            cg::packet::PacketFieldType::SubPayload,
            "cNE single ToPacket value");
        const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
        failures += Expect(static_cast<bool>(built), "build cNE single packet");
        if (built.HasValue()) {
            failures += Expect(built.Value() == BuildPlainCne(payload), "roundtrip cNE single packet");
        }
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

    const cg::packet::PacketResult<cg::packet::Packet> packet = skillData.ToPacket();
    int failures = Expect(static_cast<bool>(packet), "cNE multi ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }
    failures += ExpectCharacterSkillDataPacket(
        packet.Value(),
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "cNE multi ToPacket value");
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build cNE multi packet");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::receive::CharacterSkillData> parsed =
        cg::packet::receive::CharacterSkillData::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip cNE multi packet");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.size() == 2U, "roundtrip cNE multi count");
        const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
        failures += Expect(static_cast<bool>(data), "roundtrip cNE multi payload");
        if (data.HasValue()) {
            failures += Expect(data.Value() == payload, "roundtrip cNE multi payload value");
        }
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
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "cNE empty slot BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "cNE empty slot BuildData value");
    }

    const cg::packet::PacketResult<cg::packet::receive::CharacterSkillData> withoutTrailing =
        cg::packet::receive::CharacterSkillData::Parse(BuildPlainCne("0||||||||||"));
    failures += Expect(static_cast<bool>(withoutTrailing), "parse cNE empty slot without trailing delimiter");
    if (withoutTrailing.HasValue()) {
        const cg::packet::PacketResult<std::string> rebuilt = withoutTrailing.Value().BuildData();
        failures += Expect(static_cast<bool>(rebuilt), "build cNE empty slot canonical trailing delimiter");
        if (rebuilt.HasValue()) {
            failures += Expect(rebuilt.Value() == payload, "cNE empty slot canonical value");
        }
    }
    return failures;
}

int TestEmptyPayload()
{
    const cg::packet::PacketResult<cg::packet::receive::CharacterSkillData> parsed =
        cg::packet::receive::CharacterSkillData::Parse(BuildPlainCne(""));

    int failures = Expect(static_cast<bool>(parsed), "parse cNE empty payload");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.empty(), "cNE empty payload records");
        const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
        failures += Expect(static_cast<bool>(data), "cNE empty payload BuildData");
        if (data.HasValue()) {
            failures += Expect(data.Value().empty(), "cNE empty payload BuildData value");
        }
    }
    return failures;
}

int TestLegacyNameCanonical()
{
    const std::string gbkEast = Bytes({0x96U, 0x7cU});
    std::string legacyName = gbkEast;
    legacyName.append("\\zpipe\\ccomma\\yslash\\nline\\q");
    std::string semanticName = gbkEast;
    semanticName.append("|pipe,comma\\slash\nlineq");
    std::string canonicalName = gbkEast;
    canonicalName.append("\\zpipe\\ccomma\\yslash\\nlineq");

    std::string payload = "0|100|";
    payload.append(legacyName);
    payload.append("|1|25|10|100|1|2|3|4|");
    std::string canonical = "0|100|";
    canonical.append(canonicalName);
    canonical.append("|1|25|10|100|1|2|3|4|");
    const cg::packet::PacketResult<cg::packet::receive::CharacterSkillData> parsed =
        cg::packet::receive::CharacterSkillData::Parse(BuildPlainCne(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse cNE legacy name");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "cNE legacy name count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(
            parsed.Value().records[0],
            0,
            100,
            semanticName,
            1,
            25,
            10,
            100,
            1,
            2,
            3,
            4,
            "cNE legacy name record");
    }

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "cNE canonical legacy BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == canonical, "cNE canonical legacy BuildData value");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "cNE canonical legacy ToPacket");
    if (packet.HasValue()) {
        failures += ExpectCharacterSkillDataPacket(
            packet.Value(),
            canonical,
            cg::packet::PacketFieldType::SubPayload,
            "cNE canonical legacy ToPacket value");
    }
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
        const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
        failures += Expect(static_cast<bool>(data), "cNE build no trailing input");
        if (data.HasValue()) {
            failures += Expect(
                data.Value() == "0|100|Fire|1|25|10|100|1|2|3|4|",
                "cNE build adds trailing");
        }
    }
    return failures;
}

int TestPacketLogSample()
{
    const std::string firstName = Bytes({0xc2U, 0xd2U, 0xc9U, 0xe4U});
    const std::string lastName = Bytes({0xb6U, 0xbeU, 0xbbU, 0xf7U});
    std::string payload = "0|95|";
    payload.append(firstName);
    payload.append("|8|51000|13|68500|1|0|100|1|1|71|");
    payload.append(Bytes({0xb5U, 0xf7U, 0xbdU, 0xccU}));
    payload.append("|3|2000|3|5500|1|0|100|7|2|3|");
    payload.append(Bytes({0xc7U, 0xacU, 0xc0U, 0xa4U, 0xd2U, 0xbbU, 0xd6U, 0xc0U}));
    payload.append("|1|0|5|500|1|0|100|3|3|70|");
    payload.append(Bytes({0xb3U, 0xe8U, 0xceU, 0xefU, 0xc7U, 0xbfU, 0xbbU, 0xafU}));
    payload.append("|3|2000|3|5500|1|0|100|8|4|68|");
    payload.append(Bytes({0xc6U, 0xf8U, 0xbeU, 0xf8U, 0xbbU, 0xd8U, 0xb8U, 0xb4U}));
    payload.append("|1|0|3|500|1|0|100|5|5|12|");
    payload.append(Bytes({0xc3U, 0xf7U, 0xbeU, 0xb5U, 0xd6U, 0xb9U, 0xcbU, 0xaeU}));
    payload.append("|1|0|13|500|1|0|100|6|6|400|");
    payload.append(Bytes({0xb7U, 0xc9U, 0xb5U, 0xb6U, 0xc2U, 0xd2U, 0xceU, 0xe8U}));
    payload.append("|13|192500|13|254500|1|0|100|2|7|72|");
    payload.append(Bytes({0xc7U, 0xd4U, 0xb5U, 0xc1U}));
    payload.append("|1|0|13|500|1|0|100|4|8|1002|");
    payload.append(Bytes({0xd2U, 0xbbU, 0xbbU, 0xf7U, 0xb1U, 0xd8U, 0xd6U, 0xd0U}));
    payload.append("|7|40400|13|51000|1|0|100|9|9|1003|");
    payload.append(lastName);
    payload.append(
        "|7|37000|7|51000|1|0|100|10|"
        "10|||||||||||11|||||||||||12|||||||||||13|||||||||||14|||||||||||");
    const cg::packet::PacketResult<cg::packet::receive::CharacterSkillData> parsed =
        cg::packet::receive::CharacterSkillData::Parse(BuildPlainCne(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse cNE packet log sample");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 15U, "cNE packet log record count");
    if (parsed.Value().records.size() == 15U) {
        failures += ExpectRecord(
            parsed.Value().records[0],
            0,
            95,
            firstName,
            8,
            51000,
            13,
            68500,
            1,
            0,
            100,
            1,
            "cNE packet log first record");
        failures += ExpectRecord(
            parsed.Value().records[9],
            9,
            1003,
            lastName,
            7,
            37000,
            7,
            51000,
            1,
            0,
            100,
            10,
            "cNE packet log last skill record");
        failures += ExpectEmptyRecord(parsed.Value().records[10], 10, "cNE packet log empty slot 10");
        failures += ExpectEmptyRecord(parsed.Value().records[14], 14, "cNE packet log empty slot 14");
    }
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "cNE packet log BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "cNE packet log BuildData value");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectErrorAt(
        cg::packet::receive::CharacterSkillData::Parse(BuildPlainCne("0|100|Fire|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        0U,
        "cNE payload field count");
    failures += ExpectErrorAt(
        cg::packet::receive::CharacterSkillData::Parse(BuildPlainCne("0||Fire|1|25|10|100|1|2|3|4|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "cNE empty decimal");
    failures += ExpectErrorAt(
        cg::packet::receive::CharacterSkillData::Parse(BuildPlainCne("0||Name|||||||||")),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "cNE non-canonical empty slot");
    failures += ExpectErrorAt(
        cg::packet::receive::CharacterSkillData::Parse(BuildPlainCne("0|x|Fire|1|25|10|100|1|2|3|4|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "cNE bad decimal");
    failures += ExpectErrorAt(
        cg::packet::receive::CharacterSkillData::Parse(BuildPlainCne("0|2147483648|Fire|1|25|10|100|1|2|3|4|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
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
    failures += ExpectErrorAt(
        cg::packet::receive::CharacterSkillData::Parse(
            BuildPlainCne("0|100|Name\\|1|25|10|100|1|2|3|4|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        2U,
        "cNE truncated name escape");

    std::string truncatedGbk = "cNE 0|100|Name|1|25|10|100|1|2|3|4|";
    truncatedGbk.push_back(static_cast<char>(0x81));
    failures += ExpectErrorAt(
        cg::packet::receive::CharacterSkillData::Parse(truncatedGbk),
        cg::packet::PacketErrorCode::DecodeFailed,
        0U,
        "cNE truncated GBK payload");
    return failures;
}

int TestBuildAndToPacketErrors()
{
    std::string truncatedGbk(1U, static_cast<char>(0x81));
    const cg::packet::receive::CharacterSkillData skillData({
        {0, 100, "Valid", 1, 25, 10, 100, 1, 2, 3, 4, false},
        {1, 101, std::move(truncatedGbk), 2, 50, 10, 200, 2, 3, 4, 5, false},
    });

    int failures = 0;
    failures += ExpectErrorAt(
        skillData.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        13U,
        "cNE BuildData truncated GBK context");
    failures += ExpectErrorAt(
        skillData.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        13U,
        "cNE ToPacket truncated GBK context");
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
    failures += TestLegacyNameCanonical();
    failures += TestNoTrailingDelimiter();
    failures += TestPacketLogSample();
    failures += TestErrors();
    failures += TestBuildAndToPacketErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
