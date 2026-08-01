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

template <typename T>
int ExpectErrorAt(
    const cg::packet::PacketResult<T>& result,
    cg::packet::PacketErrorCode code,
    std::size_t fieldIndex,
    const char* message)
{
    int failures = ExpectError(result, code, "rU", message);
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

std::string BuildPacketLogExplanation(std::string_view lineBreak)
{
    std::string value = "$1";
    value.append(Bytes({0xb7U, 0xc0U, 0xd3U, 0xf9U}));
    value.append(" +51 $2");
    value.append(Bytes({0xbbU, 0xe8U, 0xcbU, 0xafU}));
    value.append(" +20 ");
    value.append(Bytes({0xbeU, 0xc6U, 0xd7U, 0xedU}));
    value.append(" +20");
    value.append(lineBreak);
    value.append("$2");
    value.append(Bytes({0xd2U, 0xc5U, 0xcdU, 0xfcU}));
    value.append(" +20 $8");
    value.append(Bytes({0xc9U, 0xc1U, 0xb6U, 0xe3U}));
    value.append(" +10 $4");
    value.append(Bytes({0xb5U, 0xc8U, 0xbcU, 0xb6U}));
    value.append(" 11");
    value.append(lineBreak);
    value.append("$4");
    value.append(Bytes({0xc4U, 0xcdU, 0xbeU, 0xc3U}));
    value.append(" 0168/0241 $0");
    value.append(Bytes({0xd6U, 0xd6U, 0xc0U, 0xe0U}));
    value.push_back(' ');
    value.append(Bytes({0xc3U, 0xb1U, 0xd7U, 0xd3U}));
    for (std::size_t index = 0; index < 6U; ++index) {
        value.append(lineBreak);
    }
    value.append("$4");
    value.append(Bytes({
        0xc4U, 0xdcU, 0xb9U, 0xbbU, 0xcaU, 0xb5U, 0xcfU, 0xd6U,
        0xc5U, 0xe4U, 0xb4U, 0xf7U, 0xd5U, 0xdfU, 0xd6U, 0xaeU,
        0xd3U, 0xfbU, 0xcdU, 0xfbU, 0xb5U, 0xc4U, 0xc3U, 0xb1U,
        0xd7U, 0xd3U,
    }));
    value.push_back('.');
    value.append(lineBreak);
    value.append(lineBreak);
    return value;
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
    cg::packet::PacketFieldType dataFieldType,
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
    failures += Expect(packet.fields[0].type == dataFieldType, message);
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
        failures += ExpectItemDataPacket(
            parsed.Value(),
            payload,
            cg::packet::PacketFieldType::EscapedString,
            "parse rU outer packet");
    }
    failures += Expect(
        cg::packet::receive::ItemData::Fields[0].type == cg::packet::PacketFieldType::SubPayload,
        "rU data spec is SubPayload");
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
            "Potion|Small",
            2,
            "Line1\nLine2",
            1001,
            1,
            0,
            3,
            5,
            7,
            10001,
            9,
            12,
            "Memo,Raw",
            "rU single record");
    }
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "rU single BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "rU single BuildData value");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "rU single ToPacket");
    if (packet.HasValue()) {
        failures += ExpectItemDataPacket(
            packet.Value(),
            payload,
            cg::packet::PacketFieldType::SubPayload,
            "rU single ToPacket value");
        const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
        failures += Expect(static_cast<bool>(built), "build rU single packet");
        if (built.HasValue()) {
            failures += Expect(built.Value() == BuildPlainRu(payload), "roundtrip rU single packet");
        }
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
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "rU clear slot BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == "8|||||||||||||", "rU clear slot BuildData value");
    }
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
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "rU multi BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "rU multi BuildData value");
    }
    return failures;
}

int TestLegacyTextCanonical()
{
    const std::string gbkEast = Bytes({0x96U, 0x7cU});
    std::string legacy = gbkEast;
    legacy.append("\\zpipe\\ccomma\\yslash\\nline\\q");
    std::string semantic = gbkEast;
    semantic.append("|pipe,comma\\slash\nlineq");
    std::string canonicalText = gbkEast;
    canonicalText.append("\\zpipe\\ccomma\\yslash\\nlineq");
    const std::string payload = JoinParts({
        "1",
        legacy,
        "3",
        legacy,
        "1002",
        "1",
        "1",
        "4",
        "6",
        "8",
        "10002",
        "10",
        "13",
        legacy,
    });
    const std::string canonical = JoinParts({
        "1",
        canonicalText,
        "3",
        canonicalText,
        "1002",
        "1",
        "1",
        "4",
        "6",
        "8",
        "10002",
        "10",
        "13",
        canonicalText,
    });
    const cg::packet::PacketResult<cg::packet::receive::ItemData> parsed =
        cg::packet::receive::ItemData::Parse(BuildPlainRu(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse rU legacy text");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "rU legacy text count");
    if (parsed.Value().records.size() == 1U) {
        failures += Expect(parsed.Value().records[0].nameRaw == semantic, "rU semantic name");
        failures += Expect(parsed.Value().records[0].explanationRaw == semantic, "rU semantic explanation");
        failures += Expect(parsed.Value().records[0].memoRaw == semantic, "rU semantic memo");
    }
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "rU canonical legacy BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == canonical, "rU canonical legacy BuildData value");
    }
    return failures;
}

int TestEmptyPayload()
{
    const cg::packet::PacketResult<cg::packet::receive::ItemData> parsed =
        cg::packet::receive::ItemData::Parse(BuildPlainRu(""));

    int failures = Expect(static_cast<bool>(parsed), "parse rU empty payload");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.empty(), "rU empty payload records");
        const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
        failures += Expect(static_cast<bool>(data), "rU empty payload BuildData");
        if (data.HasValue()) {
            failures += Expect(data.Value().empty(), "rU empty payload BuildData value");
        }
    }
    return failures;
}

int TestPacketLogSample()
{
    const std::string name = Bytes({0xb1U, 0xb4U, 0xcbU, 0xb9U, 0xc9U, 0xf1U, 0xc3U, 0xb1U});
    const std::string explanation = BuildPacketLogExplanation("\\n");
    const std::string semanticExplanation = BuildPacketLogExplanation("\n");
    const std::string payload = JoinParts({
        "0",
        name,
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
            name,
            0,
            semanticExplanation,
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
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "rU packet log BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "rU packet log BuildData value");
    }
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
    failures += ExpectErrorAt(
        cg::packet::receive::ItemData::Parse(BuildPlainRu(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "rU empty name with non-empty fields");

    parts = BuildItemParts();
    parts[1] = "Name\\";
    failures += ExpectErrorAt(
        cg::packet::receive::ItemData::Parse(BuildPlainRu(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "rU truncated name escape");

    parts = BuildItemParts();
    parts[3] = "Explanation\\";
    failures += ExpectErrorAt(
        cg::packet::receive::ItemData::Parse(BuildPlainRu(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        3U,
        "rU truncated explanation escape");

    parts = BuildItemParts();
    parts[13] = "Memo\\";
    failures += ExpectErrorAt(
        cg::packet::receive::ItemData::Parse(BuildPlainRu(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        13U,
        "rU truncated memo escape");

    parts = BuildItemParts();
    parts[1] = "Name";
    parts[3] = "Explanation";
    parts[13] = "Memo";
    std::string truncatedGbk = "rU ";
    truncatedGbk.append(JoinParts(parts));
    truncatedGbk.push_back(static_cast<char>(0x81));
    failures += ExpectErrorAt(
        cg::packet::receive::ItemData::Parse(truncatedGbk),
        cg::packet::PacketErrorCode::DecodeFailed,
        0U,
        "rU truncated GBK payload");

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

int TestBuildAndToPacketErrors()
{
    const cg::packet::receive::ItemRecord valid {
        0, "Name", 2, "Explanation", 1001, 1, 0, 3, 5, 7, 10001, 9, 12, "Memo", false,
    };
    const std::string truncatedGbk(1U, static_cast<char>(0x81));
    cg::packet::receive::ItemRecord invalidName = valid;
    invalidName.nameRaw = truncatedGbk;
    cg::packet::receive::ItemRecord invalidExplanation = valid;
    invalidExplanation.explanationRaw = truncatedGbk;
    cg::packet::receive::ItemRecord invalidMemo = valid;
    invalidMemo.memoRaw = truncatedGbk;

    int failures = 0;
    failures += ExpectErrorAt(
        cg::packet::receive::ItemData({invalidName}).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        1U,
        "rU BuildData name encode context");
    failures += ExpectErrorAt(
        cg::packet::receive::ItemData({invalidExplanation}).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        3U,
        "rU BuildData explanation encode context");
    const cg::packet::receive::ItemData memoData({invalidMemo});
    failures += ExpectErrorAt(
        memoData.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        13U,
        "rU BuildData memo encode context");
    failures += ExpectErrorAt(
        memoData.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        13U,
        "rU ToPacket memo encode context");
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
    failures += TestLegacyTextCanonical();
    failures += TestEmptyPayload();
    failures += TestPacketLogSample();
    failures += TestErrors();
    failures += TestBuildAndToPacketErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
