#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <array>
#include <cstddef>
#include <iostream>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace {

using AddressBook = cg::packet::receive::AddressBook;
using AddressBookSlot = cg::packet::receive::AddressBookSlot;

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

std::string JoinParts(const std::array<std::string, AddressBook::PayloadFieldCount>& parts)
{
    const cg::packet::PacketResult<std::string> payload = cg::packet::BuildPayload(
        std::span<const std::string>(parts.data(), parts.size()),
        AddressBook::PayloadDelimiter);
    return payload.HasValue() ? payload.Value() : std::string();
}

std::array<std::string, AddressBook::PayloadFieldCount> BuildEmptyParts()
{
    return {};
}

std::string BuildOneActivePayload()
{
    std::array<std::string, AddressBook::PayloadFieldCount> parts = BuildEmptyParts();
    parts[0] = "2";
    parts[1] = "Alice";
    parts[2] = "Leader";
    parts[3] = "123";
    parts[4] = "45";
    parts[5] = "-1";
    parts[6] = "987";
    return JoinParts(parts);
}

std::string BuildPlain(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedPayload = cg::packet::EncodeEscapedString(payload);
    if (!encodedPayload.HasValue()) {
        return "gnxd  ";
    }

    std::string plain = "gnxd ";
    plain.append(encodedPayload.Value());
    plain.push_back(' ');
    return plain;
}

std::string BuildRawPlain(std::string_view payload)
{
    std::string plain = "gnxd ";
    plain.append(payload);
    plain.push_back(' ');
    return plain;
}

int ExpectAddressBookPacket(
    const cg::packet::Packet& packet,
    std::string_view payload,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(AddressBook::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.source == cg::packet::PacketSourceTag::Xlsx, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[0].source == cg::packet::PacketSourceTag::Xlsx, message);
    failures += Expect(packet.fields[0].value == payload, message);
    return failures;
}

int ExpectAddressBookPacket(
    const cg::packet::PacketResult<cg::packet::Packet>& packet,
    std::string_view payload,
    const char* message)
{
    int failures = Expect(static_cast<bool>(packet), message);
    if (packet.HasValue()) {
        failures += ExpectAddressBookPacket(packet.Value(), payload, message);
    }
    return failures;
}

int ExpectActiveSlot(
    const AddressBookSlot& slot,
    int use,
    std::string_view charNameRaw,
    std::string_view ownTitleRaw,
    int registSeqNumber,
    int level,
    int online,
    int graphicsNo,
    const char* message)
{
    int failures = 0;
    failures += Expect(slot.IsUsed(), message);
    failures += Expect(slot.use == use, message);
    failures += Expect(slot.charNameRaw == charNameRaw, message);
    failures += Expect(slot.ownTitleRaw == ownTitleRaw, message);
    failures += Expect(slot.registSeqNumber == registSeqNumber, message);
    failures += Expect(slot.level == level, message);
    failures += Expect(slot.online == online, message);
    failures += Expect(slot.graphicsNo == graphicsNo, message);
    return failures;
}

int ExpectUnusedSlots(const AddressBook& addressBook, std::size_t firstSlot, const char* message)
{
    int failures = 0;
    for (std::size_t index = firstSlot; index < addressBook.slots.size(); ++index) {
        failures += Expect(!addressBook.slots[index].IsUsed(), message);
        failures += Expect(addressBook.slots[index].charNameRaw.empty(), message);
        failures += Expect(addressBook.slots[index].ownTitleRaw.empty(), message);
    }
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = BuildOneActivePayload();
    const cg::packet::PacketResult<cg::packet::Packet> parsed = cg::packet::ParsePacket(BuildPlain(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse gnxd outer");
    if (parsed.HasValue()) {
        failures += ExpectAddressBookPacket(parsed.Value(), payload, "parse gnxd outer packet");
    }
    return failures;
}

int TestOneActiveRoundtrip()
{
    std::array<AddressBookSlot, AddressBook::SlotCount> slots = {};
    slots[0] = AddressBookSlot {2, "Alice", "Leader", 123, 45, -1, 987};
    const AddressBook addressBook(slots);
    const std::string payload = BuildOneActivePayload();

    const cg::packet::PacketResult<std::string> data = addressBook.BuildData();
    int failures = Expect(static_cast<bool>(data), "gnxd one active BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "gnxd one active payload");
        const cg::packet::PacketResult<std::vector<std::string>> parts =
            cg::packet::SplitPayloadLegacyAware(data.Value(), AddressBook::PayloadDelimiter);
        failures += Expect(parts.HasValue(), "gnxd one active split");
        if (parts.HasValue()) {
            failures += Expect(parts.Value().size() == AddressBook::PayloadFieldCount, "gnxd no extra trailing field");
        }
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = addressBook.ToPacket();
    failures += ExpectAddressBookPacket(packet, payload, "gnxd one active ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = cg::packet::BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build gnxd packet");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<AddressBook> parsed = AddressBook::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip gnxd packet");
    if (parsed.HasValue()) {
        failures += ExpectActiveSlot(parsed.Value().slots[0], 2, "Alice", "Leader", 123, 45, -1, 987, "gnxd slot 0");
        failures += ExpectUnusedSlots(parsed.Value(), 1U, "gnxd empty slots");
        const cg::packet::PacketResult<std::string> parsedData = parsed.Value().BuildData();
        failures += Expect(parsedData.HasValue(), "gnxd parsed BuildData");
        if (parsedData.HasValue()) {
            failures += Expect(parsedData.Value() == payload, "gnxd parsed canonical BuildData");
        }
    }
    return failures;
}

int TestAllEmptyTable()
{
    const std::string payload = JoinParts(BuildEmptyParts());
    const cg::packet::PacketResult<AddressBook> parsed = AddressBook::Parse(BuildPlain(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse gnxd all empty table");
    if (parsed.HasValue()) {
        failures += ExpectUnusedSlots(parsed.Value(), 0U, "gnxd all empty slots");
        const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
        failures += Expect(data.HasValue(), "gnxd all empty BuildData");
        if (data.HasValue()) {
            failures += Expect(data.Value() == payload, "gnxd all empty canonical payload");
        }
    }
    return failures;
}

int TestLegacyTrailingEmptyCompatible()
{
    const std::string payload = BuildOneActivePayload();
    const cg::packet::PacketResult<AddressBook> parsed = AddressBook::Parse(BuildPlain(payload + "|"));

    int failures = Expect(static_cast<bool>(parsed), "parse gnxd 421 trailing empty");
    if (parsed.HasValue()) {
        const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
        failures += Expect(data.HasValue(), "gnxd 421 canonical BuildData");
        if (data.HasValue()) {
            failures += Expect(data.Value() == payload, "gnxd 421 drops extra empty tail");
        }
    }
    return failures;
}

int TestRawEscapePreserved()
{
    std::array<std::string, AddressBook::PayloadFieldCount> parts = BuildEmptyParts();
    parts[0] = "3";
    parts[1] = "A\\zB";
    parts[2] = "T\\cD";
    parts[3] = "1";
    parts[4] = "2";
    parts[5] = "0";
    parts[6] = "4";
    const std::string payload = JoinParts(parts);

    const cg::packet::PacketResult<AddressBook> parsed = AddressBook::Parse(BuildRawPlain(payload));
    int failures = Expect(static_cast<bool>(parsed), "parse gnxd raw escape");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectActiveSlot(parsed.Value().slots[0], 3, "A\\zB", "T\\cD", 1, 2, 0, 4, "gnxd raw slot");
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(data.HasValue(), "gnxd raw BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "gnxd raw escape BuildData");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        AddressBook::Parse("gnxd"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "gnxd",
        "gnxd missing data");
    failures += ExpectError(
        AddressBook::Parse("gnxd a b"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "gnxd",
        "gnxd outer extra field");
    failures += ExpectError(
        AddressBook::Parse(BuildPlain("1|Alice")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "gnxd",
        "gnxd payload field count");

    std::array<std::string, AddressBook::PayloadFieldCount> parts = BuildEmptyParts();
    parts[0] = "1";
    parts[1] = "Alice";
    parts[2] = "Title";
    parts[3] = "x";
    parts[4] = "2";
    parts[5] = "0";
    parts[6] = "4";
    failures += ExpectError(
        AddressBook::Parse(BuildPlain(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "gnxd",
        "gnxd active numeric x");

    parts = BuildEmptyParts();
    parts[1] = "Alice";
    failures += ExpectError(
        AddressBook::Parse(BuildPlain(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "gnxd",
        "gnxd empty use with non-empty slot");

    parts = BuildEmptyParts();
    parts[0] = "0";
    parts[1] = "Alice";
    failures += ExpectError(
        AddressBook::Parse(BuildPlain(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "gnxd",
        "gnxd use zero with non-empty slot");

    parts = BuildEmptyParts();
    parts[0] = "a";
    parts[1] = "Alice";
    failures += ExpectError(
        AddressBook::Parse(BuildPlain(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "gnxd",
        "gnxd rejects i62-like use");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestOneActiveRoundtrip();
    failures += TestAllEmptyTable();
    failures += TestLegacyTrailingEmptyCompatible();
    failures += TestRawEscapePreserved();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
