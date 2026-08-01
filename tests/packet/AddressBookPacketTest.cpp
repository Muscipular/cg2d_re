#include "packet/PacketBuilder.hpp"
#include "packet/Receive.hpp"

#include <array>
#include <cstddef>
#include <initializer_list>
#include <iostream>
#include <span>
#include <string>
#include <string_view>
#include <vector>

namespace {

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::AddressBook::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::AddressBook::Spec);
}

using AddressBook = cg::packet::receive::AddressBook;
using AddressBookInfo = cg::packet::receive::AddressBookInfo;
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

template <typename T>
int ExpectFieldError(
    const cg::packet::PacketResult<T>& result,
    cg::packet::PacketErrorCode code,
    std::string_view packetCode,
    std::size_t fieldIndex,
    const char* message)
{
    int failures = ExpectError(result, code, packetCode, message);
    if (result.HasError()) {
        failures += Expect(result.Error().fieldIndex.has_value(), message);
        if (result.Error().fieldIndex.has_value()) {
            failures += Expect(*result.Error().fieldIndex == fieldIndex, message);
        }
    }
    return failures;
}

std::string Bytes(std::initializer_list<unsigned int> values)
{
    std::string output;
    output.reserve(values.size());
    for (const unsigned int value : values) {
        output.push_back(static_cast<char>(value));
    }
    return output;
}

std::string SemanticCharName()
{
    std::string value = Bytes({0xC3, 0xFB, 0x96, 0x7C});
    value.append("A|B,C\\D\nE");
    return value;
}

std::string EncodedCharName()
{
    std::string value = Bytes({0xC3, 0xFB, 0x96, 0x7C});
    value.append("A\\zB\\cC\\yD\\nE");
    return value;
}

std::string SemanticOwnTitle()
{
    std::string value = Bytes({0xD3, 0xC2, 0x96, 0x5C});
    value.append("F\nG,H|I\\J");
    return value;
}

std::string EncodedOwnTitle()
{
    std::string value = Bytes({0xD3, 0xC2, 0x96, 0x5C});
    value.append("F\\nG\\cH\\zI\\yJ");
    return value;
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

std::string BuildPlainVi(std::string_view registry, std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encodedPayload = cg::packet::EncodeEscapedString(payload);
    if (!encodedPayload.HasValue()) {
        return "vI   ";
    }

    std::string plain = "vI ";
    plain.append(registry);
    plain.push_back(' ');
    plain.append(encodedPayload.Value());
    plain.push_back(' ');
    return plain;
}

std::string BuildRawPlainVi(std::string_view registry, std::string_view payload)
{
    std::string plain = "vI ";
    plain.append(registry);
    plain.push_back(' ');
    plain.append(payload);
    plain.push_back(' ');
    return plain;
}

int ExpectAddressBookPacket(
    const cg::packet::Packet& packet,
    std::string_view payload,
    cg::packet::PacketFieldType payloadType,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(AddressBook::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == payloadType, message);
    failures += Expect(packet.fields[0].value == payload, message);
    return failures;
}

int ExpectAddressBookPacket(
    const cg::packet::PacketResult<cg::packet::Packet>& packet,
    std::string_view payload,
    cg::packet::PacketFieldType payloadType,
    const char* message)
{
    int failures = Expect(static_cast<bool>(packet), message);
    if (packet.HasValue()) {
        failures += ExpectAddressBookPacket(packet.Value(), payload, payloadType, message);
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

int ExpectAddressBookInfoPacket(
    const cg::packet::Packet& packet,
    int registry,
    std::string_view payload,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(AddressBookInfo::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 2U, message);
    if (packet.fields.size() != 2U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "registry", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::Int62, message);
    failures += Expect(packet.fields[0].value == std::to_string(registry), message);
    failures += Expect(packet.fields[1].name == "data", message);
    failures += Expect(packet.fields[1].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[1].value == payload, message);
    return failures;
}

int ExpectActiveAddressBookInfo(
    const AddressBookInfo& info,
    int registry,
    int use,
    std::string_view charNameRaw,
    std::string_view ownTitleRaw,
    int registSeqNumber,
    int level,
    int online,
    int graphicsNo,
    int clearGraphicsFlag,
    std::string_view field9Raw,
    std::string_view field10Raw,
    const char* message)
{
    int failures = 0;
    failures += Expect(info.IsActive(), message);
    failures += Expect(info.registry == registry, message);
    failures += Expect(info.use == use, message);
    failures += Expect(info.charNameRaw == charNameRaw, message);
    failures += Expect(info.ownTitleRaw == ownTitleRaw, message);
    failures += Expect(info.registSeqNumber == registSeqNumber, message);
    failures += Expect(info.level == level, message);
    failures += Expect(info.online == online, message);
    failures += Expect(info.graphicsNo == graphicsNo, message);
    failures += Expect(info.clearGraphicsFlag == clearGraphicsFlag, message);
    failures += Expect(info.field9Raw == field9Raw, message);
    failures += Expect(info.field10Raw == field10Raw, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = BuildOneActivePayload();
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlain(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse gnxd outer");
    if (parsed.HasValue()) {
        failures += ExpectAddressBookPacket(
            parsed.Value(),
            payload,
            cg::packet::PacketFieldType::EscapedString,
            "parse gnxd outer packet");
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
    failures += ExpectAddressBookPacket(
        packet,
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "gnxd one active ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
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

int TestStrictPayloadFieldCount()
{
    const std::string payload = BuildOneActivePayload();
    return ExpectFieldError(
        AddressBook::Parse(BuildPlain(payload + "|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "gnxd",
        0U,
        "gnxd rejects 421st empty field");
}

int TestLegacyTextRoundtrip()
{
    std::array<std::string, AddressBook::PayloadFieldCount> parts = BuildEmptyParts();
    parts[0] = "3";
    parts[1] = EncodedCharName();
    parts[2] = EncodedOwnTitle();
    parts[3] = "1";
    parts[4] = "2";
    parts[5] = "0";
    parts[6] = "4";
    const std::string payload = JoinParts(parts);

    const cg::packet::PacketResult<AddressBook> parsed = AddressBook::Parse(BuildPlain(payload));
    int failures = Expect(static_cast<bool>(parsed), "parse gnxd legacy text");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectActiveSlot(
        parsed.Value().slots[0],
        3,
        SemanticCharName(),
        SemanticOwnTitle(),
        1,
        2,
        0,
        4,
        "gnxd legacy semantic slot");
    const cg::packet::PacketResult<std::string> data = parsed.Value().BuildData();
    failures += Expect(data.HasValue(), "gnxd legacy BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == payload, "gnxd legacy canonical BuildData");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += ExpectAddressBookPacket(
        packet,
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "gnxd legacy ToPacket type");
    return failures;
}

int TestAddressBookInfoActiveRoundtrip()
{
    const std::string payload = "1|Alice|Leader|123|45|1|987|2|raw9|raw10";
    const cg::packet::PacketResult<cg::packet::Packet> parsedPacket =
        cg::packet::ParsePacket(BuildPlainVi("10", payload), cg::packet::receive::AddressBookInfo::Spec);

    int failures = Expect(static_cast<bool>(parsedPacket), "parse vI outer");
    if (parsedPacket.HasValue()) {
        failures += ExpectAddressBookInfoPacket(parsedPacket.Value(), 62, payload, "parse vI outer packet");
    }

    const cg::packet::PacketResult<AddressBookInfo> parsed = AddressBookInfo::Parse(BuildPlainVi("10", payload));
    failures += Expect(static_cast<bool>(parsed), "parse vI active");
    if (parsed.HasValue()) {
        failures += ExpectActiveAddressBookInfo(
            parsed.Value(),
            62,
            1,
            "Alice",
            "Leader",
            123,
            45,
            1,
            987,
            2,
            "raw9",
            "raw10",
            "vI active fields");
        failures += Expect(parsed.Value().BuildData() == payload, "vI active BuildData");
    }

    const AddressBookInfo info(62, 1, "Alice", "Leader", 123, 45, 1, 987, 2, "raw9", "raw10");
    failures += Expect(info.BuildData() == payload, "vI construct BuildData");
    const cg::packet::Packet packet = info.ToPacket();
    failures += ExpectAddressBookInfoPacket(packet, 62, payload, "vI ToPacket");

    const cg::packet::PacketResult<std::string> built =
        cg::packet::BuildPacket(packet, cg::packet::receive::AddressBookInfo::Spec);
    failures += Expect(static_cast<bool>(built), "build vI packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainVi("10", payload), "vI registry 62 encoded as 10");
    }
    return failures;
}

int TestAddressBookInfoInactive()
{
    const std::string payload = "0|||||||||";
    const cg::packet::PacketResult<AddressBookInfo> parsed = AddressBookInfo::Parse(BuildPlainVi("1", payload));

    int failures = Expect(static_cast<bool>(parsed), "parse vI inactive");
    if (parsed.HasValue()) {
        failures += Expect(!parsed.Value().IsActive(), "vI inactive state");
        failures += Expect(parsed.Value().registry == 1, "vI inactive registry");
        failures += Expect(parsed.Value().use == 0, "vI inactive use");
        failures += Expect(parsed.Value().BuildData() == payload, "vI inactive BuildData");
    }
    return failures;
}

int TestAddressBookInfoRawEscapePreserved()
{
    const std::string payload = "1|A\\zB|T\\cD|1|2|0|4|3|F\\y9|G\\z10";
    const cg::packet::PacketResult<AddressBookInfo> parsed = AddressBookInfo::Parse(BuildRawPlainVi("1", payload));

    int failures = Expect(static_cast<bool>(parsed), "parse vI raw escape");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += ExpectActiveAddressBookInfo(
        parsed.Value(),
        1,
        1,
        "A\\zB",
        "T\\cD",
        1,
        2,
        0,
        4,
        3,
        "F\\y9",
        "G\\z10",
        "vI raw escape fields");
    failures += Expect(parsed.Value().BuildData() == payload, "vI raw escape BuildData");
    return failures;
}

int TestAddressBookInfoPayloadCompatibility()
{
    const std::string payload = "1|Alice|Leader|123|45|1|987|2|raw9|raw10";
    const cg::packet::PacketResult<AddressBookInfo> withEmptyTail =
        AddressBookInfo::Parse(BuildPlainVi("1", payload + "|"));

    int failures = Expect(static_cast<bool>(withEmptyTail), "parse vI 11th empty tail");
    if (withEmptyTail.HasValue()) {
        failures += Expect(withEmptyTail.Value().BuildData() == payload, "vI drops empty 11th field");
    }

    failures += ExpectError(
        AddressBookInfo::Parse(BuildPlainVi("1", "1|Alice|Leader|123|45|1|987|2|raw9")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "vI",
        "vI 9 fields fail");
    failures += ExpectError(
        AddressBookInfo::Parse(BuildPlainVi("1", payload + "|extra")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "vI",
        "vI 11th non-empty fail");
    return failures;
}

int TestAddressBookInfoErrors()
{
    int failures = 0;
    failures += ExpectError(
        AddressBookInfo::Parse("vI"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "vI",
        "vI missing fields");
    failures += ExpectError(
        AddressBookInfo::Parse(BuildPlainVi("1", "1|Alice|Leader|a|45|1|987|2|raw9|raw10")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "vI",
        "vI active numeric a");
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
    parts[0] = "1";
    parts[1] = "name\\";
    parts[2] = "title";
    parts[3] = "1";
    parts[4] = "2";
    parts[5] = "0";
    parts[6] = "4";
    failures += ExpectFieldError(
        AddressBook::Parse(BuildPlain(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "gnxd",
        1U,
        "gnxd truncated charName legacy");

    parts[1] = "name";
    parts[2] = "title\\";
    failures += ExpectFieldError(
        AddressBook::Parse(BuildPlain(JoinParts(parts))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "gnxd",
        2U,
        "gnxd truncated ownTitle legacy");

    std::array<AddressBookSlot, AddressBook::SlotCount> invalidSlots = {};
    invalidSlots[0] = AddressBookSlot {1, Bytes({0x96}), "title", 1, 2, 0, 4};
    failures += ExpectFieldError(
        AddressBook(invalidSlots).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "gnxd",
        1U,
        "gnxd truncated charName GBK");

    invalidSlots[0] = AddressBookSlot {1, "name", Bytes({0x96}), 1, 2, 0, 4};
    failures += ExpectFieldError(
        AddressBook(invalidSlots).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "gnxd",
        2U,
        "gnxd truncated ownTitle GBK");

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
    failures += TestStrictPayloadFieldCount();
    failures += TestLegacyTextRoundtrip();
    failures += TestAddressBookInfoActiveRoundtrip();
    failures += TestAddressBookInfoInactive();
    failures += TestAddressBookInfoRawEscapePreserved();
    failures += TestAddressBookInfoPayloadCompatibility();
    failures += TestAddressBookInfoErrors();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
