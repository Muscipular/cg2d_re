#include "packet/PacketBuilder.hpp"
#include "packet/receive.hpp"

#include <cstddef>
#include <iostream>
#include <initializer_list>
#include <string>
#include <string_view>
#include <span>
#include <utility>
#include <vector>

namespace {

const cg::packet::PacketSpec& PacketSpec() noexcept
{
    return cg::packet::receive::AddressBookGuildInfo::Spec;
}

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plainTokens)
{
    return cg::packet::ParsePacket(plainTokens, PacketSpec());
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, PacketSpec());
}

using AddressBookGuildInfo = cg::packet::receive::AddressBookGuildInfo;
using AddressBookGuildInfoRecord = cg::packet::receive::AddressBookGuildInfoRecord;

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

cg::packet::PacketField Field(
    std::string_view name,
    cg::packet::PacketFieldType type,
    std::string value)
{
    cg::packet::PacketField field;
    field.name = std::string(name);
    field.type = type;
    field.value = std::move(value);
    return field;
}

cg::packet::Packet PacketOf(
    cg::packet::PacketDirection direction,
    std::initializer_list<cg::packet::PacketField> fields)
{
    cg::packet::Packet packet(
        cg::packet::PacketCode(AddressBookGuildInfo::Code),
        direction);
    packet.fields.assign(fields.begin(), fields.end());
    return packet;
}

std::string BuildRawPlain(std::string_view payload)
{
    std::string plain = "PhdQ ";
    plain.append(payload);
    plain.push_back(' ');
    return plain;
}

std::string BuildPayload(std::size_t recordCount)
{
    std::vector<std::string> parts;
    parts.reserve(recordCount * AddressBookGuildInfo::RecordFieldCount);
    for (std::size_t index = 0; index < recordCount; ++index) {
        parts.push_back("Guild");
        parts.push_back("Title");
    }

    const cg::packet::PacketResult<std::string> payload =
        cg::packet::BuildPayload(std::span<const std::string>(parts.data(), parts.size()), "|");
    return payload.HasValue() ? payload.Value() : std::string();
}

int ExpectAddressBookGuildInfoPacket(
    const cg::packet::Packet& packet,
    std::string_view payload,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(AddressBookGuildInfo::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == cg::packet::PacketFieldType::EscapedString, message);
    failures += Expect(packet.fields[0].value == payload, message);
    return failures;
}

int TestParsePacketOuter()
{
    const cg::packet::PacketResult<cg::packet::Packet> parsed =
        ParsePacket("PhdQ Guild|Leader ");

    int failures = Expect(static_cast<bool>(parsed), "parse PhdQ outer");
    if (parsed.HasValue()) {
        failures += ExpectAddressBookGuildInfoPacket(parsed.Value(), "Guild|Leader", "parse PhdQ outer packet");
    }
    return failures;
}

int TestOneRecordRoundtrip()
{
    const AddressBookGuildInfo guildInfo(std::vector<AddressBookGuildInfoRecord> {{"Guild", "Leader"}});

    const cg::packet::PacketResult<std::string> data = guildInfo.BuildData();
    int failures = Expect(static_cast<bool>(data), "PhdQ one record BuildData");
    if (data.HasValue()) {
        failures += Expect(data.Value() == "Guild|Leader", "PhdQ one record payload");
    }

    const cg::packet::PacketResult<cg::packet::Packet> packet = guildInfo.ToPacket();
    failures += Expect(static_cast<bool>(packet), "PhdQ one record ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }
    failures += ExpectAddressBookGuildInfoPacket(packet.Value(), "Guild|Leader", "PhdQ one record packet");

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build PhdQ packet");
    if (!built.HasValue()) {
        return failures;
    }
    failures += Expect(built.Value() == "PhdQ Guild|Leader ", "build PhdQ value");

    const cg::packet::PacketResult<AddressBookGuildInfo> parsed = AddressBookGuildInfo::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip PhdQ packet");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.size() == 1U, "PhdQ roundtrip count");
        if (parsed.Value().records.size() == 1U) {
            failures += Expect(parsed.Value().records[0].guildNameRaw == "Guild", "PhdQ guild name");
            failures += Expect(parsed.Value().records[0].guildTitleRaw == "Leader", "PhdQ guild title");
        }
    }
    return failures;
}

int TestFinalDelimiterCompatible()
{
    int failures = 0;
    const cg::packet::PacketResult<AddressBookGuildInfo> one =
        AddressBookGuildInfo::Parse(BuildRawPlain("Guild|Title|"));
    failures += Expect(static_cast<bool>(one), "PhdQ final delimiter one record");
    if (one.HasValue()) {
        failures += Expect(one.Value().records.size() == 1U, "PhdQ final delimiter count");
        const cg::packet::PacketResult<std::string> data = one.Value().BuildData();
        failures += Expect(data.HasValue(), "PhdQ final delimiter BuildData");
        if (data.HasValue()) {
            failures += Expect(data.Value() == "Guild|Title", "PhdQ drops extra final delimiter");
        }
    }

    const cg::packet::PacketResult<AddressBookGuildInfo> two =
        AddressBookGuildInfo::Parse(BuildRawPlain("Guild|Title|Other|"));
    failures += Expect(static_cast<bool>(two), "PhdQ two records with empty title");
    if (two.HasValue()) {
        failures += Expect(two.Value().records.size() == 2U, "PhdQ two record count");
        if (two.Value().records.size() == 2U) {
            failures += Expect(two.Value().records[1].guildNameRaw == "Other", "PhdQ second guild name");
            failures += Expect(two.Value().records[1].guildTitleRaw.empty(), "PhdQ second empty title");
        }
    }
    return failures;
}

int TestEmptyDataAndSlot()
{
    int failures = 0;
    const cg::packet::PacketResult<AddressBookGuildInfo> empty = AddressBookGuildInfo::Parse("PhdQ  ");
    failures += Expect(static_cast<bool>(empty), "PhdQ empty data");
    if (empty.HasValue()) {
        failures += Expect(empty.Value().records.empty(), "PhdQ empty record count");
        const cg::packet::PacketResult<std::string> data = empty.Value().BuildData();
        failures += Expect(data.HasValue(), "PhdQ empty BuildData");
        if (data.HasValue()) {
            failures += Expect(data.Value().empty(), "PhdQ empty payload");
        }
    }

    const cg::packet::PacketResult<AddressBookGuildInfo> emptySlot = AddressBookGuildInfo::Parse(BuildRawPlain("|"));
    failures += Expect(static_cast<bool>(emptySlot), "PhdQ empty slot");
    if (emptySlot.HasValue()) {
        failures += Expect(emptySlot.Value().records.size() == 1U, "PhdQ empty slot count");
        if (emptySlot.Value().records.size() == 1U) {
            failures += Expect(emptySlot.Value().records[0].guildNameRaw.empty(), "PhdQ empty slot name");
            failures += Expect(emptySlot.Value().records[0].guildTitleRaw.empty(), "PhdQ empty slot title");
        }
        const cg::packet::PacketResult<std::string> data = emptySlot.Value().BuildData();
        failures += Expect(data.HasValue(), "PhdQ empty slot BuildData");
        if (data.HasValue()) {
            failures += Expect(data.Value() == "|", "PhdQ empty slot payload");
        }
    }
    return failures;
}

int TestRawEscapePreserved()
{
    const cg::packet::PacketResult<AddressBookGuildInfo> parsed =
        AddressBookGuildInfo::Parse(BuildRawPlain("A\\zB|C\\cD"));
    int failures = Expect(static_cast<bool>(parsed), "PhdQ raw escape parse");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "PhdQ raw record count");
    if (parsed.Value().records.size() != 1U) {
        return failures;
    }

    failures += Expect(parsed.Value().records[0].guildNameRaw == "A\\zB", "PhdQ raw guild name");
    failures += Expect(parsed.Value().records[0].guildTitleRaw == "C\\cD", "PhdQ raw guild title");

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += Expect(static_cast<bool>(packet), "PhdQ raw ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "PhdQ raw BuildPacket");
    if (built.HasValue()) {
        failures += Expect(built.Value() == "PhdQ A\\\\zB|C\\\\cD ", "PhdQ raw escaped BuildPacket");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectError(
        AddressBookGuildInfo::Parse("gnxd  "),
        cg::packet::PacketErrorCode::InvalidCode,
        "gnxd",
        "PhdQ wrong code");
    failures += ExpectError(
        AddressBookGuildInfo::Parse("PhdQ"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "PhdQ",
        "PhdQ missing field");
    failures += ExpectError(
        AddressBookGuildInfo::Parse("PhdQ a b"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "PhdQ",
        "PhdQ extra field");
    failures += ExpectError(
        AddressBookGuildInfo::Parse(BuildRawPlain("A|B|C")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "PhdQ",
        "PhdQ odd payload fields");
    failures += ExpectError(
        AddressBookGuildInfo::Parse(BuildRawPlain(BuildPayload(AddressBookGuildInfo::MaxRecordCount + 1U))),
        cg::packet::PacketErrorCode::DecodeFailed,
        "PhdQ",
        "PhdQ too many records");

    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ClientToServer,
            {Field("data", cg::packet::PacketFieldType::EscapedString, "Guild|Leader")})),
        cg::packet::PacketErrorCode::EncodeFailed,
        "PhdQ",
        "PhdQ manual direction mismatch");
    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("wrong", cg::packet::PacketFieldType::EscapedString, "Guild|Leader")})),
        cg::packet::PacketErrorCode::EncodeFailed,
        "PhdQ",
        "PhdQ manual field name mismatch");
    failures += ExpectError(
        BuildPacket(PacketOf(
            cg::packet::PacketDirection::ServerToClient,
            {Field("data", cg::packet::PacketFieldType::RawString, "Guild|Leader")})),
        cg::packet::PacketErrorCode::EncodeFailed,
        "PhdQ",
        "PhdQ manual field type mismatch");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestOneRecordRoundtrip();
    failures += TestFinalDelimiterCompatible();
    failures += TestEmptyDataAndSlot();
    failures += TestRawEscapePreserved();
    failures += TestErrors();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
