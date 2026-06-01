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
    std::string_view value,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::TitleList::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.source == cg::packet::PacketSourceTag::Xlsx, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[0].source == cg::packet::PacketSourceTag::Xlsx, message);
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

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        cg::packet::ParsePacket(BuildPlainSc("Hero|1|100|||"));

    int failures = Expect(static_cast<bool>(parsed), "parse Sc outer");
    if (parsed.HasValue()) {
        failures += ExpectTitlePacket(parsed.Value(), "Hero|1|100|||", "parse Sc outer packet");
    }
    return failures;
}

int TestTypedParseSlots()
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
    return failures;
}

int TestRoundtripKeepsEmptySlot()
{
    const cg::packet::PacketResult<cg::packet::receive::TitleList> parsed =
        cg::packet::receive::TitleList::Parse(BuildPlainSc("Hero|1|100|||"));

    int failures = Expect(static_cast<bool>(parsed), "roundtrip Sc parse");
    if (!parsed.HasValue()) {
        return failures;
    }

    const cg::packet::Packet packet = parsed.Value().ToPacket();
    failures += ExpectTitlePacket(packet, "Hero|1|100|||", "Sc ToPacket keeps empty slot");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(packet);
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
    const cg::packet::Packet packet = titleList.ToPacket();
    failures += ExpectTitlePacket(packet, "", "Sc empty ToPacket");

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(packet);
    failures += Expect(static_cast<bool>(built), "build Sc empty payload");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainSc(""), "build Sc empty payload text");
    }
    return failures;
}

int TestBuildManualSlots()
{
    std::vector<cg::packet::receive::TitleSlot> slots;
    slots.emplace_back("Hero", 1, 100);
    slots.push_back(cg::packet::receive::TitleSlot::Empty());

    const cg::packet::receive::TitleList titleList(std::move(slots));
    const cg::packet::Packet packet = titleList.ToPacket();
    return ExpectTitlePacket(packet, "Hero|1|100|||", "Sc manual ToPacket");
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        cg::packet::receive::TitleList::Parse(BuildPlainSc("Hero|1")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "Sc",
        "typed Sc field count not multiple of three");
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
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestTypedParseSlots();
    failures += TestRoundtripKeepsEmptySlot();
    failures += TestEmptyPayload();
    failures += TestBuildManualSlots();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
