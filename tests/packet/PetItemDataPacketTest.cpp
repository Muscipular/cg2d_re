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
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::PetItemData::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::PetItemData::Spec);
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
    int failures = ExpectError(result, code, "iKe", message);
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

std::string BuildPlainIKe(int petSlot, std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedPetSlot = cg::packet::EncodeInt62(petSlot);
    const cg::packet::PacketResult<std::string> encodedPayload = cg::packet::EncodeEscapedString(payload);
    if (!encodedPetSlot.HasValue() || !encodedPayload.HasValue()) {
        return "iKe  ";
    }

    std::string plain(cg::packet::receive::PetItemData::Code);
    plain.push_back(' ');
    plain.append(encodedPetSlot.Value());
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
            payload.push_back('|');
        }
        payload.append(parts[index]);
    }
    return payload;
}

std::vector<std::string> BuildItemParts()
{
    return {
        "1",
        "Pet\\zHelm",
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

cg::packet::Packet BuildManualPacket(int petSlot, std::string_view payload)
{
    cg::packet::Packet packet(
        cg::packet::PacketCode(cg::packet::receive::PetItemData::Code),
        cg::packet::PacketDirection::ServerToClient);
    packet.fields.push_back(cg::packet::PacketField {
        "petSlot",
        cg::packet::PacketFieldType::Int62,
        std::to_string(petSlot),
    });
    packet.fields.push_back(cg::packet::PacketField {
        "data",
        cg::packet::PacketFieldType::EscapedString,
        std::string(payload),
    });
    return packet;
}

int ExpectPetItemPacket(
    const cg::packet::Packet& packet,
    int petSlot,
    std::string_view value,
    cg::packet::PacketFieldType dataFieldType,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::PetItemData::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "petSlot", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[0].value == std::to_string(petSlot), message);
    failures += Expect(packet.fields[1].name == "data", message);
    failures += Expect(packet.fields[1].type == dataFieldType, message);
    failures += Expect(packet.fields[1].value == value, message);
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
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlainIKe(3, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse iKe outer");
    if (parsed.HasValue()) {
        failures += ExpectPetItemPacket(
            parsed.Value(),
            3,
            payload,
            cg::packet::PacketFieldType::EscapedString,
            "parse iKe outer packet");
    }
    failures += Expect(
        cg::packet::receive::PetItemData::Fields[1].type == cg::packet::PacketFieldType::SubPayload,
        "iKe data spec is SubPayload");
    return failures;
}

int TestSingleItemRoundtrip()
{
    const std::string payload = JoinParts(BuildItemParts());
    const cg::packet::PacketResult<cg::packet::receive::PetItemData> parsed =
        cg::packet::receive::PetItemData::Parse(BuildPlainIKe(3, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse iKe single item");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().petSlot == 3, "iKe petSlot");
    failures += Expect(parsed.Value().records.size() == 1U, "iKe single record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(
            parsed.Value().records[0],
            1,
            "Pet|Helm",
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
            "iKe single record");
    }
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "iKe single BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "iKe single BuildData value");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "iKe single ToPacket");
    if (packet.HasValue()) {
        failures += ExpectPetItemPacket(
            packet.Value(),
            3,
            payload,
            cg::packet::PacketFieldType::SubPayload,
            "iKe single ToPacket value");
        const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
        failures += Expect(static_cast<bool>(built), "build iKe single packet");
        if (built.HasValue()) {
            failures += Expect(built.Value() == BuildPlainIKe(3, payload), "roundtrip iKe single packet");
        }
    }
    return failures;
}

int TestClearSlot()
{
    const std::string payload = JoinParts(BuildClearParts(0));
    const cg::packet::PacketResult<cg::packet::receive::PetItemData> parsed =
        cg::packet::receive::PetItemData::Parse(BuildPlainIKe(2, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse iKe clear slot");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "iKe clear slot count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectClearRecord(parsed.Value().records[0], 0, "iKe clear slot record");
    }
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "iKe clear slot BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == "0|||||||||||||", "iKe clear slot BuildData value");
    }
    return failures;
}

int TestMultiRecordRoundtrip()
{
    std::vector<std::string> parts = BuildItemParts();
    const std::vector<std::string> clearParts = BuildClearParts(0);
    parts.insert(parts.end(), clearParts.begin(), clearParts.end());
    const std::string payload = JoinParts(parts);

    const cg::packet::PacketResult<cg::packet::receive::PetItemData> parsed =
        cg::packet::receive::PetItemData::Parse(BuildPlainIKe(2, payload));
    int failures = Expect(static_cast<bool>(parsed), "parse iKe multi record");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 2U, "iKe multi record count");
    if (parsed.Value().records.size() == 2U) {
        failures += ExpectClearRecord(parsed.Value().records[1], 0, "iKe multi clear record");
    }
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "iKe multi BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "iKe multi BuildData value");
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
    const cg::packet::PacketResult<cg::packet::receive::PetItemData> parsed =
        cg::packet::receive::PetItemData::Parse(BuildPlainIKe(4, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse iKe legacy text");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "iKe legacy text count");
    if (parsed.Value().records.size() == 1U) {
        failures += Expect(parsed.Value().records[0].nameRaw == semantic, "iKe semantic name");
        failures += Expect(parsed.Value().records[0].explanationRaw == semantic, "iKe semantic explanation");
        failures += Expect(parsed.Value().records[0].memoRaw == semantic, "iKe semantic memo");
    }
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "iKe canonical legacy BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == canonical, "iKe canonical legacy BuildData value");
    }
    return failures;
}

int TestEmptyPayload()
{
    const cg::packet::PacketResult<cg::packet::receive::PetItemData> parsed =
        cg::packet::receive::PetItemData::Parse(BuildPlainIKe(2, ""));

    int failures = Expect(static_cast<bool>(parsed), "parse iKe empty payload");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.empty(), "iKe empty payload records");
        const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
        failures += Expect(static_cast<bool>(data), "iKe empty payload BuildData");
        if (data.HasValue()) {
            failures += Expect(data.Value().empty(), "iKe empty payload BuildData value");
        }
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;

    std::vector<std::string> parts = BuildItemParts();
    parts.pop_back();
    failures += ExpectError(
        cg::packet::receive::PetItemData::Parse(BuildPlainIKe(2, JoinParts(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "iKe",
        "iKe thirteen fields");

    parts = BuildItemParts();
    parts.push_back("extra");
    failures += ExpectError(
        cg::packet::receive::PetItemData::Parse(BuildPlainIKe(2, JoinParts(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "iKe",
        "iKe fifteen fields");

    parts = BuildItemParts();
    parts[0] = "x";
    failures += ExpectError(
        cg::packet::receive::PetItemData::Parse(BuildPlainIKe(2, JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "iKe",
        "iKe bad slot decimal");

    parts = BuildItemParts();
    parts[2] = "x";
    failures += ExpectError(
        cg::packet::receive::PetItemData::Parse(BuildPlainIKe(2, JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "iKe",
        "iKe bad numeric decimal");

    parts = BuildItemParts();
    parts[2].clear();
    failures += ExpectError(
        cg::packet::receive::PetItemData::Parse(BuildPlainIKe(2, JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "iKe",
        "iKe empty numeric decimal");

    parts = BuildItemParts();
    parts[4] = "2147483648";
    failures += ExpectError(
        cg::packet::receive::PetItemData::Parse(BuildPlainIKe(2, JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "iKe",
        "iKe numeric overflow");

    parts = BuildItemParts();
    parts[1].clear();
    failures += ExpectErrorAt(
        cg::packet::receive::PetItemData::Parse(BuildPlainIKe(2, JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "iKe empty name with non-empty fields");

    parts = BuildItemParts();
    parts[1] = "Name\\";
    failures += ExpectErrorAt(
        cg::packet::receive::PetItemData::Parse(BuildPlainIKe(2, JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "iKe truncated name escape");

    parts = BuildItemParts();
    parts[3] = "Explanation\\";
    failures += ExpectErrorAt(
        cg::packet::receive::PetItemData::Parse(BuildPlainIKe(2, JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        3U,
        "iKe truncated explanation escape");

    parts = BuildItemParts();
    parts[13] = "Memo\\";
    failures += ExpectErrorAt(
        cg::packet::receive::PetItemData::Parse(BuildPlainIKe(2, JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        13U,
        "iKe truncated memo escape");

    parts = BuildItemParts();
    parts[1] = "Name";
    parts[3] = "Explanation";
    parts[13] = "Memo";
    std::string truncatedGbk = "iKe 2 ";
    truncatedGbk.append(JoinParts(parts));
    truncatedGbk.push_back(static_cast<char>(0x81));
    failures += ExpectErrorAt(
        cg::packet::receive::PetItemData::Parse(truncatedGbk),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "iKe truncated GBK payload");

    failures += ExpectError(
        cg::packet::receive::PetItemData::Parse("rU data "),
        cg::packet::PacketErrorCode::InvalidCode,
        "rU",
        "iKe wrong code");

    failures += ExpectError(
        cg::packet::receive::PetItemData::Parse("iKe 2"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "iKe",
        "iKe missing outer data");

    failures += ExpectError(
        cg::packet::receive::PetItemData::Parse("iKe 2 data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "iKe",
        "iKe extra outer data");

    failures += ExpectError(
        cg::packet::receive::PetItemData::Parse("iKe _ data "),
        cg::packet::PacketErrorCode::DecodeFailed,
        "iKe",
        "iKe bad petSlot i62");

    cg::packet::Packet packet = BuildManualPacket(2, JoinParts(BuildItemParts()));
    packet.direction = cg::packet::PacketDirection::ClientToServer;
    failures += ExpectError(
        cg::packet::receive::PetItemData::FromPacket(packet),
        cg::packet::PacketErrorCode::DecodeFailed,
        "iKe",
        "iKe wrong manual direction");

    packet = BuildManualPacket(2, JoinParts(BuildItemParts()));
    packet.fields[0].name = "slot";
    failures += ExpectError(
        cg::packet::receive::PetItemData::FromPacket(packet),
        cg::packet::PacketErrorCode::DecodeFailed,
        "iKe",
        "iKe wrong manual field name");

    packet = BuildManualPacket(2, JoinParts(BuildItemParts()));
    packet.fields[1].type = cg::packet::PacketFieldType::Unknown;
    failures += ExpectError(
        cg::packet::receive::PetItemData::FromPacket(packet),
        cg::packet::PacketErrorCode::InvalidFieldType,
        "iKe",
        "iKe wrong manual field type");

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
        cg::packet::receive::PetItemData(2, {invalidName}).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        1U,
        "iKe BuildData name encode context");
    failures += ExpectErrorAt(
        cg::packet::receive::PetItemData(2, {invalidExplanation}).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        3U,
        "iKe BuildData explanation encode context");
    const cg::packet::receive::PetItemData memoData(2, {valid, invalidMemo});
    failures += ExpectErrorAt(
        memoData.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        27U,
        "iKe BuildData second memo encode context");
    failures += ExpectErrorAt(
        memoData.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        27U,
        "iKe ToPacket second memo encode context");
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
    failures += TestErrors();
    failures += TestBuildAndToPacketErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
