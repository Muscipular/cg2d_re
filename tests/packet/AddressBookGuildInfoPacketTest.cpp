#include "packet/Packet.hpp"

#include <algorithm>
#include <array>
#include <cstddef>
#include <iostream>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

using AddressBookGuildInfo = cg::packet::receive::AddressBookGuildInfo;
using AddressBookGuildInfoRecord = cg::packet::receive::AddressBookGuildInfoRecord;
using AddressBookGuildInfoRecords =
    std::array<AddressBookGuildInfoRecord, AddressBookGuildInfo::SlotCount>;

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
    int failures = Expect(!result.HasValue(), message);
    failures += Expect(result.HasError(), message);
    if (!result.HasError()) {
        return failures;
    }

    failures += Expect(result.Error().code == code, message);
    failures += Expect(result.Error().packetCode.has_value(), message);
    if (result.Error().packetCode.has_value()) {
        failures += Expect(result.Error().packetCode->View() == packetCode, message);
    }
    return failures;
}

template <typename T>
int ExpectFieldError(
    const cg::packet::PacketResult<T>& result,
    cg::packet::PacketErrorCode code,
    std::size_t fieldIndex,
    const char* message)
{
    int failures = ExpectError(result, code, AddressBookGuildInfo::Code, message);
    if (!result.HasError()) {
        return failures;
    }

    failures += Expect(result.Error().fieldIndex.has_value(), message);
    if (result.Error().fieldIndex.has_value()) {
        failures += Expect(result.Error().fieldIndex.value() == fieldIndex, message);
    }
    return failures;
}

std::string LegacyDoubleByteText(unsigned char trail, std::string_view suffix)
{
    std::string text;
    text.push_back(static_cast<char>(0x81U));
    text.push_back(static_cast<char>(trail));
    text.append(suffix);
    return text;
}

std::string JoinParts(const std::vector<std::string>& parts)
{
    std::string payload;
    for (std::size_t index = 0U; index < parts.size(); ++index) {
        if (index != 0U) {
            payload.push_back('|');
        }
        payload.append(parts[index]);
    }
    return payload;
}

std::string BuildRawPlain(std::string_view payload)
{
    std::string plain("PhdQ ");
    plain.append(payload);
    plain.push_back(' ');
    return plain;
}

std::string BuildPlain(std::string_view payload)
{
    const cg::packet::PacketResult<std::string> encoded =
        cg::packet::EncodeEscapedString(payload);
    if (!encoded.HasValue()) {
        return BuildRawPlain("");
    }
    return BuildRawPlain(encoded.Value());
}

cg::packet::PacketResult<cg::packet::Packet> ParsePacket(std::string_view plain)
{
    return cg::packet::ParsePacket(plain, AddressBookGuildInfo::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, AddressBookGuildInfo::Spec);
}

int TestSpecAndRoundtrip()
{
    AddressBookGuildInfoRecords records = {};
    records[0].guildNameRaw = "Guild";
    const AddressBookGuildInfo value(std::move(records));
    const cg::packet::PacketResult<std::string> data = value.BuildData();

    int failures = Expect(AddressBookGuildInfo::Spec.fieldCount == 1U, "PhdQ Spec exact1");
    failures += Expect(
        AddressBookGuildInfo::Fields[0].type == cg::packet::PacketFieldType::SubPayload,
        "PhdQ Spec data SubPayload");
    failures += Expect(data.HasValue(), "PhdQ BuildData");
    if (!data.HasValue()) {
        return failures;
    }

    failures += Expect(
        std::count(data.Value().begin(), data.Value().end(), '|') ==
            static_cast<std::ptrdiff_t>(AddressBookGuildInfo::PayloadFieldCount - 1U),
        "PhdQ fixed 119 delimiters");

    const cg::packet::PacketResult<cg::packet::Packet> packet = value.ToPacket();
    failures += Expect(packet.HasValue(), "PhdQ ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    failures += Expect(packet.Value().fields.size() == 1U, "PhdQ ToPacket exact1");
    if (packet.Value().fields.size() == 1U) {
        failures += Expect(packet.Value().fields[0].name == "data", "PhdQ data name");
        failures += Expect(
            packet.Value().fields[0].type == cg::packet::PacketFieldType::SubPayload,
            "PhdQ ToPacket data SubPayload");
        failures += Expect(packet.Value().fields[0].value == data.Value(), "PhdQ ToPacket data value");
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(built.HasValue(), "PhdQ generic build");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::Packet> generic = ParsePacket(built.Value());
    failures += Expect(generic.HasValue(), "PhdQ generic parse");
    if (generic.HasValue() && generic.Value().fields.size() == 1U) {
        failures += Expect(
            generic.Value().fields[0].type == cg::packet::PacketFieldType::EscapedString,
            "PhdQ runtime data EscapedString");
    }

    const cg::packet::PacketResult<AddressBookGuildInfo> parsed =
        AddressBookGuildInfo::Parse(built.Value());
    failures += Expect(parsed.HasValue(), "PhdQ typed roundtrip");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.size() == AddressBookGuildInfo::SlotCount, "PhdQ 60 slots");
        failures += Expect(parsed.Value().records[0].guildNameRaw == "Guild", "PhdQ first guild name");
        failures += Expect(parsed.Value().records[0].guildTitleRaw.empty(), "PhdQ Server title empty");
    }
    return failures;
}

int TestFixedSlots()
{
    const AddressBookGuildInfo empty;
    const cg::packet::PacketResult<std::string> emptyData = empty.BuildData();
    int failures = Expect(emptyData.HasValue(), "PhdQ all-empty BuildData");
    if (emptyData.HasValue()) {
        failures += Expect(
            emptyData.Value() == std::string(AddressBookGuildInfo::PayloadFieldCount - 1U, '|'),
            "PhdQ all-empty fixed table");
        const cg::packet::PacketResult<AddressBookGuildInfo> parsed =
            AddressBookGuildInfo::Parse(BuildPlain(emptyData.Value()));
        failures += Expect(parsed.HasValue(), "PhdQ all-empty Parse");
        if (parsed.HasValue()) {
            failures += Expect(parsed.Value().records[59].guildNameRaw.empty(), "PhdQ last empty name");
            failures += Expect(parsed.Value().records[59].guildTitleRaw.empty(), "PhdQ last empty title");
        }
    }

    AddressBookGuildInfoRecords records = {};
    records[59] = AddressBookGuildInfoRecord {"LastGuild", "ConsumerTitle"};
    const AddressBookGuildInfo last(std::move(records));
    const cg::packet::PacketResult<std::string> lastData = last.BuildData();
    failures += Expect(lastData.HasValue(), "PhdQ last slot BuildData");
    if (lastData.HasValue()) {
        failures += Expect(
            lastData.Value().ends_with("LastGuild|ConsumerTitle"),
            "PhdQ last slot field order");
        const cg::packet::PacketResult<AddressBookGuildInfo> parsed =
            AddressBookGuildInfo::Parse(BuildPlain(lastData.Value()));
        failures += Expect(parsed.HasValue(), "PhdQ last slot Parse");
        if (parsed.HasValue()) {
            failures += Expect(parsed.Value().records[59].guildNameRaw == "LastGuild", "PhdQ last name");
            failures += Expect(
                parsed.Value().records[59].guildTitleRaw == "ConsumerTitle",
                "PhdQ nonempty title consumer compatibility");
        }
    }
    return failures;
}

int TestLegacyTextRoundtrip()
{
    const std::string semanticName =
        LegacyDoubleByteText(0x7CU, "A|B,C\\D\nE");
    const std::string semanticTitle =
        LegacyDoubleByteText(0x5CU, "T|U");
    AddressBookGuildInfoRecords records = {};
    records[0] = AddressBookGuildInfoRecord {semanticName, semanticTitle};
    const AddressBookGuildInfo value(std::move(records));

    const cg::packet::PacketResult<std::string> data = value.BuildData();
    int failures = Expect(data.HasValue(), "PhdQ legacy BuildData");
    if (!data.HasValue()) {
        return failures;
    }

    failures += Expect(data.Value().find("\\z") != std::string::npos, "PhdQ legacy pipe encoded");
    failures += Expect(data.Value().find("\\c") != std::string::npos, "PhdQ legacy comma encoded");
    failures += Expect(data.Value().find("\\y") != std::string::npos, "PhdQ legacy slash encoded");
    failures += Expect(data.Value().find("\\n") != std::string::npos, "PhdQ legacy newline encoded");

    const cg::packet::PacketResult<AddressBookGuildInfo> parsed =
        AddressBookGuildInfo::Parse(BuildPlain(data.Value()));
    failures += Expect(parsed.HasValue(), "PhdQ legacy Parse");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records[0].guildNameRaw == semanticName, "PhdQ semantic name");
        failures += Expect(parsed.Value().records[0].guildTitleRaw == semanticTitle, "PhdQ semantic title");
    }
    return failures;
}

int TestStrictFieldCount()
{
    int failures = 0;
    for (const std::size_t fieldCount : {0U, 119U, 121U}) {
        const std::vector<std::string> parts(fieldCount);
        failures += ExpectError(
            AddressBookGuildInfo::Parse(BuildPlain(JoinParts(parts))),
            cg::packet::PacketErrorCode::FieldCountMismatch,
            "PhdQ",
            "PhdQ rejects non-120 field payload");
    }

    std::string extraDelimiter(AddressBookGuildInfo::PayloadFieldCount - 1U, '|');
    extraDelimiter.push_back('|');
    failures += ExpectError(
        AddressBookGuildInfo::Parse(BuildPlain(extraDelimiter)),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "PhdQ",
        "PhdQ rejects extra trailing delimiter");
    return failures;
}

int TestLegacyErrors()
{
    AddressBookGuildInfoRecords badName = {};
    badName[0].guildNameRaw.assign(1U, static_cast<char>(0x81U));
    int failures = ExpectFieldError(
        AddressBookGuildInfo(std::move(badName)).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        0U,
        "PhdQ build truncated guild name");

    AddressBookGuildInfoRecords badTitle = {};
    badTitle[59].guildTitleRaw.assign(1U, static_cast<char>(0x81U));
    failures += ExpectFieldError(
        AddressBookGuildInfo(std::move(badTitle)).BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        119U,
        "PhdQ build truncated guild title");

    std::vector<std::string> trailingEscape(AddressBookGuildInfo::PayloadFieldCount);
    trailingEscape[0] = "Bad\\";
    failures += ExpectFieldError(
        AddressBookGuildInfo::Parse(BuildPlain(JoinParts(trailingEscape))),
        cg::packet::PacketErrorCode::DecodeFailed,
        0U,
        "PhdQ parse truncated legacy escape");
    return failures;
}

int TestOuterErrors()
{
    const std::string payload(AddressBookGuildInfo::PayloadFieldCount - 1U, '|');
    int failures = ExpectError(
        AddressBookGuildInfo::Parse("gnxd "),
        cg::packet::PacketErrorCode::InvalidCode,
        "gnxd",
        "PhdQ wrong code");
    failures += Expect(
        !AddressBookGuildInfo::Parse("PhdQ").HasValue(),
        "PhdQ missing data");
    failures += Expect(
        !AddressBookGuildInfo::Parse(BuildPlain(payload) + "extra ").HasValue(),
        "PhdQ extra outer field");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestSpecAndRoundtrip();
    failures += TestFixedSlots();
    failures += TestLegacyTextRoundtrip();
    failures += TestStrictFieldCount();
    failures += TestLegacyErrors();
    failures += TestOuterErrors();
    if (failures != 0) {
        std::cerr << failures << " PhdQ expectations failed\n";
        return 1;
    }
    return 0;
}
