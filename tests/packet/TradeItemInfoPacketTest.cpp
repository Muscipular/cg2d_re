#include "packet/Packet.hpp"

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
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::TradeItemInfo::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::TradeItemInfo::Spec);
}

using TradeItemInfo = cg::packet::receive::TradeItemInfo;
using TradeItemInfoRecord = cg::packet::receive::TradeItemInfoRecord;

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

cg::packet::PacketField Field(
    std::string_view name,
    cg::packet::PacketFieldType type,
    std::string value)
{
    cg::packet::PacketField field;
    field.name = std::string(name);
    field.type = type;
    field.value = std::move(value);
    return field;
}

cg::packet::Packet PacketOf(
    cg::packet::PacketDirection direction,
    std::initializer_list<cg::packet::PacketField> fields)
{
    cg::packet::Packet packet(
        cg::packet::PacketCode(TradeItemInfo::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::string BuildRawPlain(std::string_view payload)
{
    std::string plain = "nUbJ ";
    plain.append(payload);
    plain.push_back(' ');
    return plain;
}

std::string BuildPlain(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return BuildRawPlain("");
    }
    return BuildRawPlain(encoded.Value());
}

std::vector<std::string> BuildRecordParts(
    std::string tradeWindowIndex,
    std::string nameRaw)
{
    return {
        std::move(tradeWindowIndex),
        std::move(nameRaw),
        "-2",
        "Line1\\nLine2",
        "1001",
        "1",
        "0",
        "3",
        "5",
        "7",
        "10001",
        "9",
        "12",
        "Memo\\cRaw",
    };
}

std::string JoinParts(const std::vector<std::string>& parts, bool trailingDelimiter)
{
    std::string payload;
    for (std::size_t index = 0; index < parts.size(); ++index) {
        if (index != 0U) {
            payload.push_back('|');
        }
        payload.append(parts[index]);
    }
    if (trailingDelimiter) {
        payload.push_back('|');
    }
    return payload;
}

std::string BuildPayload(std::size_t recordCount, bool trailingDelimiter)
{
    std::vector<std::string> parts;
    parts.reserve(recordCount * TradeItemInfo::RecordFieldCount);
    for (std::size_t index = 0; index < recordCount; ++index) {
        std::vector<std::string> record = BuildRecordParts(
            std::to_string(index),
            index == 0U ? "Potion\\zSmall" : "Ether");
        parts.insert(parts.end(), record.begin(), record.end());
    }
    return JoinParts(parts, trailingDelimiter);
}

int ExpectTradeItemInfoPacket(
    const cg::packet::Packet& packet,
    std::string_view payload,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(TradeItemInfo::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[0].value == payload, message);
    return failures;
}

int ExpectRecord(
    const TradeItemInfoRecord& record,
    int tradeWindowIndex,
    std::string_view nameRaw,
    const char* message)
{
    int failures = 0;
    failures += Expect(record.tradeWindowIndex == tradeWindowIndex, message);
    failures += Expect(record.nameRaw == nameRaw, message);
    failures += Expect(record.color == -2, message);
    failures += Expect(record.descriptionRaw == "Line1\nLine2", message);
    failures += Expect(record.imageNo == 1001, message);
    failures += Expect(record.canUseInField == 1, message);
    failures += Expect(record.battleUseFlag == 0, message);
    failures += Expect(record.target == 3, message);
    failures += Expect(record.level == 5, message);
    failures += Expect(record.flag == 7, message);
    failures += Expect(record.itemId == 10001, message);
    failures += Expect(record.itemType == 9, message);
    failures += Expect(record.count == 12, message);
    failures += Expect(record.memoRaw == "Memo,Raw", message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = BuildPayload(1U, true);
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlain(payload));

    int failures = Expect(TradeItemInfo::Spec.fieldCount == 1U, "nUbJ Spec exact1");
    failures += Expect(
        TradeItemInfo::Fields[0].type == cg::packet::PacketFieldType::SubPayload,
        "nUbJ Spec data SubPayload");
    failures += Expect(static_cast<bool>(parsed), "parse nUbJ outer");
    if (parsed.HasValue()) {
        failures += ExpectTradeItemInfoPacket(parsed.Value(), payload, "parse nUbJ packet");
    }
    return failures;
}

int TestSingleRecordRoundtrip()
{
    const std::string payload = BuildPayload(1U, true);
    const cg::packet::PacketResult<TradeItemInfo> parsed = TradeItemInfo::Parse(BuildPlain(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse nUbJ single record");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "nUbJ single count");
    if (!parsed.Value().records.empty()) {
        failures += ExpectRecord(parsed.Value().records[0], 0, "Potion|Small", "nUbJ single record");
    }

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "nUbJ single BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "nUbJ single canonical payload");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "nUbJ ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    failures += Expect(packet.Value().fields.size() == 1U, "nUbJ ToPacket exact1");
    if (packet.Value().fields.size() == 1U) {
        failures += Expect(
            packet.Value().fields[0].type == cg::packet::PacketFieldType::SubPayload,
            "nUbJ ToPacket data SubPayload");
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build nUbJ packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlain(payload), "nUbJ packet roundtrip");
    }
    return failures;
}

int TestTwoRecordsWithoutTrailingDelimiter()
{
    const std::string noTailPayload = BuildPayload(2U, false);
    const std::string canonicalPayload = BuildPayload(2U, true);
    const cg::packet::PacketResult<TradeItemInfo> parsed = TradeItemInfo::Parse(BuildPlain(noTailPayload));

    int failures = Expect(static_cast<bool>(parsed), "parse nUbJ no trailing delimiter");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 2U, "nUbJ two record count");
    if (parsed.Value().records.size() == 2U) {
        failures += ExpectRecord(parsed.Value().records[0], 0, "Potion|Small", "nUbJ first record");
        failures += ExpectRecord(parsed.Value().records[1], 1, "Ether", "nUbJ second record");
    }

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "nUbJ no tail BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == canonicalPayload, "nUbJ no tail canonical output");
    }
    return failures;
}

int TestLegacyTextRoundtrip()
{
    std::string semanticName;
    semanticName.push_back(static_cast<char>(0x81U));
    semanticName.push_back(static_cast<char>(0x7CU));
    semanticName.append("Name|Comma,Path\\Line\nEnd");

    std::string semanticDescription;
    semanticDescription.push_back(static_cast<char>(0x81U));
    semanticDescription.push_back(static_cast<char>(0x5CU));
    semanticDescription.append("Description|Comma,Path\\Line\nEnd");

    const std::string semanticMemo = "Memo|Comma,Path\\Line\nEnd";

    std::string encodedName;
    encodedName.push_back(static_cast<char>(0x81U));
    encodedName.push_back(static_cast<char>(0x7CU));
    encodedName.append("Name\\zComma\\cPath\\yLine\\nEnd");

    std::string encodedDescription;
    encodedDescription.push_back(static_cast<char>(0x81U));
    encodedDescription.push_back(static_cast<char>(0x5CU));
    encodedDescription.append("Description\\zComma\\cPath\\yLine\\nEnd");

    const std::string encodedMemo = "Memo\\zComma\\cPath\\yLine\\nEnd";
    std::vector<std::string> parts = BuildRecordParts("0", encodedName);
    parts[3] = encodedDescription;
    parts[13] = encodedMemo;
    const std::string payload = JoinParts(parts, true);

    const TradeItemInfo info({TradeItemInfoRecord {
        0,
        semanticName,
        -2,
        semanticDescription,
        1001,
        1,
        0,
        3,
        5,
        7,
        10001,
        9,
        12,
        semanticMemo,
    }});
    const cg::packet::PacketResult<std::string> data = info.BuildData();

    int failures = Expect(static_cast<bool>(data), "nUbJ semantic BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "nUbJ semantic canonical payload");
    }

    const cg::packet::PacketResult<TradeItemInfo> parsed = TradeItemInfo::Parse(BuildPlain(payload));
    failures += Expect(static_cast<bool>(parsed), "parse nUbJ semantic text");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "nUbJ semantic record count");
    if (!parsed.Value().records.empty()) {
        const TradeItemInfoRecord& record = parsed.Value().records[0];
        failures += Expect(record.nameRaw == semanticName, "nUbJ semantic name");
        failures += Expect(record.descriptionRaw == semanticDescription, "nUbJ semantic description");
        failures += Expect(record.memoRaw == semanticMemo, "nUbJ semantic memo");
    }
    return failures;
}

int TestEmptyMemoWithoutTrailingDelimiter()
{
    std::vector<std::string> parts = BuildRecordParts("0", "Potion");
    parts[13].clear();
    const std::string noTailPayload = JoinParts(parts, false);
    const std::string canonicalPayload = JoinParts(parts, true);
    const cg::packet::PacketResult<TradeItemInfo> parsed = TradeItemInfo::Parse(BuildPlain(noTailPayload));

    int failures = Expect(static_cast<bool>(parsed), "parse nUbJ no tail empty memo");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "nUbJ empty memo record count");
    if (!parsed.Value().records.empty()) {
        failures += Expect(parsed.Value().records[0].memoRaw.empty(), "nUbJ empty memo raw");
    }

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "nUbJ empty memo BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == canonicalPayload, "nUbJ empty memo canonical output");
    }
    return failures;
}

int TestEmptyAndTwentyOneRecords()
{
    const cg::packet::PacketResult<TradeItemInfo> empty = TradeItemInfo::Parse("nUbJ  ");
    int failures = Expect(static_cast<bool>(empty), "parse nUbJ empty payload");
    if (empty.HasValue()) {
        failures += Expect(empty.Value().records.empty(), "nUbJ empty record list");
        const cg::packet::PacketResult<std::string> emptyData = empty.Value().BuildData();
        failures += Expect(static_cast<bool>(emptyData), "nUbJ empty BuildData");
        if (emptyData.HasValue()) {
            failures += Expect(emptyData.Value().empty(), "nUbJ empty canonical payload");
        }
    }

    std::vector<TradeItemInfoRecord> records;
    records.reserve(21U);
    for (std::size_t index = 0; index < 21U; ++index) {
        records.push_back(TradeItemInfoRecord {0, "Potion"});
    }

    const TradeItemInfo info(std::move(records));
    const cg::packet::PacketResult<std::string> data = info.BuildData();
    failures += Expect(static_cast<bool>(data), "nUbJ 21 records BuildData");
    if (!data.HasValue()) {
        return failures;
    }
    failures += Expect(!data.Value().empty() && data.Value().back() == '|', "nUbJ 21 records canonical tail");

    const cg::packet::PacketResult<TradeItemInfo> parsed = TradeItemInfo::Parse(BuildPlain(data.Value()));
    failures += Expect(static_cast<bool>(parsed), "parse nUbJ 21 records");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.size() == 21U, "nUbJ 21 record count");
        for (const TradeItemInfoRecord& record : parsed.Value().records) {
            failures += Expect(record.tradeWindowIndex == 0, "nUbJ repeated slot preserved");
        }
    }
    return failures;
}

int TestPayloadErrors()
{
    int failures = 0;

    std::vector<std::string> parts = BuildRecordParts("0", "Potion");
    parts.pop_back();
    failures += ExpectError(
        TradeItemInfo::Parse(BuildPlain(JoinParts(parts, false))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nUbJ",
        "nUbJ 13 fields");

    parts = BuildRecordParts("0", "Potion");
    parts.push_back("extra");
    failures += ExpectError(
        TradeItemInfo::Parse(BuildPlain(JoinParts(parts, false))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nUbJ",
        "nUbJ 15 fields");

    failures += ExpectError(
        TradeItemInfo::Parse(BuildPlain(BuildPayload(1U, true) + "|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nUbJ",
        "nUbJ extra trailing empty segment");

    parts = BuildRecordParts("x", "Potion");
    failures += ExpectError(
        TradeItemInfo::Parse(BuildPlain(JoinParts(parts, true))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "nUbJ",
        "nUbJ bad decimal");

    parts = BuildRecordParts("0", "Potion");
    parts[2].clear();
    failures += ExpectError(
        TradeItemInfo::Parse(BuildPlain(JoinParts(parts, true))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "nUbJ",
        "nUbJ empty decimal");

    parts = BuildRecordParts("0", "Potion");
    parts[12] = "2147483648";
    failures += ExpectError(
        TradeItemInfo::Parse(BuildPlain(JoinParts(parts, true))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "nUbJ",
        "nUbJ decimal overflow");

    parts = BuildRecordParts("0", "Potion");
    parts[13].assign(1U, static_cast<char>(0x81U));
    failures += ExpectError(
        TradeItemInfo::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field(
                "data",
                cg::packet::PacketFieldType::EscapedString,
                JoinParts(parts, false))})),
        cg::packet::PacketErrorCode::DecodeFailed,
        "nUbJ",
        "nUbJ truncated GBK payload");

    return failures;
}

int TestPacketErrors()
{
    int failures = 0;
    failures += ExpectError(
        TradeItemInfo::Parse("ms Alice a "),
        cg::packet::PacketErrorCode::InvalidCode,
        "ms",
        "nUbJ wrong code");
    failures += ExpectError(
        TradeItemInfo::Parse("nUbJ"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nUbJ",
        "nUbJ missing outer data");
    failures += ExpectError(
        TradeItemInfo::Parse("nUbJ data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "nUbJ",
        "nUbJ extra outer token");
    failures += ExpectError(
        TradeItemInfo::FromPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {Field("data", cg::packet::PacketFieldType::EscapedString, BuildPayload(1U, true))})),
        cg::packet::PacketErrorCode::DecodeFailed,
        "nUbJ",
        "nUbJ manual wrong direction");
    failures += ExpectError(
        TradeItemInfo::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("item", cg::packet::PacketFieldType::EscapedString, BuildPayload(1U, true))})),
        cg::packet::PacketErrorCode::DecodeFailed,
        "nUbJ",
        "nUbJ manual wrong field name");
    failures += ExpectError(
        TradeItemInfo::FromPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("data", cg::packet::PacketFieldType::Unknown, BuildPayload(1U, true))})),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "nUbJ",
        "nUbJ manual wrong field type");
    return failures;
}

int TestTextEncodingErrors()
{
    const std::string truncatedGbk(1U, static_cast<char>(0x81U));
    int failures = 0;

    TradeItemInfoRecord badName {0, truncatedGbk};
    const cg::packet::PacketResult<std::string> badNameResult =
        TradeItemInfo({badName}).BuildData();
    failures += ExpectError(
        badNameResult,
        cg::packet::PacketErrorCode::EncodeFailed,
        "nUbJ",
        "nUbJ build truncated name");
    if (badNameResult.HasError()) {
        failures += Expect(
            badNameResult.Error().fieldIndex.has_value() && badNameResult.Error().fieldIndex.value() == 1U,
            "nUbJ truncated name field index");
    }

    TradeItemInfoRecord badDescription {0, "Potion"};
    badDescription.descriptionRaw = truncatedGbk;
    const cg::packet::PacketResult<std::string> badDescriptionResult =
        TradeItemInfo({badDescription}).BuildData();
    failures += ExpectError(
        badDescriptionResult,
        cg::packet::PacketErrorCode::EncodeFailed,
        "nUbJ",
        "nUbJ build truncated description");
    if (badDescriptionResult.HasError()) {
        failures += Expect(
            badDescriptionResult.Error().fieldIndex.has_value() &&
                badDescriptionResult.Error().fieldIndex.value() == 3U,
            "nUbJ truncated description field index");
    }

    TradeItemInfoRecord badMemo {0, "Potion"};
    badMemo.memoRaw = truncatedGbk;
    const cg::packet::PacketResult<std::string> badMemoResult =
        TradeItemInfo({badMemo}).BuildData();
    failures += ExpectError(
        badMemoResult,
        cg::packet::PacketErrorCode::EncodeFailed,
        "nUbJ",
        "nUbJ build truncated memo");
    if (badMemoResult.HasError()) {
        failures += Expect(
            badMemoResult.Error().fieldIndex.has_value() && badMemoResult.Error().fieldIndex.value() == 13U,
            "nUbJ truncated memo field index");
    }
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestSingleRecordRoundtrip();
    failures += TestTwoRecordsWithoutTrailingDelimiter();
    failures += TestLegacyTextRoundtrip();
    failures += TestEmptyMemoWithoutTrailingDelimiter();
    failures += TestEmptyAndTwentyOneRecords();
    failures += TestPayloadErrors();
    failures += TestPacketErrors();
    failures += TestTextEncodingErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
