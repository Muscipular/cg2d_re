#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::TitleList::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::TitleList::Spec);
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

std::string BuildPlainSc(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded = cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return "Sc ";
    }

    std::string plain = "Sc ";
    plain.append(encoded.Value());
    plain.push_back(' ');
    return plain;
}

int ExpectTitlePacket(
    const cg::packet::Packet& packet,
    cg::packet::PacketFieldType fieldType,
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::TitleList::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == fieldType, message);
    failures += Expect(packet.fields[0].value == value, message);
    return failures;
}

int ExpectTitleSlot(
    const cg::packet::receive::TitleSlot& slot,
    std::string_view titleName,
    int titleMakeId,
    int titleId,
    bool empty,
    const char* message)
{
    int failures = 0;
    failures += Expect(slot.empty == empty, message);
    if (empty) {
        return failures;
    }

    failures += Expect(slot.titleName == titleName, message);
    failures += Expect(slot.titleMakeId == titleMakeId, message);
    failures += Expect(slot.titleId == titleId, message);
    return failures;
}

std::vector<cg::packet::receive::TitleSlot> MakeSlots(std::size_t count)
{
    std::vector<cg::packet::receive::TitleSlot> slots;
    slots.reserve(count);
    for (std::size_t index = 0; index < count; ++index) {
        slots.emplace_back("T", static_cast<int>(index), -static_cast<int>(index));
    }
    return slots;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket(BuildPlainSc("Hero|1|100|||"));

    int failures = Expect(static_cast<bool>(parsed), "parse Sc outer");
    if (parsed.HasValue()) {
        failures += ExpectTitlePacket(
            parsed.Value(),
            cg::packet::PacketFieldType::EscapedString,
            "Hero|1|100|||",
            "parse Sc outer packet");
    }
    return failures;
}

int TestTypedParseAndRoundtrip()
{
    const cg::packet::PacketResult<cg::packet::receive::TitleList> parsed =
        cg::packet::receive::TitleList::Parse(BuildPlainSc("Hero|1|100|||"));

    int failures = Expect(static_cast<bool>(parsed), "typed parse Sc slots");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().slots.size() == 2U, "typed Sc slot count");
    if (parsed.Value().slots.size() == 2U) {
        failures += ExpectTitleSlot(parsed.Value().slots[0], "Hero", 1, 100, false, "typed Sc filled slot");
        failures += ExpectTitleSlot(parsed.Value().slots[1], "", 0, 0, true, "typed Sc empty slot");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "Sc ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    failures += ExpectTitlePacket(
        packet.Value(),
        cg::packet::PacketFieldType::SubPayload,
        "Hero|1|100|||",
        "Sc ToPacket keeps empty slot");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build Sc packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainSc("Hero|1|100|||"), "roundtrip Sc empty slot text");
    }
    return failures;
}

int TestEmptyPayload()
{
    int failures = 0;
    const cg::packet::PacketResult<cg::packet::receive::TitleList> parsed =
        cg::packet::receive::TitleList::Parse(BuildPlainSc(""));
    failures += Expect(static_cast<bool>(parsed), "parse Sc empty payload");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().slots.empty(), "parse Sc empty slots");
    }

    const cg::packet::receive::TitleList titleList({});
    const cg::packet::PacketResult<std::string> data = titleList.BuildData();
    failures += Expect(data.HasValue() && data.Value().empty(), "Sc empty BuildData");

    const cg::packet::PacketResult<cg::packet::Packet> packet = titleList.ToPacket();
    failures += Expect(static_cast<bool>(packet), "Sc empty ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    failures += ExpectTitlePacket(
        packet.Value(),
        cg::packet::PacketFieldType::SubPayload,
        "",
        "Sc empty ToPacket value");
    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build Sc empty payload");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainSc(""), "build Sc empty payload text");
    }
    return failures;
}

int TestLegacyTextCanonical()
{
    constexpr std::string_view input =
        "Line\\nComma\\cPipe\\zSlash\\yUnknown\\q|1|-2";
    constexpr std::string_view canonical =
        "Line\\nComma\\cPipe\\zSlash\\yUnknownq|1|-2";
    constexpr std::string_view semantic =
        "Line\nComma,Pipe|Slash\\Unknownq";

    const cg::packet::PacketResult<cg::packet::receive::TitleList> parsed =
        cg::packet::receive::TitleList::Parse(BuildPlainSc(input));
    int failures = Expect(static_cast<bool>(parsed), "parse Sc legacy title text");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().slots.size() == 1U, "Sc legacy slot count");
    if (parsed.Value().slots.size() == 1U) {
        failures += ExpectTitleSlot(
            parsed.Value().slots[0],
            semantic,
            1,
            -2,
            false,
            "Sc legacy title semantic bytes");
    }

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(data.HasValue(), "build Sc legacy canonical data");
    if (data.HasValue()) {
        failures += Expect(data.Value() == canonical, "Sc unknown escape canonicalized");
    }
    return failures;
}

int TestGbkDelimiterByte()
{
    std::string titleName;
    titleName.push_back(static_cast<char>(0x81));
    titleName.push_back(static_cast<char>(0x7c));

    std::string payload = titleName;
    payload.append("|1|2");

    const cg::packet::PacketResult<cg::packet::receive::TitleList> parsed =
        cg::packet::receive::TitleList::Parse(BuildPlainSc(payload));
    int failures = Expect(static_cast<bool>(parsed), "parse Sc GBK trail delimiter byte");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().slots.size() == 1U, "Sc GBK slot count");
    if (parsed.Value().slots.size() == 1U) {
        failures += ExpectTitleSlot(parsed.Value().slots[0], titleName, 1, 2, false, "Sc GBK title bytes");
    }

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(data.HasValue(), "build Sc GBK title");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "roundtrip Sc GBK title");
    }
    return failures;
}

int TestSlotLimits()
{
    int failures = 0;
    const cg::packet::receive::TitleList maxList(MakeSlots(64U));
    const cg::packet::PacketResult<std::string> maxData = maxList.BuildData();
    failures += Expect(maxData.HasValue(), "build Sc 64 slots");
    if (maxData.HasValue()) {
        const cg::packet::PacketResult<cg::packet::receive::TitleList> parsed =
            cg::packet::receive::TitleList::Parse(BuildPlainSc(maxData.Value()));
        failures += Expect(parsed.HasValue(), "parse Sc 64 slots");
        if (parsed.HasValue()) {
            failures += Expect(parsed.Value().slots.size() == 64U, "Sc 64 slot count");
        }
    }

    const cg::packet::receive::TitleList overList(MakeSlots(65U));
    failures += ExpectError(
        overList.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Sc",
        "build Sc 65 slots rejected");

    std::string overPayload;
    for (std::size_t index = 0; index < 65U; ++index) {
        if (!overPayload.empty()) {
            overPayload.push_back('|');
        }
        overPayload.append("T|0|0");
    }
    failures += ExpectError(
        cg::packet::receive::TitleList::Parse(BuildPlainSc(overPayload)),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Sc",
        "parse Sc 65 slots rejected");
    return failures;
}

int TestParseErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::TitleList::Parse(BuildPlainSc("Hero|1")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Sc",
        "typed Sc missing tail field");
    failures += ExpectError(
        cg::packet::receive::TitleList::Parse(BuildPlainSc("Hero|1|100|extra")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Sc",
        "typed Sc extra field");
    failures += ExpectError(
        cg::packet::receive::TitleList::Parse(BuildPlainSc("Hero||")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Sc",
        "typed Sc partial empty ids");
    failures += ExpectError(
        cg::packet::receive::TitleList::Parse(BuildPlainSc("Hero|a|100")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Sc",
        "typed Sc titleMakeId raw decimal");
    failures += ExpectError(
        cg::packet::receive::TitleList::Parse(BuildPlainSc("Hero|1|a")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Sc",
        "typed Sc titleId raw decimal");
    failures += ExpectError(
        cg::packet::receive::TitleList::Parse(BuildPlainSc("Hero|+1|100")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Sc",
        "typed Sc signed decimal rejects plus");
    failures += ExpectError(
        cg::packet::receive::TitleList::Parse(BuildPlainSc("Hero|2147483648|100")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Sc",
        "typed Sc titleMakeId overflow");
    failures += ExpectError(
        cg::packet::receive::TitleList::Parse(BuildPlainSc("Hero|1|-2147483649")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "Sc",
        "typed Sc titleId underflow");
    return failures;
}

int TestPartialEmptyName()
{
    const cg::packet::PacketResult<cg::packet::receive::TitleList> parsed =
        cg::packet::receive::TitleList::Parse(BuildPlainSc("|1|100"));
    int failures = Expect(parsed.HasValue(), "parse Sc empty titleName with ids");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().slots.size() == 1U, "Sc partial empty slot count");
    if (parsed.Value().slots.size() == 1U) {
        failures += ExpectTitleSlot(
            parsed.Value().slots[0],
            "",
            1,
            100,
            false,
            "Sc empty titleName is non-empty slot");
    }

    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(data.HasValue(), "build Sc empty titleName with ids");
    if (data.HasValue()) {
        failures += Expect(data.Value() == "|1|100", "roundtrip Sc empty titleName with ids");
    }
    return failures;
}

int TestBuildErrors()
{
    std::string truncatedGbk(1U, static_cast<char>(0x81));
    const cg::packet::receive::TitleList truncatedList({
        cg::packet::receive::TitleSlot(std::move(truncatedGbk), 1, 2),
    });

    return ExpectError(
        truncatedList.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "Sc",
        "build Sc truncated GBK title");
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestTypedParseAndRoundtrip();
    failures += TestEmptyPayload();
    failures += TestLegacyTextCanonical();
    failures += TestGbkDelimiterByte();
    failures += TestSlotLimits();
    failures += TestParseErrors();
    failures += TestPartialEmptyName();
    failures += TestBuildErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
