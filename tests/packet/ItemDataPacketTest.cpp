#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::ItemData::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::ItemData::Spec);
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

std::string BuildPlainRu(std::string_view payload)
{
    return BuildPlain(cg::packet::receive::ItemData::Code, payload);
}

std::string JoinParts(const std::vector<std::string>& parts)
{
    std::string payload;
    for (std::size_t index = 0; index < parts.size(); ++index) {
        if (index != 0U) {
            payload.push_back('|');
        }
        payload.append(parts[index]);
    }
    return payload;
}

std::vector<std::string> BuildItemParts()
{
    return {
        "0",
        "Potion\\zSmall",
        "2",
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

std::vector<std::string> BuildClearParts(int slot)
{
    std::vector<std::string> parts(14U);
    parts[0] = std::to_string(slot);
    return parts;
}

int ExpectItemDataPacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::ItemData::Code), message);
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
    const cg::packet::receive::ItemRecord& record,
    int slot,
    std::string_view nameRaw,
    int nameColor,
    std::string_view explanationRaw,
    int imageId,
    int ableUseField,
    int battleUseFlag,
    int target,
    int level,
    int flag,
    int itemId,
    int type,
    int remain,
    std::string_view memoRaw,
    const char* message)
{
    int failures = 0;
    failures += Expect(!record.clearSlot, message);
    failures += Expect(record.slot == slot, message);
    failures += Expect(record.nameRaw == nameRaw, message);
    failures += Expect(record.nameColor == nameColor, message);
    failures += Expect(record.explanationRaw == explanationRaw, message);
    failures += Expect(record.imageId == imageId, message);
    failures += Expect(record.ableUseField == ableUseField, message);
    failures += Expect(record.battleUseFlag == battleUseFlag, message);
    failures += Expect(record.target == target, message);
    failures += Expect(record.level == level, message);
    failures += Expect(record.flag == flag, message);
    failures += Expect(record.itemId == itemId, message);
    failures += Expect(record.type == type, message);
    failures += Expect(record.remain == remain, message);
    failures += Expect(record.memoRaw == memoRaw, message);
    return failures;
}

int ExpectClearRecord(const cg::packet::receive::ItemRecord& record, int slot, const char* message)
{
    int failures = 0;
    failures += Expect(record.clearSlot, message);
    failures += Expect(record.slot == slot, message);
    failures += Expect(record.nameRaw.empty(), message);
    failures += Expect(record.explanationRaw.empty(), message);
    failures += Expect(record.memoRaw.empty(), message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = JoinParts(BuildItemParts());
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlainRu(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse rU outer");
    if (parsed.HasValue()) {
        failures += ExpectItemDataPacket(parsed.Value(), payload, "parse rU outer packet");
    }
    return failures;
}

int TestSingleItemRoundtrip()
{
    const std::string payload = JoinParts(BuildItemParts());
    const cg::packet::PacketResult<cg::packet::receive::ItemData> parsed =
        cg::packet::receive::ItemData::Parse(BuildPlainRu(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse rU single item");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "rU single record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(
            parsed.Value().records[0],
            0,
            "Potion\\zSmall",
            2,
            "Line1\\nLine2",
            1001,
            1,
            0,
            3,
            5,
            7,
            10001,
            9,
            12,
            "Memo\\cRaw",
            "rU single record");
    }
    failures += Expect(parsed.Value().BuildData() == payload, "rU single BuildData");
    failures += ExpectItemDataPacket(parsed.Value().ToPacket(), payload, "rU single ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build rU single packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainRu(payload), "roundtrip rU single packet");
    }
    return failures;
}

int TestClearSlot()
{
    const std::string payload = JoinParts(BuildClearParts(8));
    const cg::packet::PacketResult<cg::packet::receive::ItemData> parsed =
        cg::packet::receive::ItemData::Parse(BuildPlainRu(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse rU clear slot");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "rU clear slot count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectClearRecord(parsed.Value().records[0], 8, "rU clear slot record");
    }
    failures += Expect(parsed.Value().BuildData() == "8|||||||||||||", "rU clear slot BuildData");
    return failures;
}

int TestMultiRecordRoundtrip()
{
    std::vector<std::string> parts = BuildItemParts();
    const std::vector<std::string> clearParts = BuildClearParts(8);
    parts.insert(parts.end(), clearParts.begin(), clearParts.end());
    const std::string payload = JoinParts(parts);

    const cg::packet::PacketResult<cg::packet::receive::ItemData> parsed =
        cg::packet::receive::ItemData::Parse(BuildPlainRu(payload));
    int failures = Expect(static_cast<bool>(parsed), "parse rU multi record");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 2U, "rU multi record count");
    if (parsed.Value().records.size() == 2U) {
        failures += ExpectClearRecord(parsed.Value().records[1], 8, "rU multi clear record");
    }
    failures += Expect(parsed.Value().BuildData() == payload, "rU multi BuildData");
    return failures;
}

int TestRawInnerEscapesPreserved()
{
    const std::string raw = "A\\zB\\cC\\yD\\nE";
    const std::string payload = JoinParts({
        "1",
        raw,
        "3",
        raw,
        "1002",
        "1",
        "1",
        "4",
        "6",
        "8",
        "10002",
        "10",
        "13",
        raw,
    });
    const cg::packet::PacketResult<cg::packet::receive::ItemData> parsed =
        cg::packet::receive::ItemData::Parse(BuildPlainRu(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse rU raw inner escapes");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "rU raw inner count");
    if (parsed.Value().records.size() == 1U) {
        failures += Expect(parsed.Value().records[0].nameRaw == raw, "rU raw name");
        failures += Expect(parsed.Value().records[0].explanationRaw == raw, "rU raw explanation");
        failures += Expect(parsed.Value().records[0].memoRaw == raw, "rU raw memo");
    }
    failures += Expect(parsed.Value().BuildData() == payload, "rU raw inner BuildData");
    return failures;
}

int TestEmptyPayload()
{
    const cg::packet::PacketResult<cg::packet::receive::ItemData> parsed =
        cg::packet::receive::ItemData::Parse(BuildPlainRu(""));

    int failures = Expect(static_cast<bool>(parsed), "parse rU empty payload");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.empty(), "rU empty payload records");
        failures += Expect(parsed.Value().BuildData().empty(), "rU empty payload BuildData");
    }
    return failures;
}

int TestPacketLogSample()
{
    const std::string explanation =
        R"($1防御\S+51\S$2昏睡\S+20\S酒醉\S+20\\n$2遗忘\S+20\S$8闪躲\S+10\S$4等级\S11\\n$4耐久\S0168/0241\S$0种类\S帽子\\n\\n\\n\\n\\n\\n$4能够实现配戴者之欲望的帽子.\\n\\n)";
    const std::string payload = JoinParts({
        "0",
        "贝斯神帽",
        "0",
        explanation,
        "21511",
        "1",
        "2",
        "0",
        "11",
        "153",
        "601615",
        "9",
        "0",
        "",
    });
    const cg::packet::PacketResult<cg::packet::receive::ItemData> parsed =
        cg::packet::receive::ItemData::Parse(BuildPlainRu(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse rU packet log sample");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "rU packet log record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(
            parsed.Value().records[0],
            0,
            "贝斯神帽",
            0,
            explanation,
            21511,
            1,
            2,
            0,
            11,
            153,
            601615,
            9,
            0,
            "",
            "rU packet log record");
    }
    failures += Expect(parsed.Value().BuildData() == payload, "rU packet log BuildData");
    return failures;
}

int TestErrors()
{
    int failures = 0;

    std::vector<std::string> parts = BuildItemParts();
    parts.pop_back();
    failures += ExpectError(
        cg::packet::receive::ItemData::Parse(BuildPlainRu(JoinParts(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "rU",
        "rU thirteen fields");

    parts = BuildItemParts();
    parts.push_back("extra");
    failures += ExpectError(
        cg::packet::receive::ItemData::Parse(BuildPlainRu(JoinParts(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "rU",
        "rU fifteen fields");

    parts = BuildItemParts();
    parts[0] = "x";
    failures += ExpectError(
        cg::packet::receive::ItemData::Parse(BuildPlainRu(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "rU",
        "rU bad slot decimal");

    parts = BuildItemParts();
    parts[2] = "x";
    failures += ExpectError(
        cg::packet::receive::ItemData::Parse(BuildPlainRu(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "rU",
        "rU bad numeric decimal");

    parts = BuildItemParts();
    parts[2].clear();
    failures += ExpectError(
        cg::packet::receive::ItemData::Parse(BuildPlainRu(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "rU",
        "rU empty numeric decimal");

    parts = BuildItemParts();
    parts[4] = "2147483648";
    failures += ExpectError(
        cg::packet::receive::ItemData::Parse(BuildPlainRu(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "rU",
        "rU numeric overflow");

    parts = BuildItemParts();
    parts[1].clear();
    failures += ExpectError(
        cg::packet::receive::ItemData::Parse(BuildPlainRu(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "rU",
        "rU empty name with non-empty fields");

    failures += ExpectError(
        cg::packet::receive::ItemData::Parse("rU"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "rU",
        "rU missing outer data");

    failures += ExpectError(
        cg::packet::receive::ItemData::Parse("rU data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "rU",
        "rU extra outer data");

    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestSingleItemRoundtrip();
    failures += TestClearSlot();
    failures += TestMultiRecordRoundtrip();
    failures += TestRawInnerEscapesPreserved();
    failures += TestEmptyPayload();
    failures += TestPacketLogSample();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
