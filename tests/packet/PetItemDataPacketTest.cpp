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
    failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::EscapedString, message);
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
        failures += ExpectPetItemPacket(parsed.Value(), 3, payload, "parse iKe outer packet");
    }
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
            "Pet\\zHelm",
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
            "iKe single record");
    }
    failures += Expect(parsed.Value().BuildData() == payload, "iKe single BuildData");
    failures += ExpectPetItemPacket(parsed.Value().ToPacket(), 3, payload, "iKe single ToPacket");

    const cg::packet::PacketResult<std::string> built = BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build iKe single packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainIKe(3, payload), "roundtrip iKe single packet");
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
    failures += Expect(parsed.Value().BuildData() == "0|||||||||||||", "iKe clear slot BuildData");
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
    failures += Expect(parsed.Value().BuildData() == payload, "iKe multi BuildData");
    return failures;
}

int TestRawInnerEscapesPreserved()
{
    const std::string raw = "A\\zB\\nC\\cD";
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
    const cg::packet::PacketResult<cg::packet::receive::PetItemData> parsed =
        cg::packet::receive::PetItemData::Parse(BuildPlainIKe(4, payload));

    int failures = Expect(static_cast<bool>(parsed), "parse iKe raw inner escapes");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "iKe raw inner count");
    if (parsed.Value().records.size() == 1U) {
        failures += Expect(parsed.Value().records[0].nameRaw == raw, "iKe raw name");
        failures += Expect(parsed.Value().records[0].explanationRaw == raw, "iKe raw explanation");
        failures += Expect(parsed.Value().records[0].memoRaw == raw, "iKe raw memo");
    }
    failures += Expect(parsed.Value().BuildData() == payload, "iKe raw inner BuildData");
    return failures;
}

int TestEmptyPayload()
{
    const cg::packet::PacketResult<cg::packet::receive::PetItemData> parsed =
        cg::packet::receive::PetItemData::Parse(BuildPlainIKe(2, ""));

    int failures = Expect(static_cast<bool>(parsed), "parse iKe empty payload");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.empty(), "iKe empty payload records");
        failures += Expect(parsed.Value().BuildData().empty(), "iKe empty payload BuildData");
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
    failures += ExpectError(
        cg::packet::receive::PetItemData::Parse(BuildPlainIKe(2, JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "iKe",
        "iKe empty name with non-empty fields");

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
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
