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
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::ItemInfoList::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::ItemInfoList::Spec);
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
    int failures = ExpectError(result, code, "ELoy", message);
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

cg::packet::receive::ItemInfoSlot BuildSemanticItemSlot()
{
    return {
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
        false,
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
    cg::packet::PacketFieldType dataFieldType,
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
    failures += Expect(packet.fields[0].type == dataFieldType, message);
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
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlainELoy(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse ELoy outer");
    if (parsed.HasValue()) {
        failures += ExpectItemInfoListPacket(
            parsed.Value(),
            payload,
            cg::packet::PacketFieldType::EscapedString,
            "parse ELoy outer packet");
    }
    failures += Expect(
        cg::packet::receive::ItemInfoList::Fields[0].type == cg::packet::PacketFieldType::SubPayload,
        "ELoy data spec is SubPayload");
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
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "ELoy empty BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value().empty(), "ELoy empty BuildData value");
    }
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
        "ELoy first slot");
    for (std::size_t index = 1U; index < parsed.Value().slots.size(); ++index) {
        failures += Expect(parsed.Value().slots[index].emptySlot, "ELoy trailing empty slot");
    }
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "ELoy BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "ELoy BuildData value");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "ELoy ToPacket");
    if (packet.HasValue()) {
        failures += ExpectItemInfoListPacket(
            packet.Value(),
            payload,
            cg::packet::PacketFieldType::SubPayload,
            "ELoy ToPacket value");

        const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
        failures += Expect(static_cast<bool>(built), "build ELoy packet");
        if (built.HasValue()) {
            failures += Expect(built.Value() == BuildPlainELoy(payload), "ELoy packet roundtrip");
        }
    }
    return failures;
}

int TestLegacyTextCanonical()
{
    const std::string gbkEast = Bytes({0x96U, 0x7cU});
    std::string legacyText = gbkEast;
    legacyText.append("\\nline\\ccomma\\zpipe\\yslash\\q");
    std::string semanticText = gbkEast;
    semanticText.append("\nline,comma|pipe\\slashq");
    std::string canonicalText = gbkEast;
    canonicalText.append("\\nline\\ccomma\\zpipe\\yslashq");

    std::vector<std::string> legacySlot = BuildItemSlotParts();
    legacySlot[0] = legacyText;
    legacySlot[2] = legacyText;
    legacySlot[12] = legacyText;
    const std::string ignored0 = "ignored\\q";
    const std::string payload = JoinParts(BuildPayloadParts(ignored0, legacySlot));

    std::vector<std::string> canonicalSlot = BuildItemSlotParts();
    canonicalSlot[0] = canonicalText;
    canonicalSlot[2] = canonicalText;
    canonicalSlot[12] = canonicalText;
    const std::string canonicalPayload = JoinParts(BuildPayloadParts(ignored0, canonicalSlot));

    const cg::packet::PacketResult<cg::packet::receive::ItemInfoList> parsed =
        cg::packet::receive::ItemInfoList::Parse(BuildPlainELoy(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse ELoy legacy text");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().ignored0 == ignored0, "ELoy ignored0 opaque");
    failures += Expect(parsed.Value().slots[0].nameRaw == semanticText, "ELoy legacy name semantic");
    failures += Expect(parsed.Value().slots[0].descriptionRaw == semanticText, "ELoy legacy description semantic");
    failures += Expect(parsed.Value().slots[0].memoRaw == semanticText, "ELoy legacy memo semantic");

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(static_cast<bool>(data), "ELoy legacy canonical BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == canonicalPayload, "ELoy legacy canonical BuildData value");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "ELoy legacy canonical ToPacket");
    if (packet.HasValue()) {
        failures += ExpectItemInfoListPacket(
            packet.Value(),
            canonicalPayload,
            cg::packet::PacketFieldType::SubPayload,
            "ELoy legacy canonical ToPacket value");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;

    std::vector<std::string> parts = BuildPayloadParts("ignored0", BuildItemSlotParts());
    parts.pop_back();
    failures += ExpectErrorAt(
        cg::packet::receive::ItemInfoList::Parse(BuildPlainELoy(JoinParts(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        0U,
        "ELoy missing payload field");

    parts = BuildPayloadParts("ignored0", BuildItemSlotParts());
    parts.emplace_back("extra");
    failures += ExpectErrorAt(
        cg::packet::receive::ItemInfoList::Parse(BuildPlainELoy(JoinParts(parts))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        0U,
        "ELoy extra payload field");

    parts = BuildPayloadParts("ignored0", BuildItemSlotParts());
    parts[2] = "x";
    failures += ExpectErrorAt(
        cg::packet::receive::ItemInfoList::Parse(BuildPlainELoy(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        2U,
        "ELoy bad decimal");

    parts = BuildPayloadParts("ignored0", BuildItemSlotParts());
    parts[4] = "2147483648";
    failures += ExpectErrorAt(
        cg::packet::receive::ItemInfoList::Parse(BuildPlainELoy(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        4U,
        "ELoy decimal overflow");

    std::vector<std::string> partialSlot = BuildEmptySlotParts();
    partialSlot[0] = "Name";
    parts = BuildPayloadParts("ignored0", partialSlot);
    failures += ExpectErrorAt(
        cg::packet::receive::ItemInfoList::Parse(BuildPlainELoy(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        2U,
        "ELoy partial slot is not empty");

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
    parts[1].push_back('\\');
    failures += ExpectErrorAt(
        cg::packet::receive::ItemInfoList::Parse(BuildPlainELoy(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        1U,
        "ELoy name trailing backslash");

    parts = BuildPayloadParts("ignored0", BuildItemSlotParts());
    parts[3].push_back('\\');
    failures += ExpectErrorAt(
        cg::packet::receive::ItemInfoList::Parse(BuildPlainELoy(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        3U,
        "ELoy description trailing backslash");

    parts = BuildPayloadParts("ignored0", BuildItemSlotParts());
    parts[13].push_back('\\');
    failures += ExpectErrorAt(
        cg::packet::receive::ItemInfoList::Parse(BuildPlainELoy(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        13U,
        "ELoy memo trailing backslash");

    return failures;
}

int TestBuildAndToPacketErrors()
{
    const std::string truncatedGbk = Bytes({0x81U});
    int failures = 0;

    cg::packet::receive::ItemInfoList badName("", {});
    badName.slots[0] = BuildSemanticItemSlot();
    badName.slots[0].nameRaw = truncatedGbk;
    failures += ExpectErrorAt(
        badName.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        1U,
        "ELoy BuildData name truncated GBK context");

    cg::packet::receive::ItemInfoList badDescription("", {});
    badDescription.slots[0] = BuildSemanticItemSlot();
    badDescription.slots[0].descriptionRaw = truncatedGbk;
    failures += ExpectErrorAt(
        badDescription.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        3U,
        "ELoy BuildData description truncated GBK context");

    cg::packet::receive::ItemInfoList badSecondMemo("", {});
    badSecondMemo.slots[0] = BuildSemanticItemSlot();
    badSecondMemo.slots[1] = BuildSemanticItemSlot();
    badSecondMemo.slots[1].memoRaw = truncatedGbk;
    failures += ExpectErrorAt(
        badSecondMemo.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        26U,
        "ELoy BuildData second memo truncated GBK context");
    failures += ExpectErrorAt(
        badSecondMemo.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        26U,
        "ELoy ToPacket second memo truncated GBK context");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestEmptyPayloadClearsSlots();
    failures += TestSingleItemRoundtrip();
    failures += TestLegacyTextCanonical();
    failures += TestErrors();
    failures += TestBuildAndToPacketErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
