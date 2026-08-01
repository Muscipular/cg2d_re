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
    return cg::packet::ParsePacket(plainTokens, cg::packet::receive::ItemFrontCharList::Spec);
}

cg::packet::PacketResult<std::string> BuildPacket(const cg::packet::Packet& packet)
{
    return cg::packet::BuildPacket(packet, cg::packet::receive::ItemFrontCharList::Spec);
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
int ExpectFieldError(
    const cg::packet::PacketResult<T>& result,
    cg::packet::PacketErrorCode code,
    std::string_view packetCode,
    std::size_t fieldIndex,
    const char* message)
{
    int failures = ExpectError(result, code, packetCode, message);
    if (!result.HasError()) {
        return failures;
    }

    failures += Expect(result.Error().fieldIndex.has_value(), message);
    if (result.Error().fieldIndex.has_value()) {
        failures += Expect(result.Error().fieldIndex.value() == fieldIndex, message);
    }
    return failures;
}

std::string EncodeLegacy(std::string_view value)
{
    const cg::packet::PacketResult<std::string> encoded =
        cg::packet::EncodeLegacyPayloadText(value);
    return encoded.HasValue() ? encoded.Value() : std::string();
}

std::string WithGbkTrailPipe(std::string_view prefix)
{
    std::string value(prefix);
    value.push_back(static_cast<char>(0x96));
    value.push_back(static_cast<char>(0x7C));
    return value;
}

std::string BuildRecordsData(
    const std::vector<cg::packet::receive::FrontCharRecord>& records)
{
    std::string data;
    for (const cg::packet::receive::FrontCharRecord& record : records) {
        data.append(EncodeLegacy(record.name));
        data.push_back('|');
        data.append(std::to_string(record.nonPlayerFlag));
        data.push_back('|');
    }
    return data;
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

std::string BuildPlainLy(std::string_view payload)
{
    return BuildPlain(cg::packet::receive::ItemFrontCharList::Code, payload);
}

int ExpectItemFrontCharListPacket(
    const cg::packet::Packet& packet,
    std::string_view value,
    cg::packet::PacketFieldType dataType,
    const char* message)
{
    int failures = 0;
    failures += Expect(packet.code == cg::packet::PacketCode(cg::packet::receive::ItemFrontCharList::Code), message);
    failures += Expect(packet.direction == cg::packet::PacketDirection::ServerToClient, message);
    failures += Expect(packet.fields.size() == 1U, message);
    if (packet.fields.size() != 1U) {
        return failures;
    }

    failures += Expect(packet.fields[0].name == "data", message);
    failures += Expect(packet.fields[0].type == dataType, message);
    failures += Expect(packet.fields[0].value == value, message);
    return failures;
}

int ExpectItemFrontCharListPacket(
    const cg::packet::PacketResult<cg::packet::Packet>& packet,
    std::string_view value,
    cg::packet::PacketFieldType dataType,
    const char* message)
{
    int failures = Expect(static_cast<bool>(packet), message);
    if (packet.HasValue()) {
        failures += ExpectItemFrontCharListPacket(packet.Value(), value, dataType, message);
    }
    return failures;
}

int ExpectBuildData(
    const cg::packet::receive::ItemFrontCharList& itemFrontCharList,
    std::string_view value,
    const char* message)
{
    const cg::packet::PacketResult<std::string> data = itemFrontCharList.BuildData();
    int failures = Expect(static_cast<bool>(data), message);
    if (data.HasValue()) {
        failures += Expect(data.Value() == value, message);
    }
    return failures;
}

int ExpectRecord(
    const cg::packet::receive::FrontCharRecord& record,
    std::string_view name,
    int nonPlayerFlag,
    const char* message)
{
    int failures = 0;
    failures += Expect(record.name == name, message);
    failures += Expect(record.nonPlayerFlag == nonPlayerFlag, message);
    return failures;
}

int TestParsePacketOuter()
{
    const std::string payload = "Alice|0|Pet|1|";
    const cg::packet::PacketResult<cg::packet::Packet> parsed = ParsePacket(BuildPlainLy(payload));

    int failures = Expect(
        cg::packet::receive::ItemFrontCharList::Fields[0U].type ==
            cg::packet::PacketFieldType::SubPayload,
        "LY data spec SubPayload");
    failures += Expect(static_cast<bool>(parsed), "parse LY outer");
    if (parsed.HasValue()) {
        failures += ExpectItemFrontCharListPacket(
            parsed.Value(),
            payload,
            cg::packet::PacketFieldType::EscapedString,
            "parse LY outer packet");
    }
    return failures;
}

int TestEmptyData()
{
    const cg::packet::PacketResult<cg::packet::receive::ItemFrontCharList> parsed =
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy(""));

    int failures = Expect(static_cast<bool>(parsed), "parse LY empty data");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.empty(), "LY empty data record count");
        failures += ExpectBuildData(parsed.Value(), "", "LY empty data BuildData");
        failures += ExpectItemFrontCharListPacket(
            parsed.Value().ToPacket(),
            "",
            cg::packet::PacketFieldType::SubPayload,
            "LY empty data ToPacket");
    }
    return failures;
}

int TestTwoRecords()
{
    const std::string payload = "Alice|0|Pet|1|";
    const cg::packet::PacketResult<cg::packet::receive::ItemFrontCharList> parsed =
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse LY two records");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 2U, "LY two record count");
    if (parsed.Value().records.size() == 2U) {
        failures += ExpectRecord(parsed.Value().records[0], "Alice", 0, "LY first record");
        failures += ExpectRecord(parsed.Value().records[1], "Pet", 1, "LY second record");
    }
    failures += ExpectBuildData(parsed.Value(), payload, "LY two records BuildData");
    return failures;
}

int TestNineRecords()
{
    std::vector<cg::packet::receive::FrontCharRecord> records;
    records.reserve(9U);
    for (int index = 0; index < 9; ++index) {
        records.push_back(cg::packet::receive::FrontCharRecord {
            index == 8 ? WithGbkTrailPipe("Name8") : "Name" + std::to_string(index),
            index % 2,
        });
    }
    const std::string lastName = records.back().name;
    const std::string payload = BuildRecordsData(records);
    const cg::packet::receive::ItemFrontCharList value(std::move(records));

    int failures = ExpectBuildData(value, payload, "LY nine records BuildData");
    const cg::packet::PacketResult<cg::packet::Packet> packet = value.ToPacket();
    failures += ExpectItemFrontCharListPacket(
        packet,
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "LY nine records ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build LY nine records packet");
    if (!built.HasValue()) {
        return failures;
    }
    const cg::packet::PacketResult<cg::packet::receive::ItemFrontCharList> parsed =
        cg::packet::receive::ItemFrontCharList::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "parse LY nine records");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.size() == 9U, "LY nine-record upper bound");
        if (parsed.Value().records.size() == 9U) {
            failures += ExpectRecord(parsed.Value().records[8], lastName, 0, "LY ninth semantic record");
        }
    }
    return failures;
}

int TestOuterSpaceNameRoundtrip()
{
    const std::string payload = "  Alice Bob  |0|";
    const cg::packet::PacketResult<cg::packet::receive::ItemFrontCharList> parsed =
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse LY spaced name");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "LY spaced name record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(parsed.Value().records[0], "  Alice Bob  ", 0, "LY spaced name record");
    }
    failures += ExpectBuildData(parsed.Value(), payload, "LY spaced name BuildData");

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += ExpectItemFrontCharListPacket(
        packet,
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "LY spaced name ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build LY spaced name packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainLy(payload), "LY spaced name roundtrip");
    }
    return failures;
}

int TestSemanticNameRoundtrip()
{
    std::string name = "A|B,C\\D\nE";
    name.push_back(static_cast<char>(0x96));
    name.push_back(static_cast<char>(0x7C));
    std::string payload = "A\\zB\\cC\\yD\\nE";
    payload.push_back(static_cast<char>(0x96));
    payload.push_back(static_cast<char>(0x7C));
    payload.append("|0|");
    const cg::packet::PacketResult<cg::packet::receive::ItemFrontCharList> parsed =
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy(payload));

    int failures = Expect(static_cast<bool>(parsed), "parse LY semantic GBK/legacy name");
    if (!parsed.HasValue()) {
        return failures;
    }

    failures += Expect(parsed.Value().records.size() == 1U, "LY one record count");
    if (parsed.Value().records.size() == 1U) {
        failures += ExpectRecord(parsed.Value().records[0], name, 0, "LY semantic name record");
    }
    failures += ExpectBuildData(parsed.Value(), payload, "LY semantic name BuildData");

    const cg::packet::PacketResult<cg::packet::Packet> packet = parsed.Value().ToPacket();
    failures += ExpectItemFrontCharListPacket(
        packet,
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "LY semantic name ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build LY semantic name packet");
    if (built.HasValue()) {
        failures += Expect(built.Value() == BuildPlainLy(payload), "LY semantic name roundtrip");
    }
    return failures;
}

int TestConstructRoundtrip()
{
    std::vector<cg::packet::receive::FrontCharRecord> records;
    records.push_back(cg::packet::receive::FrontCharRecord {"Alice", 0});
    records.push_back(cg::packet::receive::FrontCharRecord {"Pet", 1});
    const cg::packet::receive::ItemFrontCharList itemFrontCharList(std::move(records));
    const std::string payload = "Alice|0|Pet|1|";

    const cg::packet::PacketResult<cg::packet::Packet> packet = itemFrontCharList.ToPacket();
    int failures = ExpectItemFrontCharListPacket(
        packet,
        payload,
        cg::packet::PacketFieldType::SubPayload,
        "LY ToPacket");
    if (!packet.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<std::string> built = BuildPacket(packet.Value());
    failures += Expect(static_cast<bool>(built), "build LY packet");
    if (!built.HasValue()) {
        return failures;
    }

    const cg::packet::PacketResult<cg::packet::receive::ItemFrontCharList> parsed =
        cg::packet::receive::ItemFrontCharList::Parse(built.Value());
    failures += Expect(static_cast<bool>(parsed), "roundtrip LY packet");
    if (parsed.HasValue()) {
        failures += Expect(parsed.Value().records.size() == 2U, "roundtrip LY record count");
        failures += ExpectBuildData(parsed.Value(), payload, "roundtrip LY payload");
    }
    return failures;
}

int TestErrors()
{
    int failures = 0;
    failures += ExpectFieldError(
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy("Alice|0")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "LY",
        0U,
        "LY missing trailing delimiter");
    failures += ExpectFieldError(
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy("Alice|0|Bob|")),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "LY",
        0U,
        "LY odd payload fields");
    failures += ExpectFieldError(
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy("Alice||")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "LY",
        1U,
        "LY nonPlayerFlag rejects empty");
    failures += ExpectFieldError(
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy("Alice|x|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "LY",
        1U,
        "LY nonPlayerFlag non-decimal");
    failures += ExpectFieldError(
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy("Alice|2|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "LY",
        1U,
        "LY nonPlayerFlag rejects 2");
    failures += ExpectFieldError(
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy("Alice|-1|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "LY",
        1U,
        "LY nonPlayerFlag rejects -1");
    failures += ExpectError(
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlain("ysVc", "Alice|0|")),
        cg::packet::PacketErrorCode::InvalidCode,
        "ysVc",
        "LY rejects wrong code");
    failures += ExpectFieldError(
        cg::packet::receive::ItemFrontCharList::Parse("LY"),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "LY",
        0U,
        "LY outer missing data index");
    failures += ExpectFieldError(
        cg::packet::receive::ItemFrontCharList::Parse("LY data extra "),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "LY",
        1U,
        "LY outer extra field index");
    failures += ExpectFieldError(
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy("Alice|0|Pet|2|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "LY",
        3U,
        "LY second flag field index");
    failures += ExpectFieldError(
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy("Name\\|0|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "LY",
        0U,
        "LY first name legacy truncated index");
    failures += ExpectFieldError(
        cg::packet::receive::ItemFrontCharList::Parse(BuildPlainLy("Alice|0|Name\\|1|")),
        cg::packet::PacketErrorCode::DecodeFailed,
        "LY",
        2U,
        "LY second name legacy truncated index");

    const std::vector<cg::packet::receive::FrontCharRecord> tenRecords(
        10U,
        cg::packet::receive::FrontCharRecord {"Name", 0});
    failures += ExpectFieldError(
        cg::packet::receive::ItemFrontCharList::Parse(
            BuildPlainLy(BuildRecordsData(tenRecords))),
        cg::packet::PacketErrorCode::FieldCountMismatch,
        "LY",
        0U,
        "LY parse tenth record rejected");
    return failures;
}

int TestBuildRejectsInvalidNonPlayerFlag()
{
    int failures = 0;
    const cg::packet::receive::ItemFrontCharList itemFrontCharListWithTwo(
        {cg::packet::receive::FrontCharRecord {"Alice", 2}});
    failures += ExpectFieldError(
        itemFrontCharListWithTwo.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "LY",
        1U,
        "LY BuildData rejects 2");
    failures += ExpectFieldError(
        itemFrontCharListWithTwo.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "LY",
        1U,
        "LY ToPacket rejects 2");

    const cg::packet::receive::ItemFrontCharList itemFrontCharListWithNegative(
        {cg::packet::receive::FrontCharRecord {"Alice", -1}});
    failures += ExpectFieldError(
        itemFrontCharListWithNegative.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "LY",
        1U,
        "LY BuildData rejects -1");
    failures += ExpectFieldError(
        itemFrontCharListWithNegative.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "LY",
        1U,
        "LY ToPacket rejects -1");

    std::string truncatedName(1U, static_cast<char>(0x96));
    const cg::packet::receive::ItemFrontCharList itemFrontCharListWithTruncatedName({
        cg::packet::receive::FrontCharRecord {"Alice", 0},
        cg::packet::receive::FrontCharRecord {std::move(truncatedName), 1},
    });
    failures += ExpectFieldError(
        itemFrontCharListWithTruncatedName.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "LY",
        2U,
        "LY second name BuildData encoding failure index");
    failures += ExpectFieldError(
        itemFrontCharListWithTruncatedName.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "LY",
        2U,
        "LY second name ToPacket encoding failure index");

    const std::vector<cg::packet::receive::FrontCharRecord> tenRecords(
        10U,
        cg::packet::receive::FrontCharRecord {"Name", 0});
    const cg::packet::receive::ItemFrontCharList itemFrontCharListWithTen(tenRecords);
    failures += ExpectFieldError(
        itemFrontCharListWithTen.BuildData(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "LY",
        0U,
        "LY BuildData rejects tenth record");
    failures += ExpectFieldError(
        itemFrontCharListWithTen.ToPacket(),
        cg::packet::PacketErrorCode::EncodeFailed,
        "LY",
        0U,
        "LY ToPacket rejects tenth record");
    return failures;
}

} // namespace

int main()
{
    int failures = 0;
    failures += TestParsePacketOuter();
    failures += TestEmptyData();
    failures += TestTwoRecords();
    failures += TestNineRecords();
    failures += TestOuterSpaceNameRoundtrip();
    failures += TestSemanticNameRoundtrip();
    failures += TestConstructRoundtrip();
    failures += TestErrors();
    failures += TestBuildRejectsInvalidNonPlayerFlag();
    if (failures != 0) {
        std::cerr << failures << " checks failed\n";
        return 1;
    }

    return 0;
}
