#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

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

std::string BuildPlainELoy(std::string_view payload)
{
    return BuildPlain(cg::packet::receive::ItemInfoList::Code, payload);
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

std::vector<std::string> BuildEmptySlotParts()
{
    return std::vector<std::string>(cg::packet::receive::ItemInfoList::SlotFieldCount);
}

std::vector<std::string> BuildItemSlotParts()
{
    return {
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

std::vector<std::string> BuildPayloadParts(
    std::string ignored0,
    const std::vector<std::string>& firstSlot)
{
    std::vector<std::string> parts;
    parts.reserve(1U +
        (cg::packet::receive::ItemInfoList::SlotCount *
            cg::packet::receive::ItemInfoList::SlotFieldCount));
    parts.push_back(std::move(ignored0));
    parts.insert(parts.end(), firstSlot.begin(), firstSlot.end());
    for (std::size_t index = 1U; index < cg::packet::receive::ItemInfoList::SlotCount; ++index) {
        std::vector<std::string> emptySlot = BuildEmptySlotParts();
        parts.insert(parts.end(), emptySlot.begin(), emptySlot.end());
    }
    return parts;
}

std::string BuildOneItemPayload()
{
    return JoinParts(BuildPayloadParts("ignored\\z0", BuildItemSlotParts()));
}

int ExpectItemInfoListPacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::ItemInfoList::Code), message);
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

int ExpectSlot(
    const cg::packet::receive::ItemInfoSlot& slot,
    std::string_view nameRaw,
    int color,
    std::string_view descriptionRaw,
    int imageNo,
    int canUseInField,
    int battleUseFlag,
    int target,
    int level,
    int flag,
    int itemId,
    int itemType,
    int count,
    std::string_view memoRaw,
    const char* message)
{
    int failures = 0;
    failures += Expect(!slot.emptySlot, message);
    failures += Expect(slot.nameRaw == nameRaw, message);
    failures += Expect(slot.color == color, message);
    failures += Expect(slot.descriptionRaw == descriptionRaw, message);
    failures += Expect(slot.imageNo == imageNo, message);
    failures += Expect(slot.canUseInField == canUseInField, message);
    failures += Expect(slot.battleUseFlag == battleUseFlag, message);
    failures += Expect(slot.target == target, message);
    failures += Expect(slot.level == level, message);
    failures += Expect(slot.flag == flag, message);
    failures += Expect(slot.itemId == itemId, message);
    failures += Expect(slot.itemType == itemType, message);
    failures += Expect(slot.count == count, message);
    failures += Expect(slot.memoRaw == memoRaw, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = BuildOneItemPayload();
    const cg::packet::PacketResult<cg::packet::Packet> parsed = cg::packet::ParsePacket(BuildPlainELoy(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse ELoy outer");
    if (parsed.HasValue()) {
        failures += ExpectItemInfoListPacket(parsed.Value(), payload, "parse ELoy outer packet");
    }
    return failures;
}

int TestEmptyPayloadClearsSlots()
{
    const cg::packet::PacketResult<cg::packet::receive::ItemInfoList> parsed =
        cg::packet::receive::ItemInfoList::Parse(BuildPlainELoy(""));

    int failures = Expect(static_cast<bool>(parsed), "parse ELoy empty payload");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().ignored0.empty(), "ELoy empty ignored0");
    for (const cg::packet::receive::ItemInfoSlot& slot : parsed.Value().slots) {
        failures += Expect(slot.emptySlot, "ELoy empty slot");
    }
    failures += Expect(parsed.Value().BuildData().empty(), "ELoy empty BuildData");
    return failures;
}

int TestSingleItemRoundtrip()
{
    const std::string payload = BuildOneItemPayload();
    const cg::packet::PacketResult<cg::packet::receive::ItemInfoList> parsed =
        cg::packet::receive::ItemInfoList::Parse(BuildPlainELoy(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse ELoy single item");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().ignored0 == "ignored\\z0", "ELoy ignored0 raw");
    failures += Expect(parsed.Value().slots.size() == cg::packet::receive::ItemInfoList::SlotCount, "ELoy slot count");
    failures += ExpectSlot(
        parsed.Value().slots[0],
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
        "ELoy first slot");
    for (std::size_t index = 1U; index < parsed.Value().slots.size(); ++index) {
        failures += Expect(parsed.Value().slots[index].emptySlot, "ELoy trailing empty slot");
    }
    failures += Expect(parsed.Value().BuildData() == payload, "ELoy BuildData");
    failures += ExpectItemInfoListPacket(parsed.Value().ToPacket(), payload, "ELoy ToPacket");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(parsed.Value().ToPacket());
    failures += Expect(static_cast<bool>(built), "build ELoy packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainELoy(payload), "ELoy packet roundtrip");
    }
    return failures;
}

int TestRawInnerEscapesPreserved()
{
    const std::string raw = "A\\zB\\cC\\nD\\yE";
    std::vector<std::string> slot = BuildItemSlotParts();
    slot[0] = raw;
    slot[2] = raw;
    slot[12] = raw;
    const std::string payload = JoinParts(BuildPayloadParts("ignored0", slot));

    const cg::packet::PacketResult<cg::packet::receive::ItemInfoList> parsed =
        cg::packet::receive::ItemInfoList::Parse(BuildPlainELoy(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse ELoy raw inner escapes");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().slots[0].nameRaw == raw, "ELoy raw name");
    failures += Expect(parsed.Value().slots[0].descriptionRaw == raw, "ELoy raw description");
    failures += Expect(parsed.Value().slots[0].memoRaw == raw, "ELoy raw memo");
    failures += Expect(parsed.Value().BuildData() == payload, "ELoy raw BuildData");
    return failures;
}

int TestErrors()
{
    int failures = 0;

    std::vector<std::string> parts = BuildPayloadParts("ignored0", BuildItemSlotParts());
    parts.pop_back();
    failures += ExpectError(
        cg::packet::receive::ItemInfoList::Parse(BuildPlainELoy(JoinParts(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ELoy",
        "ELoy missing payload field");

    parts = BuildPayloadParts("ignored0", BuildItemSlotParts());
    parts[2] = "x";
    failures += ExpectError(
        cg::packet::receive::ItemInfoList::Parse(BuildPlainELoy(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ELoy",
        "ELoy bad decimal");

    parts = BuildPayloadParts("ignored0", BuildItemSlotParts());
    parts[4] = "2147483648";
    failures += ExpectError(
        cg::packet::receive::ItemInfoList::Parse(BuildPlainELoy(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ELoy",
        "ELoy decimal overflow");

    failures += ExpectError(
        cg::packet::receive::ItemInfoList::Parse(BuildPlain("rU", BuildOneItemPayload())),
        cg::packet::PacketErrorCode::InvalidCode,
        "rU",
        "ELoy wrong code");

    failures += ExpectError(
        cg::packet::receive::ItemInfoList::Parse("ELoy"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ELoy",
        "ELoy missing outer data");

    failures += ExpectError(
        cg::packet::receive::ItemInfoList::Parse("ELoy data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "ELoy",
        "ELoy extra outer token");

    std::string badOuter("ELoy ");
    badOuter.push_back(static_cast<char>(0x80U));
    badOuter.push_back(' ');
    failures += ExpectError(
        cg::packet::receive::ItemInfoList::Parse(badOuter),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ELoy",
        "ELoy outer es truncated");

    parts = BuildPayloadParts("ignored0", BuildItemSlotParts());
    parts[1].push_back(static_cast<char>(0x80U));
    failures += ExpectError(
        cg::packet::receive::ItemInfoList::Parse(BuildPlainELoy(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "ELoy",
        "ELoy nested raw string truncated");

    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestEmptyPayloadClearsSlots();
    failures += TestSingleItemRoundtrip();
    failures += TestRawInnerEscapesPreserved();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
